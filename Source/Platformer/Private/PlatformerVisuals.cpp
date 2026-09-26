#include "PlatformerVisuals.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace PlatformerVisuals
{
	UStaticMesh* Cube()     { return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")); }
	UStaticMesh* Sphere()   { return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere")); }
	UStaticMesh* Cylinder() { return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder")); }
	UStaticMesh* Cone()     { return LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone")); }

	UMaterialInterface* BaseMaterial()
	{
		return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	UStaticMeshComponent* CreateMeshPart(AActor* Owner, USceneComponent* Parent, FName Name,
		UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FRotator& Rotation)
	{
		UStaticMeshComponent* Part = Owner->CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Part->SetupAttachment(Parent);
		Part->SetStaticMesh(Mesh);
		Part->SetRelativeLocation(Location);
		Part->SetRelativeRotation(Rotation);
		Part->SetRelativeScale3D(Scale);
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetGenerateOverlapEvents(false);
		Part->SetCanEverAffectNavigation(false);
		return Part;
	}

	void Paint(UPrimitiveComponent* Component, const FLinearColor& Color)
	{
		if (!Component)
		{
			return;
		}
		// Se reutiliza el material dinámico si el componente ya tiene uno.
		UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(Component->GetMaterial(0));
		if (!MID)
		{
			MID = Component->CreateDynamicMaterialInstance(0, BaseMaterial());
		}
		if (MID)
		{
			MID->SetVectorParameterValue(TEXT("Color"), Color);
		}
	}
}
