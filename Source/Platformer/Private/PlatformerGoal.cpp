#include "PlatformerGoal.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "PlatformerVisuals.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

APlatformerGoal::APlatformerGoal()
{
	PrimaryActorTick.bCanEverTick = true;

	// El actor está en el centro de la celda 'G'; el mástil sube 3 celdas desde su base.
	const float Base = -PlatformerVisuals::TileSize * 0.5f;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.f, 0.f, Base + 150.f));
	Trigger->SetBoxExtent(FVector(40.f, 100.f, 150.f));
	Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	Trigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	using namespace PlatformerVisuals;
	Pole = CreateMeshPart(this, RootComponent, TEXT("Pole"), Cylinder(), FVector(0.f, 0.f, Base + 150.f), FVector(0.08f, 0.08f, 3.f));
	Flag = CreateMeshPart(this, RootComponent, TEXT("Flag"), Cube(),     FVector(40.f, 0.f, Base + 260.f), FVector(0.75f, 0.05f, 0.5f));
	Star = CreateMeshPart(this, RootComponent, TEXT("Star"), Sphere(),   FVector(0.f, 0.f, Base + 310.f), FVector(0.25f));
}

void APlatformerGoal::BeginPlay()
{
	Super::BeginPlay();

	PlatformerVisuals::Paint(Pole, FLinearColor(0.9f, 0.9f, 0.9f));
	PlatformerVisuals::Paint(Flag, FLinearColor(0.1f, 0.8f, 0.3f));
	PlatformerVisuals::Paint(Star, FLinearColor(1.f, 0.85f, 0.1f));
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &APlatformerGoal::HandleOverlap);
}

void APlatformerGoal::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// La bandera ondea un poco.
	const float Time = GetWorld()->GetTimeSeconds();
	Flag->SetRelativeRotation(FRotator(0.f, FMath::Sin(Time * 4.f) * 12.f, 0.f));
}

void APlatformerGoal::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlatformerCharacter* Player = Cast<APlatformerCharacter>(OtherActor))
	{
		if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
		{
			GameMode->OnGoalReached(Player);
		}
	}
}
