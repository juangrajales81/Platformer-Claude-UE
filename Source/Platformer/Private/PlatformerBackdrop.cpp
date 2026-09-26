#include "PlatformerBackdrop.h"
#include "PlatformerVisuals.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

APlatformerBackdrop::APlatformerBackdrop()
{
	PrimaryActorTick.bCanEverTick = true;
	// Se actualiza después de la cámara para no ir un fotograma por detrás.
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	using namespace PlatformerVisuals;
	AddLayer(TEXT("HillLayer"),  Sphere(), 16, -1200.f, -300.f, 900.f,  FVector(12.f, 4.f, 7.f),  0.5f, Hills);
	AddLayer(TEXT("CloudLayer"), Sphere(), 10, -2000.f, 850.f, 1400.f, FVector(6.f, 1.f, 1.8f), 0.8f, Clouds);
}

void APlatformerBackdrop::AddLayer(FName Name, UStaticMesh* Mesh, int32 Count, float Depth, float BaseZ, float Spacing,
	const FVector& Scale, float Follow, TArray<TObjectPtr<UStaticMeshComponent>>& OutParts)
{
	USceneComponent* LayerRoot = CreateDefaultSubobject<USceneComponent>(Name);
	LayerRoot->SetupAttachment(RootComponent);
	LayerRoot->SetRelativeLocation(FVector(0.f, Depth, 0.f));

	// Posiciones pseudoaleatorias pero deterministas para que el fondo sea siempre igual.
	FRandomStream Random(static_cast<int32>(GetTypeHash(Name)));
	for (int32 i = 0; i < Count; ++i)
	{
		const FVector Location((i - Count / 3) * Spacing + Random.FRandRange(-0.3f, 0.3f) * Spacing, 0.f, BaseZ + Random.FRandRange(-150.f, 150.f));
		const FVector PartScale = Scale * Random.FRandRange(0.75f, 1.25f);
		UStaticMeshComponent* Part = PlatformerVisuals::CreateMeshPart(this, LayerRoot,
			*FString::Printf(TEXT("%s_%d"), *Name.ToString(), i), Mesh, Location, PartScale);
		Part->SetCastShadow(false);
		OutParts.Add(Part);
	}

	FBackdropLayer Layer;
	Layer.Root = LayerRoot;
	Layer.Follow = Follow;
	ParallaxLayers.Add(Layer);
}

void APlatformerBackdrop::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < Hills.Num(); ++i)
	{
		PlatformerVisuals::Paint(Hills[i], i % 2 ? FLinearColor(0.12f, 0.35f, 0.2f) : FLinearColor(0.16f, 0.42f, 0.22f));
	}
	for (UStaticMeshComponent* Cloud : Clouds)
	{
		PlatformerVisuals::Paint(Cloud, FLinearColor(0.95f, 0.95f, 1.f));
	}
}

void APlatformerBackdrop::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager)
	{
		return;
	}

	const float CameraX = PC->PlayerCameraManager->GetCameraLocation().X;
	for (const FBackdropLayer& Layer : ParallaxLayers)
	{
		FVector Location = Layer.Root->GetRelativeLocation();
		Location.X = CameraX * Layer.Follow;
		Layer.Root->SetRelativeLocation(Location);
	}
}
