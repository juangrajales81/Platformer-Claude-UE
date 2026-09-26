#include "LevelBuilder.h"
#include "Platformer.h"
#include "PlatformerBlocks.h"
#include "PlatformerEnemy.h"
#include "PlatformerPickups.h"
#include "PlatformerVisuals.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

using PlatformerVisuals::TileSize;

ALevelBuilder::ALevelBuilder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// El suelo es más profundo que las plataformas para dar sensación de volumen.
	GrassTiles = CreateTileLayer(TEXT("GrassTiles"), 3.f);
	DirtTiles  = CreateTileLayer(TEXT("DirtTiles"), 3.f);
	StoneTiles = CreateTileLayer(TEXT("StoneTiles"), 1.f);

	auto CreateWall = [this](FName Name)
	{
		UBoxComponent* Wall = CreateDefaultSubobject<UBoxComponent>(Name);
		Wall->SetupAttachment(RootComponent);
		Wall->SetBoxExtent(FVector(50.f, 500.f, 5000.f));
		Wall->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
		return Wall;
	};
	LeftWall = CreateWall(TEXT("LeftWall"));
	RightWall = CreateWall(TEXT("RightWall"));
}

UInstancedStaticMeshComponent* ALevelBuilder::CreateTileLayer(FName Name, float DepthScale)
{
	UInstancedStaticMeshComponent* Layer = CreateDefaultSubobject<UInstancedStaticMeshComponent>(Name);
	Layer->SetupAttachment(RootComponent);
	Layer->SetMobility(EComponentMobility::Movable);
	Layer->SetStaticMesh(PlatformerVisuals::Cube());
	Layer->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	Layer->SetCanEverAffectNavigation(false);
	// La escala de profundidad se guarda en la escala del componente para no repetirla en cada instancia.
	Layer->SetRelativeScale3D(FVector(1.f, DepthScale, 1.f));
	return Layer;
}

void ALevelBuilder::BeginPlay()
{
	Super::BeginPlay();

	PlatformerVisuals::Paint(GrassTiles, FLinearColor(0.25f, 0.65f, 0.15f));
	PlatformerVisuals::Paint(DirtTiles,  FLinearColor(0.45f, 0.25f, 0.1f));
	PlatformerVisuals::Paint(StoneTiles, FLinearColor(0.35f, 0.4f, 0.55f));
}

FString ALevelBuilder::GetLevelFilePath(int32 LevelNumber)
{
	return FPaths::ProjectContentDir() / TEXT("Levels") / FString::Printf(TEXT("Level%d.txt"), LevelNumber);
}

bool ALevelBuilder::LevelExists(int32 LevelNumber)
{
	return FPaths::FileExists(GetLevelFilePath(LevelNumber));
}

FVector ALevelBuilder::CellToWorld(int32 Column, int32 RowFromBottom) const
{
	return FVector(Column * TileSize, 0.f, RowFromBottom * TileSize);
}

void ALevelBuilder::Clear()
{
	GrassTiles->ClearInstances();
	DirtTiles->ClearInstances();
	StoneTiles->ClearInstances();

	for (AActor* Actor : SpawnedActors)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedActors.Reset();
}

FBuiltLevelInfo ALevelBuilder::Build(int32 LevelNumber)
{
	Clear();

	FBuiltLevelInfo Info;
	const FString Path = GetLevelFilePath(LevelNumber);

	TArray<FString> RawLines;
	if (!FFileHelper::LoadFileToStringArray(RawLines, *Path))
	{
		UE_LOG(LogPlatformer, Error, TEXT("No se pudo leer el nivel %s"), *Path);
		return Info;
	}

	TArray<FString> Lines;
	for (const FString& Line : RawLines)
	{
		if (!Line.StartsWith(TEXT(";")))
		{
			Lines.Add(Line.TrimEnd());
		}
	}
	// Las filas vacías al final no forman parte del mapa.
	while (Lines.Num() > 0 && Lines.Last().IsEmpty())
	{
		Lines.Pop();
	}

	Info.Rows = Lines.Num();
	for (const FString& Line : Lines)
	{
		Info.Columns = FMath::Max(Info.Columns, Line.Len());
	}

	auto CellAt = [&Lines](int32 Column, int32 Row) -> TCHAR
	{
		return Lines.IsValidIndex(Row) && Column < Lines[Row].Len() ? Lines[Row][Column] : TEXT('.');
	};

	for (int32 Row = 0; Row < Info.Rows; ++Row)
	{
		const int32 RowFromBottom = Info.Rows - 1 - Row;
		for (int32 Column = 0; Column < Lines[Row].Len(); ++Column)
		{
			const TCHAR Cell = Lines[Row][Column];
			const FVector Location = CellToWorld(Column, RowFromBottom);
			// El constructor está en el origen y las celdas en Y = 0, así que la escala de
			// profundidad de cada capa no altera la posición local de la instancia.
			const FTransform TileTransform(Location);

			switch (Cell)
			{
			case TEXT('#'):
				// Hierba si no hay tierra encima, tierra en el resto.
				(CellAt(Column, Row - 1) == TEXT('#') ? DirtTiles : GrassTiles)->AddInstance(TileTransform);
				break;
			case TEXT('='):
				StoneTiles->AddInstance(TileTransform);
				break;
			case TEXT('.'):
			case TEXT(' '):
				break;
			default:
				SpawnCellActor(Cell, Location, Info);
				break;
			}
		}
	}

	const float HalfTile = TileSize * 0.5f;
	LeftWall->SetWorldLocation(FVector(-TileSize - HalfTile, 0.f, 0.f));
	RightWall->SetWorldLocation(FVector(Info.Columns * TileSize + HalfTile, 0.f, 0.f));

	Info.bValid = true;
	UE_LOG(LogPlatformer, Log, TEXT("Nivel %d construido: %dx%d celdas"), LevelNumber, Info.Columns, Info.Rows);
	return Info;
}

void ALevelBuilder::SpawnCellActor(TCHAR Cell, const FVector& Location, FBuiltLevelInfo& Info)
{
	switch (Cell)
	{
	case TEXT('P'):
		Info.PlayerStart = Location;
		break;
	case TEXT('G'):
		Info.GoalLocation = Location;
		break;
	case TEXT('B'):
		SpawnLevelActor<ABrickBlock>(Location);
		break;
	case TEXT('?'):
		SpawnLevelActor<ABonusBlock>(Location);
		break;
	case TEXT('D'):
		SpawnLevelActor<ADiamond>(Location);
		break;
	case TEXT('E'):
		// Se baja a ras de suelo: la cápsula del enemigo es más baja que una celda.
		SpawnLevelActor<AEnemyWalker>(Location - FVector(0.f, 0.f, 10.f));
		break;
	case TEXT('^'):
		SpawnLevelActor<ASpikes>(Location);
		break;
	default:
		UE_LOG(LogPlatformer, Warning, TEXT("Carácter de nivel desconocido '%c'"), Cell);
		break;
	}
}
