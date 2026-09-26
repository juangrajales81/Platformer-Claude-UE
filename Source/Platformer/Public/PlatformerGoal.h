#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerGoal.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/** Mástil con bandera al final del nivel. Tocarlo completa el nivel. */
UCLASS()
class PLATFORMER_API APlatformerGoal : public AActor
{
	GENERATED_BODY()

public:
	APlatformerGoal();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Pole;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Flag;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Star;
};
