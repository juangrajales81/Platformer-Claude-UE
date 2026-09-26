#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlatformerGameMode.generated.h"

/** Reglas de la partida: vidas, puntuación, muerte, reaparición y cambio de nivel. */
UCLASS()
class PLATFORMER_API APlatformerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APlatformerGameMode();
};
