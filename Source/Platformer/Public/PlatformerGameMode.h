#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LevelBuilder.h"
#include "PlatformerGameMode.generated.h"

/** Reglas de la partida: vidas, puntuación, muerte, reaparición y cambio de nivel. */
UCLASS()
class PLATFORMER_API APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerGameMode();

	virtual void StartPlay() override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	/** Lo llama la jugadora al terminar su animación de muerte. */
	virtual void OnPlayerDied(class APlatformerCharacter* Player);

	void AddScore(int32 Points);
	void AddDiamond();

	int32 GetScore() const { return Score; }
	int32 GetDiamonds() const { return Diamonds; }

protected:
	/** Construye el escenario. Los jugadores no aparecen hasta que termina. */
	virtual void BuildWorld();

	void SpawnPlayer(AController* Controller);

	/** Punto de aparición del jugador en el nivel actual. */
	FVector PlayerStartLocation = FVector(0.f, 0.f, 200.f);

	UPROPERTY(Transient)
	TObjectPtr<ALevelBuilder> LevelBuilder;

	FBuiltLevelInfo CurrentLevelInfo;
	int32 CurrentLevel = 1;

	int32 Score = 0;
	int32 Diamonds = 0;

private:
	bool bWorldReady = false;
};
