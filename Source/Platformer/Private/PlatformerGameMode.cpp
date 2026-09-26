#include "PlatformerGameMode.h"
#include "PlatformerCharacter.h"
#include "PlatformerEnvironment.h"
#include "PlatformerVisuals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

APlatformerGameMode::APlatformerGameMode()
{
	DefaultPawnClass = APlatformerCharacter::StaticClass();
}

void APlatformerGameMode::StartPlay()
{
	BuildWorld();
	bWorldReady = true;

	Super::StartPlay();

	// Los jugadores que se conectaron antes de construir el mundo aparecen ahora.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && !PC->GetPawn())
		{
			SpawnPlayer(PC);
		}
	}
}

void APlatformerGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (bWorldReady)
	{
		SpawnPlayer(NewPlayer);
	}
}

void APlatformerGameMode::SpawnPlayer(AController* Controller)
{
	RestartPlayerAtTransform(Controller, FTransform(PlayerStartLocation));
}

void APlatformerGameMode::BuildWorld()
{
	UWorld* World = GetWorld();
	World->SpawnActor<APlatformerEnvironment>();

	// Suelo provisional para probar el movimiento (se sustituye por el constructor de niveles).
	FActorSpawnParameters Params;
	AStaticMeshActor* Floor = World->SpawnActor<AStaticMeshActor>(FVector(1000.f, 0.f, -50.f), FRotator::ZeroRotator, Params);
	UStaticMeshComponent* FloorMesh = Floor->GetStaticMeshComponent();
	FloorMesh->SetMobility(EComponentMobility::Movable);
	FloorMesh->SetStaticMesh(PlatformerVisuals::Cube());
	Floor->SetActorScale3D(FVector(30.f, 4.f, 1.f));
	PlatformerVisuals::Paint(FloorMesh, FLinearColor(0.3f, 0.6f, 0.2f));
}
