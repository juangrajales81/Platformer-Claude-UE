#include "PlatformerGameMode.h"
#include "PlatformerCharacter.h"
#include "PlatformerEnvironment.h"
#include "Platformer.h"
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

	LevelBuilder = World->SpawnActor<ALevelBuilder>();
	CurrentLevelInfo = LevelBuilder->Build(CurrentLevel);
	if (CurrentLevelInfo.bValid)
	{
		// El centro de la celda 'P' está a media celda del suelo; se sube un poco la cápsula.
		PlayerStartLocation = CurrentLevelInfo.PlayerStart + FVector(0.f, 0.f, 10.f);
	}
	else
	{
		UE_LOG(LogPlatformer, Error, TEXT("No hay nivel %d: revisa Content/Levels"), CurrentLevel);
	}
}
