#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "PlatformerEnemy.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * Búho que patrulla: camina en línea recta y se da la vuelta al llegar a una pared
 * o al borde de una plataforma. Muere si le saltan encima; si no, hace daño.
 */
UCLASS()
class PLATFORMER_API AEnemyWalker : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyWalker();

	virtual void Tick(float DeltaSeconds) override;

	/** Elimina al enemigo. Aplastado se queda plano; si no, sale despedido. */
	void Kill(bool bSquashed);

	bool IsAlive() const { return !bDying; }

protected:
	virtual void BeginPlay() override;

	/** Comprueba si delante hay una pared o un hueco. */
	bool ShouldTurnAround() const;

	void CheckPlayerContact();

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<USceneComponent> VisualRoot;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BellyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> LeftEyeMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> RightEyeMesh;

	UPROPERTY(VisibleAnywhere, Category = "Visual")
	TObjectPtr<UStaticMeshComponent> BeakMesh;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	float WalkSpeed = 140.f;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	int32 ScoreValue = 100;

	/** +1 hacia la derecha, -1 hacia la izquierda. Empiezan yendo hacia la jugadora. */
	float Direction = -1.f;

private:
	bool bDying = false;
	float TurnCooldown = 0.f;
};

/** Pinchos: dañan a la jugadora al tocarlos. */
UCLASS()
class PLATFORMER_API ASpikes : public AActor
{
	GENERATED_BODY()

public:
	ASpikes();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> DamageArea;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UStaticMeshComponent>> Spikes;
};
