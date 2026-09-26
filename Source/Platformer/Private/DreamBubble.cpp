#include "DreamBubble.h"
#include "PlatformerEnemy.h"
#include "PlatformerVisuals.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

namespace
{
	constexpr float BubbleSpeed = 850.f;
	constexpr float BounceSpeed = 450.f;
}

ADreamBubble::ADreamBubble()
{
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	Collision->InitSphereRadius(16.f);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Collision->SetGenerateOverlapEvents(true);
	RootComponent = Collision;

	Mesh = PlatformerVisuals::CreateMeshPart(this, Collision, TEXT("Mesh"), PlatformerVisuals::Sphere(), FVector::ZeroVector, FVector(0.32f));

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->bAutoActivate = false;
	Movement->bShouldBounce = true;
	Movement->Bounciness = 1.f;
	Movement->Friction = 0.f;
	Movement->ProjectileGravityScale = 2.f;
	Movement->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	Movement->SetPlaneConstraintEnabled(true);

	InitialLifeSpan = 2.5f;
}

void ADreamBubble::BeginPlay()
{
	Super::BeginPlay();

	PlatformerVisuals::Paint(Mesh, FLinearColor(0.6f, 0.4f, 1.f));
	Movement->OnProjectileBounce.AddDynamic(this, &ADreamBubble::HandleBounce);
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ADreamBubble::HandleOverlap);
}

void ADreamBubble::Fire(float Direction)
{
	Movement->Velocity = FVector(Direction * BubbleSpeed, 0.f, -150.f);
	Movement->Activate();
}

void ADreamBubble::HandleBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (ImpactResult.ImpactNormal.Z > 0.6f)
	{
		// Bote de altura constante sobre el suelo, manteniendo la velocidad horizontal.
		Movement->Velocity = FVector(FMath::Sign(ImpactVelocity.X) * BubbleSpeed, 0.f, BounceSpeed);
	}
	else if (FMath::Abs(ImpactResult.ImpactNormal.X) > 0.6f)
	{
		// Contra una pared la burbuja revienta.
		Destroy();
	}
}

void ADreamBubble::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyWalker* Enemy = Cast<AEnemyWalker>(OtherActor);
	if (Enemy && Enemy->IsAlive())
	{
		Enemy->Kill(false);
		Destroy();
	}
}
