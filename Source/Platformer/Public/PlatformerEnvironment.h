#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerEnvironment.generated.h"

class UDirectionalLightComponent;
class USkyLightComponent;
class USkyAtmosphereComponent;

/** Iluminación dinámica y cielo del nivel (el mapa base está vacío). */
UCLASS()
class PLATFORMER_API APlatformerEnvironment : public AActor
{
	GENERATED_BODY()

public:
	APlatformerEnvironment();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDirectionalLightComponent> Sun;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkyLightComponent> SkyLight;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkyAtmosphereComponent> SkyAtmosphere;
};
