#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerPickups.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class APlatformerCharacter;

/** Objeto que gira, flota y se recoge al tocarlo. */
UCLASS(Abstract)
class PLATFORMER_API APickup : public AActor
{
	GENERATED_BODY()

public:
	APickup();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	/** Efecto de recoger el objeto. Devuelve true si debe desaparecer. */
	virtual bool OnCollected(APlatformerCharacter* Player) { return true; }

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	FLinearColor Color = FLinearColor::White;
	float SpinSpeed = 180.f;
	float BobHeight = 6.f;

private:
	float Age = 0.f;
};

/** Diamante: suma uno al contador (100 = vida extra) y da puntos. */
UCLASS()
class PLATFORMER_API ADiamond : public APickup
{
	GENERATED_BODY()

public:
	ADiamond();

	virtual void Tick(float DeltaSeconds) override;

	/** Diamante que sale de un bloque bonus: se cuenta al momento y sube antes de desaparecer. */
	void CollectAsPopup(APlatformerCharacter* Player);

protected:
	virtual bool OnCollected(APlatformerCharacter* Player) override;

	void GiveReward();

private:
	bool bPopup = false;
};

/**
 * Bola de sueño: cada una sube un nivel de poder.
 * Nivel 1 "punk" rompe ladrillos; nivel 2 "fuego" además dispara burbujas.
 */
UCLASS()
class PLATFORMER_API APowerUp : public APickup
{
	GENERATED_BODY()

public:
	APowerUp();

protected:
	virtual void BeginPlay() override;
	virtual bool OnCollected(APlatformerCharacter* Player) override;

	/** Anillo que rodea la bola. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Ring;
};
