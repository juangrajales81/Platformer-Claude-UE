#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlatformerHUD.generated.h"

/** Marcador dibujado con Canvas: puntos, diamantes, vidas, nivel, tiempo y mensajes centrales. */
UCLASS()
class PLATFORMER_API APlatformerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	/** Texto con sombra. Si bCentered, X es el centro horizontal. */
	void DrawShadowedText(const FString& Text, float X, float Y, float Scale, const FLinearColor& Color, bool bCentered = false);

	void DrawCenterBanner(const FString& Title, const FString& Subtitle, const FLinearColor& Color, bool bDarkenScreen);
};
