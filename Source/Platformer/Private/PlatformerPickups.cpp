#include "PlatformerPickups.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "PlatformerVisuals.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

// ---------------------------------------------------------------------------
// APickup

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(35.f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);
	RootComponent = Collision;

	Mesh = PlatformerVisuals::CreateMeshPart(this, Collision, TEXT("Mesh"), PlatformerVisuals::Sphere(), FVector::ZeroVector, FVector(0.4f));
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	PlatformerVisuals::Paint(Mesh, Color);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &APickup::HandleOverlap);
	// Desfase para que no todos floten al mismo ritmo.
	Age = GetActorLocation().X * 0.01f;
}

void APickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Age += DeltaSeconds;
	Mesh->AddRelativeRotation(FRotator(0.f, SpinSpeed * DeltaSeconds, 0.f));
	Mesh->SetRelativeLocation(FVector(0.f, 0.f, FMath::Sin(Age * 3.f) * BobHeight));
}

void APickup::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlatformerCharacter* Player = Cast<APlatformerCharacter>(OtherActor);
	if (Player && !Player->IsDead() && OnCollected(Player))
	{
		Destroy();
	}
}

// ---------------------------------------------------------------------------
// ADiamond

ADiamond::ADiamond()
{
	Color = FLinearColor(0.1f, 0.8f, 1.f);

	// Un cubo girado sobre su diagonal parece una gema.
	Mesh->SetStaticMesh(PlatformerVisuals::Cube());
	Mesh->SetRelativeScale3D(FVector(0.3f));
	Mesh->SetRelativeRotation(FRotator(45.f, 0.f, 35.f));
}

bool ADiamond::OnCollected(APlatformerCharacter* Player)
{
	GiveReward();
	return true;
}

void ADiamond::GiveReward()
{
	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->AddDiamond();
	}
}

void ADiamond::CollectAsPopup(APlatformerCharacter* Player)
{
	bPopup = true;
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GiveReward();
	SetLifeSpan(0.45f);
}

void ADiamond::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bPopup)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, 400.f * DeltaSeconds));
		Mesh->AddRelativeRotation(FRotator(0.f, 720.f * DeltaSeconds, 0.f));
	}
}

// ---------------------------------------------------------------------------
// APowerUp

APowerUp::APowerUp()
{
	Color = FLinearColor(1.f, 0.2f, 0.6f);
	SpinSpeed = 120.f;

	Mesh->SetRelativeScale3D(FVector(0.45f));
	Ring = PlatformerVisuals::CreateMeshPart(this, Mesh, TEXT("Ring"), PlatformerVisuals::Cylinder(),
		FVector::ZeroVector, FVector(1.35f, 1.35f, 0.12f), FRotator(0.f, 0.f, 70.f));
}

void APowerUp::BeginPlay()
{
	Super::BeginPlay();
	PlatformerVisuals::Paint(Ring, FLinearColor(1.f, 0.9f, 0.2f));
}

bool APowerUp::OnCollected(APlatformerCharacter* Player)
{
	Player->GainPower();
	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->AddScore(500);
	}
	return true;
}
