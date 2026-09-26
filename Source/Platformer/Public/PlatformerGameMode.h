#pragma once

#include "CoreMinimal.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/GameModeBase.h"
#include "LevelBuilder.h"
#include "PlatformerGameMode.generated.h"

class APlatformerCharacter;

UENUM()
enum class EPlatformerPhase : uint8
{
	Title,
	Playing,
	LevelComplete,
	GameOver,
	Victory,
};

/** Reglas de la partida: vidas, puntuación, tiempo, muerte, reaparición y cambio de nivel. */
UCLASS()
class PLATFORMER_API APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerGameMode();

	virtual void StartPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;

	/** Lo llama la jugadora al terminar su animación de muerte. */
	virtual void OnPlayerDied(APlatformerCharacter* Player);

	/** Desde la pantalla de título: empieza a contar el tiempo y a jugar. */
	void StartGame();

	/** Lo llama la meta cuando la jugadora la alcanza. */
	void OnGoalReached(APlatformerCharacter* Player);

	void AddScore(int32 Points);
	void AddDiamond();

	int32 GetScore() const { return Score; }
	int32 GetDiamonds() const { return Diamonds; }
	int32 GetLives() const { return Lives; }
	int32 GetTimeLeft() const { return FMath::CeilToInt(TimeLeft); }
	int32 GetCurrentLevel() const { return CurrentLevel; }
	const FString& GetLevelTitle() const { return CurrentLevelInfo.Title; }
	EPlatformerPhase GetPhase() const { return Phase; }

	/** Segundos que lleva cargado el nivel actual (para el rótulo de presentación). */
	float GetLevelElapsedTime() const;

	APlatformerCharacter* GetPlayerCharacter() const;

protected:
	/** Crea el entorno y el constructor, y carga el primer nivel. Los jugadores esperan a que termine. */
	virtual void BuildWorld();

	/** (Re)construye un nivel y reinicia el tiempo. */
	void LoadLevel(int32 LevelNumber);

	/** Elimina el personaje actual (si lo hay) y crea uno nuevo en el inicio del nivel. */
	void RespawnPlayers();
	void SpawnPlayer(AController* Controller);

	void GoToNextLevel();

	/** Reinicia vidas y puntos y vuelve a la pantalla de título con el nivel 1. */
	void ReturnToTitle();

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	int32 StartingLives = 3;

	/** Segundos disponibles para completar cada nivel. */
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	float LevelTimeLimit = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	int32 DiamondsPerExtraLife = 100;

	UPROPERTY(Transient)
	TObjectPtr<ALevelBuilder> LevelBuilder;

	FBuiltLevelInfo CurrentLevelInfo;
	FVector PlayerStartLocation = FVector(0.f, 0.f, 200.f);

	EPlatformerPhase Phase = EPlatformerPhase::Title;
	int32 CurrentLevel = 1;
	int32 Lives = 3;
	int32 Score = 0;
	int32 Diamonds = 0;
	float TimeLeft = 0.f;
	float LevelStartTime = 0.f;

	FTimerHandle PhaseTimer;

private:
	bool bWorldReady = false;
};
