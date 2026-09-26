#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DreamBubble.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

/** Burbuja que dispara la jugadora con el poder de fuego: bota por el suelo y elimina enemigos. */
UCLASS()
class PLATFORMER_API ADreamBubble : public AActor
{
	GENERATED_BODY()

public:
	ADreamBubble();

	/** Lanza la burbuja hacia la derecha (+1) o la izquierda (-1). */
	void Fire(float Direction);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> Movement;
};
