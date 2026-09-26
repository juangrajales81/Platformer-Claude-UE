#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerBlocks.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class APlatformerCharacter;

/** Bloque sólido que reacciona cuando el jugador lo golpea desde abajo con la cabeza. */
UCLASS(Abstract)
class PLATFORMER_API APlatformerBlock : public AActor
{
	GENERATED_BODY()

public:
	APlatformerBlock();

	virtual void Tick(float DeltaSeconds) override;

	/** Lo llama el personaje al chocar con la parte inferior del bloque. */
	virtual void HitFromBelow(APlatformerCharacter* Player);

protected:
	virtual void BeginPlay() override;

	/** Pequeño salto visual del bloque (la colisión no se mueve). */
	void Bump();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Collision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Visual;

	FLinearColor BaseColor = FLinearColor::White;

private:
	float BumpTime = -1.f;
};

/** Ladrillo: se rompe si la jugadora tiene el poder "punk"; si no, solo rebota. */
UCLASS()
class PLATFORMER_API ABrickBlock : public APlatformerBlock
{
	GENERATED_BODY()

public:
	ABrickBlock();

	virtual void HitFromBelow(APlatformerCharacter* Player) override;

	void Break();
};

UENUM()
enum class EBonusContent : uint8
{
	Diamond,
	PowerUp,
};

/** Bloque bonus: suelta su contenido una sola vez y queda apagado. */
UCLASS()
class PLATFORMER_API ABonusBlock : public APlatformerBlock
{
	GENERATED_BODY()

public:
	ABonusBlock();

	virtual void HitFromBelow(APlatformerCharacter* Player) override;

	UPROPERTY(EditAnywhere, Category = "Bonus")
	EBonusContent Content = EBonusContent::Diamond;

protected:
	virtual void BeginPlay() override;

	/** Rombo decorativo en la cara que mira a la cámara. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Emblem;

	bool bUsed = false;
};

/** Trozo de ladrillo que sale despedido al romperse y desaparece. */
UCLASS()
class PLATFORMER_API ABlockDebris : public AActor
{
	GENERATED_BODY()

public:
	ABlockDebris();

	void Launch(const FVector& Velocity, const FLinearColor& Color);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> Movement;
};
