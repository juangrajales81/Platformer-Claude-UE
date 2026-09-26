#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelBuilder.generated.h"

class UInstancedStaticMeshComponent;
class UBoxComponent;

/** Datos de un nivel ya construido que necesita el GameMode. */
USTRUCT()
struct FBuiltLevelInfo
{
	GENERATED_BODY()

	bool bValid = false;
	/** Texto del primer comentario del archivo, p. ej. "Nivel 1 - Las colinas del sueño". */
	FString Title;
	FVector PlayerStart = FVector::ZeroVector;
	FVector GoalLocation = FVector::ZeroVector;
	int32 Columns = 0;
	int32 Rows = 0;
};

/**
 * Construye un nivel a partir de un mapa ASCII (Content/Levels/LevelN.txt).
 * Cada carácter es una celda de 1x1 m; la última fila del archivo está a Z = 0.
 * Las líneas que empiezan por ';' son comentarios. Leyenda en docs/LEVELS.md.
 */
UCLASS()
class PLATFORMER_API ALevelBuilder : public AActor
{
	GENERATED_BODY()

public:
	ALevelBuilder();

	/** Ruta absoluta del archivo de un nivel (1, 2, ...). */
	static FString GetLevelFilePath(int32 LevelNumber);
	static bool LevelExists(int32 LevelNumber);

	/** Borra el nivel actual y construye el indicado. */
	FBuiltLevelInfo Build(int32 LevelNumber);

	/** Elimina todos los bloques y actores creados por el constructor. */
	void Clear();

	/** Centro de una celda del mapa en coordenadas del mundo. */
	FVector CellToWorld(int32 Column, int32 RowFromBottom) const;

protected:
	virtual void BeginPlay() override;

	/** Crea el actor correspondiente a un carácter que no es geometría fija. */
	void SpawnCellActor(TCHAR Cell, const FVector& Location, FBuiltLevelInfo& Info);

	template <typename T>
	T* SpawnLevelActor(const FVector& Location)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		T* Actor = GetWorld()->SpawnActor<T>(Location, FRotator::ZeroRotator, Params);
		if (Actor)
		{
			SpawnedActors.Add(Actor);
		}
		return Actor;
	}

	UInstancedStaticMeshComponent* CreateTileLayer(FName Name, float DepthScale);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> GrassTiles;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> DirtTiles;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInstancedStaticMeshComponent> StoneTiles;

	/** Paredes invisibles en los extremos del nivel. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> LeftWall;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RightWall;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};
