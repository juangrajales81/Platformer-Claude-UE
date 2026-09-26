#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlatformerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * Protagonista: se mueve solo en el plano XZ (desplazamiento lateral) con una cámara
 * lateral que la sigue. Los controles se crean en C++ con Enhanced Input, sin assets.
 */
UCLASS()
class PLATFORMER_API APlatformerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlatformerCharacter();

	/** Nivel de poder: 0 normal, 1 "punk" (rompe ladrillos), 2 "fuego" (dispara). */
	int32 GetPowerLevel() const { return PowerLevel; }
	bool CanBreakBricks() const { return PowerLevel >= 1; }
	bool IsDead() const { return bDead; }

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
		FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);

	/** Crea las acciones y el mapeo de teclas la primera vez que se necesitan. */
	void CreateInputObjects();

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> DressMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> LegsMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> HairMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> LeftEyeMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> RightEyeMesh;

	UPROPERTY(VisibleInstanceOnly, Category = "State")
	int32 PowerLevel = 0;

	UPROPERTY(VisibleInstanceOnly, Category = "State")
	bool bDead = false;

	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> JumpAction;
};
