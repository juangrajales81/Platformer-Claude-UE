#include "PlatformerGameMode.h"
#include "PlatformerCharacter.h"
#include "PlatformerEnvironment.h"
#include "PlatformerHUD.h"
#include "Platformer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

APlatformerGameMode::APlatformerGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = APlatformerCharacter::StaticClass();
	HUDClass = APlatformerHUD::StaticClass();
}

void APlatformerGameMode::StartPlay()
{
	Lives = StartingLives;
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

void APlatformerGameMode::BuildWorld()
{
	UWorld* World = GetWorld();
	World->SpawnActor<APlatformerEnvironment>();
	LevelBuilder = World->SpawnActor<ALevelBuilder>();
	LoadLevel(CurrentLevel);
}

void APlatformerGameMode::LoadLevel(int32 LevelNumber)
{
	CurrentLevel = LevelNumber;
	CurrentLevelInfo = LevelBuilder->Build(LevelNumber);
	if (CurrentLevelInfo.bValid)
	{
		// El centro de la celda 'P' está a media celda del suelo; se sube un poco la cápsula.
		PlayerStartLocation = CurrentLevelInfo.PlayerStart + FVector(0.f, 0.f, 10.f);
	}
	else
	{
		UE_LOG(LogPlatformer, Error, TEXT("No hay nivel %d: revisa Content/Levels"), LevelNumber);
	}

	TimeLeft = LevelTimeLimit;
	LevelStartTime = GetWorld()->GetTimeSeconds();
	Phase = EPlatformerPhase::Playing;
}

void APlatformerGameMode::SpawnPlayer(AController* Controller)
{
	RestartPlayerAtTransform(Controller, FTransform(PlayerStartLocation));
}

void APlatformerGameMode::RespawnPlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}
		if (APawn* OldPawn = PC->GetPawn())
		{
			OldPawn->Destroy();
		}
		SpawnPlayer(PC);
	}
}

APlatformerCharacter* APlatformerGameMode::GetPlayerCharacter() const
{
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	return PC ? Cast<APlatformerCharacter>(PC->GetPawn()) : nullptr;
}

float APlatformerGameMode::GetLevelElapsedTime() const
{
	return GetWorld()->GetTimeSeconds() - LevelStartTime;
}

void APlatformerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Phase != EPlatformerPhase::Playing)
	{
		return;
	}

	APlatformerCharacter* Player = GetPlayerCharacter();
	if (Player && !Player->IsDead())
	{
		TimeLeft -= DeltaSeconds;
		if (TimeLeft <= 0.f)
		{
			TimeLeft = 0.f;
			Player->Die();
		}
	}
}

void APlatformerGameMode::OnPlayerDied(APlatformerCharacter* Player)
{
	--Lives;
	if (Lives > 0)
	{
		// Se reconstruye el nivel: vuelven los enemigos, diamantes y bloques.
		LoadLevel(CurrentLevel);
		RespawnPlayers();
		return;
	}

	Phase = EPlatformerPhase::GameOver;
	Player->Destroy();
	GetWorldTimerManager().SetTimer(PhaseTimer, this, &APlatformerGameMode::RestartGame, 4.f);
}

void APlatformerGameMode::OnGoalReached(APlatformerCharacter* Player)
{
	if (Phase != EPlatformerPhase::Playing || !Player || Player->IsDead())
	{
		return;
	}

	Phase = EPlatformerPhase::LevelComplete;
	Player->CelebrateGoal();
	// Bonificación por el tiempo sobrante.
	AddScore(GetTimeLeft() * 10);
	GetWorldTimerManager().SetTimer(PhaseTimer, this, &APlatformerGameMode::GoToNextLevel, 3.f);
}

void APlatformerGameMode::GoToNextLevel()
{
	if (ALevelBuilder::LevelExists(CurrentLevel + 1))
	{
		LoadLevel(CurrentLevel + 1);
		RespawnPlayers();
		return;
	}

	Phase = EPlatformerPhase::Victory;
	GetWorldTimerManager().SetTimer(PhaseTimer, this, &APlatformerGameMode::RestartGame, 8.f);
}

void APlatformerGameMode::RestartGame()
{
	Lives = StartingLives;
	Score = 0;
	Diamonds = 0;
	LoadLevel(1);
	RespawnPlayers();
}

void APlatformerGameMode::AddScore(int32 Points)
{
	Score += Points;
}

void APlatformerGameMode::AddDiamond()
{
	++Diamonds;
	AddScore(100);
	if (Diamonds >= DiamondsPerExtraLife)
	{
		Diamonds -= DiamondsPerExtraLife;
		++Lives;
	}
}
