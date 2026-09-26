#pragma once

#include "CoreMinimal.h"

class UStaticMesh;
class UStaticMeshComponent;
class UPrimitiveComponent;
class UMaterialInterface;
class USceneComponent;
class AActor;

/**
 * Utilidades para construir la parte visual del juego con las formas básicas del motor
 * (/Engine/BasicShapes) y color por material dinámico, sin assets propios.
 */
namespace PlatformerVisuals
{
	/** Tamaño de una celda del mapa ASCII en unidades de Unreal (1 m). */
	constexpr float TileSize = 100.f;

	PLATFORMER_API UStaticMesh* Cube();
	PLATFORMER_API UStaticMesh* Sphere();
	PLATFORMER_API UStaticMesh* Cylinder();
	PLATFORMER_API UStaticMesh* Cone();
	PLATFORMER_API UMaterialInterface* BaseMaterial();

	/**
	 * Crea (en el constructor de Owner) un componente de malla decorativo, sin colisión.
	 * Scale está en metros, porque las formas básicas miden 100 unidades.
	 */
	PLATFORMER_API UStaticMeshComponent* CreateMeshPart(AActor* Owner, USceneComponent* Parent, FName Name,
		UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);

	/** Asigna un material dinámico con el color indicado (llamar en BeginPlay o después). */
	PLATFORMER_API void Paint(UPrimitiveComponent* Component, const FLinearColor& Color);
}
