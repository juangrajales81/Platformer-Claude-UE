#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformerBackdrop.generated.h"

class UStaticMeshComponent;

/** Una capa del fondo: se desplaza con la cámara a una fracción de su velocidad. */
USTRUCT()
struct FBackdropLayer
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USceneComponent> Root;

	/** 0 = fija en el mundo, 1 = se mueve con la cámara (parece infinitamente lejana). */
	UPROPERTY()
	float Follow = 0.f;
};

/** Fondo con parallax: colinas lejanas y nubes detrás del nivel (Y negativa). */
UCLASS()
class PLATFORMER_API APlatformerBackdrop : public AActor
{
	GENERATED_BODY()

public:
	APlatformerBackdrop();

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	void AddLayer(FName Name, UStaticMesh* Mesh, int32 Count, float Depth, float BaseZ, float Spacing,
		const FVector& Scale, float Follow, TArray<TObjectPtr<UStaticMeshComponent>>& OutParts);

	UPROPERTY(VisibleAnywhere)
	TArray<FBackdropLayer> Layers;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UStaticMeshComponent>> Hills;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UStaticMeshComponent>> Clouds;
};
