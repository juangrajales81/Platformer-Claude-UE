#include "PlatformerEnemy.h"
#include "PlatformerCharacter.h"
#include "PlatformerGameMode.h"
#include "PlatformerVisuals.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"

namespace
{
	constexpr float EnemyRadius = 34.f;
	constexpr float EnemyHalfHeight = 36.f;
}

// ---------------------------------------------------------------------------
// AEnemyWalker

AEnemyWalker::AEnemyWalker()
{
	PrimaryActorTick.bCanEverTick = true;

	// Sin controlador de IA: el propio actor decide su movimiento.
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->InitCapsuleSize(EnemyRadius, EnemyHalfHeight);
	// Los personajes se atraviesan entre sí; el contacto se resuelve en CheckPlayerContact.
	Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bRunPhysicsWithNoController = true;
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 720.f, 0.f);
	Movement->MaxWalkSpeed = WalkSpeed;
	Movement->GravityScale = 2.f;
	Movement->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	Movement->SetPlaneConstraintEnabled(true);
	Movement->bSnapToPlaneAtStart = true;

	using namespace PlatformerVisuals;
	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(Capsule);

	BodyMesh     = CreateMeshPart(this, VisualRoot, TEXT("Body"),     Sphere(), FVector::ZeroVector,         FVector(0.7f, 0.7f, 0.75f));
	BellyMesh    = CreateMeshPart(this, VisualRoot, TEXT("Belly"),    Sphere(), FVector(14.f, 0.f, -8.f),    FVector(0.5f, 0.55f, 0.5f));
	LeftEyeMesh  = CreateMeshPart(this, VisualRoot, TEXT("LeftEye"),  Sphere(), FVector(26.f, -12.f, 14.f), FVector(0.2f));
	RightEyeMesh = CreateMeshPart(this, VisualRoot, TEXT("RightEye"), Sphere(), FVector(26.f, 12.f, 14.f),  FVector(0.2f));
	BeakMesh     = CreateMeshPart(this, VisualRoot, TEXT("Beak"),     Cone(),   FVector(34.f, 0.f, 2.f),    FVector(0.12f, 0.12f, 0.15f), FRotator(-90.f, 0.f, 0.f));
}

void AEnemyWalker::BeginPlay()
{
	Super::BeginPlay();

	using namespace PlatformerVisuals;
	Paint(BodyMesh,     FLinearColor(0.35f, 0.18f, 0.08f));
	Paint(BellyMesh,    FLinearColor(0.8f, 0.65f, 0.4f));
	Paint(LeftEyeMesh,  FLinearColor(1.f, 0.95f, 0.3f));
	Paint(RightEyeMesh, FLinearColor(1.f, 0.95f, 0.3f));
	Paint(BeakMesh,     FLinearColor(1.f, 0.45f, 0.f));

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AEnemyWalker::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bDying)
	{
		return;
	}

	TurnCooldown -= DeltaSeconds;
	if (TurnCooldown <= 0.f && GetCharacterMovement()->IsMovingOnGround() && ShouldTurnAround())
	{
		Direction = -Direction;
		TurnCooldown = 0.25f;
	}
	AddMovementInput(FVector(Direction, 0.f, 0.f));

	CheckPlayerContact();
}

bool AEnemyWalker::ShouldTurnAround() const
{
	const UWorld* World = GetWorld();
	const FVector Location = GetActorLocation();
	const FVector Ahead = FVector(Direction * (EnemyRadius + 12.f), 0.f, 0.f);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemyProbe), false, this);
	const FCollisionObjectQueryParams Solid(FCollisionObjectQueryParams::InitType::AllStaticObjects);
	FHitResult Hit;

	// Pared delante.
	if (World->LineTraceSingleByObjectType(Hit, Location, Location + Ahead, Solid, Params))
	{
		return true;
	}

	// Hueco delante: no hay suelo justo por debajo de los pies.
	const FVector ProbeStart = Location + Ahead;
	const FVector ProbeEnd = ProbeStart - FVector(0.f, 0.f, EnemyHalfHeight + 40.f);
	return !World->LineTraceSingleByObjectType(Hit, ProbeStart, ProbeEnd, Solid, Params);
}

void AEnemyWalker::CheckPlayerContact()
{
	TArray<AActor*> Overlapping;
	GetCapsuleComponent()->GetOverlappingActors(Overlapping, APlatformerCharacter::StaticClass());

	for (AActor* Actor : Overlapping)
	{
		APlatformerCharacter* Player = Cast<APlatformerCharacter>(Actor);
		if (!Player || Player->IsDead())
		{
			continue;
		}

		// Pisotón: la jugadora cae y sus pies están por encima del centro del enemigo.
		const float PlayerFeet = Player->GetActorLocation().Z - Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		if (Player->GetVelocity().Z <= 0.f && PlayerFeet > GetActorLocation().Z)
		{
			Kill(true);
			Player->BounceOffEnemy();
			return;
		}

		Player->ReceiveDamage();
	}
}

void AEnemyWalker::Kill(bool bSquashed)
{
	if (bDying)
	{
		return;
	}
	bDying = true;

	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->AddScore(ScoreValue);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UCharacterMovementComponent* Movement = GetCharacterMovement();

	if (bSquashed)
	{
		Movement->DisableMovement();
		VisualRoot->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.25f));
		VisualRoot->SetRelativeLocation(FVector(0.f, 0.f, -EnemyHalfHeight * 0.8f));
		SetLifeSpan(0.6f);
	}
	else
	{
		// Boca arriba y fuera de la pantalla atravesando el suelo.
		VisualRoot->SetRelativeRotation(FRotator(0.f, 0.f, 180.f));
		Movement->SetMovementMode(MOVE_Falling);
		Movement->Velocity = FVector(Direction * 150.f, 0.f, 700.f);
		SetLifeSpan(2.f);
	}
}

// ---------------------------------------------------------------------------
// ASpikes

ASpikes::ASpikes()
{
	PrimaryActorTick.bCanEverTick = true;

	// El actor está en el centro de la celda; los pinchos ocupan su mitad inferior.
	const float Half = PlatformerVisuals::TileSize * 0.5f;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DamageArea = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageArea"));
	DamageArea->SetupAttachment(RootComponent);
	DamageArea->SetRelativeLocation(FVector(0.f, 0.f, -Half + 20.f));
	DamageArea->SetBoxExtent(FVector(Half - 8.f, Half, 20.f));
	DamageArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageArea->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Conos de 50 de alto con la base apoyada en el suelo de la celda.
	for (int32 i = 0; i < 3; ++i)
	{
		const FName Name = *FString::Printf(TEXT("Spike%d"), i);
		Spikes.Add(PlatformerVisuals::CreateMeshPart(this, RootComponent, Name, PlatformerVisuals::Cone(),
			FVector((i - 1) * 30.f, 0.f, -Half + 25.f), FVector(0.28f, 0.28f, 0.5f)));
	}
}

void ASpikes::BeginPlay()
{
	Super::BeginPlay();

	for (UStaticMeshComponent* Spike : Spikes)
	{
		PlatformerVisuals::Paint(Spike, FLinearColor(0.75f, 0.75f, 0.8f));
	}
}

void ASpikes::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<AActor*> Overlapping;
	DamageArea->GetOverlappingActors(Overlapping, APlatformerCharacter::StaticClass());
	for (AActor* Actor : Overlapping)
	{
		if (APlatformerCharacter* Player = Cast<APlatformerCharacter>(Actor))
		{
			Player->ReceiveDamage();
		}
	}
}
