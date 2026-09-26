#include "PlatformerBlocks.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "PlatformerPickups.h"
#include "PlatformerVisuals.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/World.h"
#include "GameFramework/ProjectileMovementComponent.h"

namespace
{
	constexpr float BumpDuration = 0.18f;
	constexpr float BumpHeight = 18.f;
}

// ---------------------------------------------------------------------------
// APlatformerBlock

APlatformerBlock::APlatformerBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetBoxExtent(FVector(PlatformerVisuals::TileSize * 0.5f));
	Collision->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RootComponent = Collision;

	Visual = PlatformerVisuals::CreateMeshPart(this, Collision, TEXT("Visual"), PlatformerVisuals::Cube(), FVector::ZeroVector, FVector(1.f));
}

void APlatformerBlock::BeginPlay()
{
	Super::BeginPlay();
	PlatformerVisuals::Paint(Visual, BaseColor);
}

void APlatformerBlock::HitFromBelow(APlatformerCharacter* Player)
{
	Bump();
}

void APlatformerBlock::Bump()
{
	BumpTime = 0.f;
	SetActorTickEnabled(true);
}

void APlatformerBlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (BumpTime < 0.f)
	{
		SetActorTickEnabled(false);
		return;
	}

	BumpTime += DeltaSeconds;
	const float Alpha = FMath::Clamp(BumpTime / BumpDuration, 0.f, 1.f);
	Visual->SetRelativeLocation(FVector(0.f, 0.f, FMath::Sin(Alpha * PI) * BumpHeight));
	if (Alpha >= 1.f)
	{
		BumpTime = -1.f;
	}
}

// ---------------------------------------------------------------------------
// ABrickBlock

ABrickBlock::ABrickBlock()
{
	BaseColor = FLinearColor(0.7f, 0.22f, 0.08f);
}

void ABrickBlock::HitFromBelow(APlatformerCharacter* Player)
{
	if (Player && Player->CanBreakBricks())
	{
		Break();
	}
	else
	{
		Super::HitFromBelow(Player);
	}
}

void ABrickBlock::Break()
{
	UWorld* World = GetWorld();
	const FVector Center = GetActorLocation();
	const float Offset = PlatformerVisuals::TileSize * 0.25f;

	for (int32 i = 0; i < 4; ++i)
	{
		const float SideX = (i % 2 == 0) ? -1.f : 1.f;
		const float Top = (i < 2) ? 1.f : 0.f;
		const FVector PieceLocation = Center + FVector(SideX * Offset, 0.f, (Top - 0.5f) * 2.f * Offset);
		if (ABlockDebris* Debris = World->SpawnActor<ABlockDebris>(PieceLocation, FRotator::ZeroRotator))
		{
			Debris->Launch(FVector(SideX * 250.f, 0.f, 500.f + Top * 250.f), BaseColor);
		}
	}

	if (APlatformerGameMode* GameMode = World->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->AddScore(50);
	}
	Destroy();
}

// ---------------------------------------------------------------------------
// ABonusBlock

ABonusBlock::ABonusBlock()
{
	BaseColor = FLinearColor(1.f, 0.7f, 0.05f);

	Emblem = PlatformerVisuals::CreateMeshPart(this, Visual, TEXT("Emblem"), PlatformerVisuals::Cube(),
		FVector(0.f, 50.f, 0.f), FVector(0.3f, 0.05f, 0.3f), FRotator(45.f, 0.f, 0.f));
}

void ABonusBlock::BeginPlay()
{
	Super::BeginPlay();
	PlatformerVisuals::Paint(Emblem, FLinearColor(1.f, 1.f, 1.f));
}

void ABonusBlock::HitFromBelow(APlatformerCharacter* Player)
{
	Super::HitFromBelow(Player);
	if (bUsed)
	{
		return;
	}
	bUsed = true;

	UWorld* World = GetWorld();
	const FVector Above = GetActorLocation() + FVector(0.f, 0.f, PlatformerVisuals::TileSize);

	switch (Content)
	{
	case EBonusContent::Diamond:
		if (ADiamond* Diamond = World->SpawnActor<ADiamond>(Above, FRotator::ZeroRotator))
		{
			Diamond->CollectAsPopup(Player);
		}
		break;
	case EBonusContent::PowerUp:
		break;
	}

	// Bloque gastado: color apagado y sin emblema.
	PlatformerVisuals::Paint(Visual, FLinearColor(0.3f, 0.2f, 0.12f));
	Emblem->SetVisibility(false);
}

// ---------------------------------------------------------------------------
// ABlockDebris

ABlockDebris::ABlockDebris()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetStaticMesh(PlatformerVisuals::Cube());
	Mesh->SetRelativeScale3D(FVector(0.35f));
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Mesh;

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->ProjectileGravityScale = 2.f;
	Movement->bAutoActivate = false;

	InitialLifeSpan = 1.2f;
}

void ABlockDebris::Launch(const FVector& Velocity, const FLinearColor& Color)
{
	PlatformerVisuals::Paint(Mesh, Color);
	Movement->Velocity = Velocity;
	Movement->Activate();
	SetActorRotation(FRotator(FMath::FRandRange(0.f, 90.f), 0.f, FMath::FRandRange(0.f, 90.f)));
}
