#include "PlatformerEnvironment.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"

APlatformerEnvironment::APlatformerEnvironment()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// El sol ilumina desde el lado de la cámara (+Y) y desde arriba.
	Sun = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("Sun"));
	Sun->SetupAttachment(RootComponent);
	Sun->SetMobility(EComponentMobility::Movable);
	Sun->SetRelativeRotation(FRotator(-45.f, -120.f, 0.f));
	Sun->SetIntensity(8.f);
	Sun->SetAtmosphereSunLight(true);

	SkyAtmosphere = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphere->SetupAttachment(RootComponent);

	SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLight->SetupAttachment(RootComponent);
	SkyLight->SetMobility(EComponentMobility::Movable);
	SkyLight->bRealTimeCapture = true;
	SkyLight->SetIntensity(1.5f);
}
