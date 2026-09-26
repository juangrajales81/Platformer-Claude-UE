#include "PlatformerHUD.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"

void APlatformerHUD::DrawHUD()
{
	Super::DrawHUD();

	const APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	if (!GameMode || !Canvas)
	{
		return;
	}

	// Todo se escala respecto a una resolución de referencia de 720 píxeles de alto.
	const float UIScale = Canvas->ClipY / 720.f;
	const float TextScale = 1.2f * UIScale;
	const float Top = 16.f * UIScale;
	const FLinearColor Yellow(1.f, 0.85f, 0.2f);
	const FLinearColor Cyan(0.3f, 0.9f, 1.f);

	DrawShadowedText(FString::Printf(TEXT("PUNTOS %07d"), GameMode->GetScore()), 24.f * UIScale, Top, TextScale, FLinearColor::White);
	DrawShadowedText(FString::Printf(TEXT("DIAMANTES %02d"), GameMode->GetDiamonds()), Canvas->ClipX * 0.3f, Top, TextScale, Cyan);
	DrawShadowedText(FString::Printf(TEXT("VIDAS %d"), GameMode->GetLives()), Canvas->ClipX * 0.52f, Top, TextScale, FLinearColor::White);
	DrawShadowedText(FString::Printf(TEXT("NIVEL %d"), GameMode->GetCurrentLevel()), Canvas->ClipX * 0.67f, Top, TextScale, FLinearColor::White);

	const int32 TimeLeft = GameMode->GetTimeLeft();
	const FLinearColor TimeColor = TimeLeft <= 30 ? FLinearColor(1.f, 0.25f, 0.2f) : Yellow;
	DrawShadowedText(FString::Printf(TEXT("TIEMPO %03d"), TimeLeft), Canvas->ClipX * 0.83f, Top, TextScale, TimeColor);

	if (const APlatformerCharacter* Player = GameMode->GetPlayerCharacter())
	{
		static const TCHAR* PowerNames[] = { TEXT(""), TEXT("PODER: PUNK"), TEXT("PODER: FUEGO") };
		const int32 Power = FMath::Clamp(Player->GetPowerLevel(), 0, 2);
		if (Power > 0)
		{
			DrawShadowedText(PowerNames[Power], 24.f * UIScale, Top + 34.f * UIScale, TextScale * 0.8f, FLinearColor(1.f, 0.4f, 0.8f));
		}
	}

	switch (GameMode->GetPhase())
	{
	case EPlatformerPhase::Title:
		DrawCenterBanner(TEXT("DREAM PLATFORMER"), TEXT("Pulsa ESPACIO o A para empezar"), Yellow, true);
		DrawShadowedText(TEXT("Moverse: A/D o flechas   Saltar: Espacio   Disparar: F"),
			Canvas->ClipX * 0.5f, Canvas->ClipY * 0.75f, 0.9f * UIScale, FLinearColor(0.8f, 0.8f, 0.9f), true);
		break;
	case EPlatformerPhase::Playing:
		// Rótulo de presentación durante los primeros segundos del nivel.
		if (GameMode->GetLevelElapsedTime() < 2.5f)
		{
			DrawCenterBanner(FString::Printf(TEXT("NIVEL %d"), GameMode->GetCurrentLevel()), GameMode->GetLevelTitle(), Yellow, false);
		}
		break;
	case EPlatformerPhase::LevelComplete:
		DrawCenterBanner(TEXT("¡NIVEL COMPLETADO!"), TEXT("Bonificación por tiempo"), Yellow, false);
		break;
	case EPlatformerPhase::GameOver:
		DrawCenterBanner(TEXT("GAME OVER"), FString::Printf(TEXT("Puntuación final: %d"), GameMode->GetScore()), FLinearColor(1.f, 0.3f, 0.2f), true);
		break;
	case EPlatformerPhase::Victory:
		DrawCenterBanner(TEXT("¡HAS DESPERTADO DEL SUEÑO!"), FString::Printf(TEXT("Puntuación final: %d"), GameMode->GetScore()), Yellow, true);
		break;
	}
}

void APlatformerHUD::DrawShadowedText(const FString& Text, float X, float Y, float Scale, const FLinearColor& Color, bool bCentered)
{
	UFont* Font = GEngine->GetLargeFont();
	if (bCentered)
	{
		float Width = 0.f;
		float Height = 0.f;
		GetTextSize(Text, Width, Height, Font, Scale);
		X -= Width * 0.5f;
	}

	const float ShadowOffset = 2.f * Scale;
	DrawText(Text, FLinearColor(0.f, 0.f, 0.f, 0.8f), X + ShadowOffset, Y + ShadowOffset, Font, Scale);
	DrawText(Text, Color, X, Y, Font, Scale);
}

void APlatformerHUD::DrawCenterBanner(const FString& Title, const FString& Subtitle, const FLinearColor& Color, bool bDarkenScreen)
{
	const float UIScale = Canvas->ClipY / 720.f;
	const float CenterX = Canvas->ClipX * 0.5f;
	const float CenterY = Canvas->ClipY * 0.4f;

	if (bDarkenScreen)
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.05f, 0.85f), 0.f, 0.f, Canvas->ClipX, Canvas->ClipY);
	}
	else
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.4f), 0.f, CenterY - 20.f * UIScale, Canvas->ClipX, 130.f * UIScale);
	}

	DrawShadowedText(Title, CenterX, CenterY, 2.5f * UIScale, Color, true);
	if (!Subtitle.IsEmpty())
	{
		DrawShadowedText(Subtitle, CenterX, CenterY + 60.f * UIScale, 1.2f * UIScale, FLinearColor::White, true);
	}
}
