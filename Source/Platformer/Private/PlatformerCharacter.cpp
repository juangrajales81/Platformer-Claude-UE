#include "PlatformerCharacter.h"
#include "DreamBubble.h"
#include "PlatformerBlocks.h"
#include "PlatformerGameMode.h"
#include "PlatformerVisuals.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "TimerManager.h"

APlatformerCharacter::APlatformerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(30.f, 44.f);

	// La rotación la decide el movimiento, no el mando.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.f, 2000.f, 0.f);
	Movement->MaxWalkSpeed = 600.f;
	Movement->MaxAcceleration = 3000.f;
	Movement->BrakingDecelerationWalking = 2500.f;
	Movement->GroundFriction = 8.f;
	Movement->GravityScale = 2.2f;
	Movement->JumpZVelocity = 900.f;
	Movement->AirControl = 0.85f;
	Movement->BrakingDecelerationFalling = 600.f;
	Movement->MaxStepHeight = 30.f;
	Movement->SetWalkableFloorAngle(50.f);
	// Juego 2.5D: el personaje nunca sale del plano XZ.
	Movement->SetPlaneConstraintNormal(FVector(0.f, 1.f, 0.f));
	Movement->SetPlaneConstraintEnabled(true);
	Movement->bSnapToPlaneAtStart = true;

	// Mantener pulsado el salto lo hace más alto (salto variable, como en los clásicos).
	JumpMaxHoldTime = 0.22f;
	JumpMaxCount = 1;

	// Cámara lateral: mira hacia -Y, de modo que +X queda a la derecha de la pantalla.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->SetRelativeRotation(FRotator(-6.f, -90.f, 0.f));
	CameraBoom->TargetArmLength = 1600.f;
	CameraBoom->TargetOffset = FVector(0.f, 0.f, 180.f);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 6.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->SetFieldOfView(50.f);

	// Aspecto de la protagonista hecho con formas básicas (los pies están en Z = -44).
	using namespace PlatformerVisuals;
	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(RootComponent);
	VisualRoot->SetRelativeLocation(FVector(0.f, 0.f, -44.f));

	LegsMesh     = CreateMeshPart(this, VisualRoot, TEXT("Legs"),     Cylinder(), FVector(0.f, 0.f, 12.f),   FVector(0.22f, 0.3f, 0.24f));
	DressMesh    = CreateMeshPart(this, VisualRoot, TEXT("Dress"),    Cone(),     FVector(0.f, 0.f, 42.f),   FVector(0.55f, 0.55f, 0.42f));
	HeadMesh     = CreateMeshPart(this, VisualRoot, TEXT("Head"),     Sphere(),   FVector(2.f, 0.f, 74.f),   FVector(0.36f));
	HairMesh     = CreateMeshPart(this, VisualRoot, TEXT("Hair"),     Sphere(),   FVector(-6.f, 0.f, 80.f),  FVector(0.40f, 0.44f, 0.38f));
	LeftEyeMesh  = CreateMeshPart(this, VisualRoot, TEXT("LeftEye"),  Sphere(),   FVector(18.f, -7.f, 76.f), FVector(0.06f));
	RightEyeMesh = CreateMeshPart(this, VisualRoot, TEXT("RightEye"), Sphere(),   FVector(18.f, 7.f, 76.f),  FVector(0.06f));

	for (int32 i = 0; i < 4; ++i)
	{
		const float Angle = -45.f + i * 30.f;
		const FVector Offset = FRotator(Angle, 0.f, 0.f).RotateVector(FVector(0.f, 0.f, 22.f));
		UStaticMeshComponent* Spike = CreateMeshPart(this, VisualRoot, *FString::Printf(TEXT("PunkSpike%d"), i), Cone(),
			FVector(-6.f, 0.f, 84.f) + Offset, FVector(0.12f, 0.12f, 0.25f), FRotator(Angle, 0.f, 0.f));
		Spike->SetVisibility(false);
		PunkSpikes.Add(Spike);
	}
}

void APlatformerCharacter::BeginPlay()
{
	Super::BeginPlay();

	using namespace PlatformerVisuals;
	Paint(LegsMesh,     FLinearColor(0.9f, 0.75f, 0.6f));
	Paint(DressMesh,    FLinearColor(0.8f, 0.05f, 0.1f));
	Paint(HeadMesh,     FLinearColor(1.f, 0.8f, 0.65f));
	Paint(LeftEyeMesh,  FLinearColor(0.02f, 0.05f, 0.3f));
	Paint(RightEyeMesh, FLinearColor(0.02f, 0.05f, 0.3f));
	UpdatePowerVisuals();
}

void APlatformerCharacter::UpdatePowerVisuals()
{
	using namespace PlatformerVisuals;
	const bool bPunk = PowerLevel >= 1;
	const FLinearColor HairColor = bPunk ? FLinearColor(1.f, 0.35f, 0.7f) : FLinearColor(1.f, 0.8f, 0.1f);
	const FLinearColor DressColor = PowerLevel >= 2 ? FLinearColor(0.45f, 0.1f, 0.9f) : FLinearColor(0.8f, 0.05f, 0.1f);

	Paint(HairMesh, HairColor);
	Paint(DressMesh, DressColor);
	for (UStaticMeshComponent* Spike : PunkSpikes)
	{
		Spike->SetVisibility(bPunk);
		Paint(Spike, HairColor);
	}
}

void APlatformerCharacter::GainPower()
{
	PowerLevel = FMath::Min(PowerLevel + 1, 2);
	UpdatePowerVisuals();
}

void APlatformerCharacter::Fire()
{
	const float Now = GetWorld()->GetTimeSeconds();
	if (bDead || PowerLevel < 2 || Now - LastFireTime < 0.35f)
	{
		return;
	}
	LastFireTime = Now;

	const float Direction = GetActorForwardVector().X >= 0.f ? 1.f : -1.f;
	const FVector SpawnLocation = GetActorLocation() + FVector(Direction * 45.f, 0.f, 10.f);

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ADreamBubble* Bubble = GetWorld()->SpawnActor<ADreamBubble>(SpawnLocation, FRotator::ZeroRotator, Params))
	{
		Bubble->Fire(Direction);
	}
}

void APlatformerCharacter::CreateInputObjects()
{
	if (MappingContext)
	{
		return;
	}

	MoveAction = NewObject<UInputAction>(this, TEXT("IA_Move"));
	MoveAction->ValueType = EInputActionValueType::Axis1D;

	JumpAction = NewObject<UInputAction>(this, TEXT("IA_Jump"));
	JumpAction->ValueType = EInputActionValueType::Boolean;

	FireAction = NewObject<UInputAction>(this, TEXT("IA_Fire"));
	FireAction->ValueType = EInputActionValueType::Boolean;

	MappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_Platformer"));

	auto MapNegated = [this](const UInputAction* Action, const FKey& Key)
	{
		FEnhancedActionKeyMapping& Mapping = MappingContext->MapKey(Action, Key);
		Mapping.Modifiers.Add(NewObject<UInputModifierNegate>(MappingContext));
	};

	MappingContext->MapKey(MoveAction, EKeys::D);
	MappingContext->MapKey(MoveAction, EKeys::Right);
	MappingContext->MapKey(MoveAction, EKeys::Gamepad_DPad_Right);
	MapNegated(MoveAction, EKeys::A);
	MapNegated(MoveAction, EKeys::Left);
	MapNegated(MoveAction, EKeys::Gamepad_DPad_Left);
	{
		FEnhancedActionKeyMapping& Stick = MappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftX);
		Stick.Modifiers.Add(NewObject<UInputModifierDeadZone>(MappingContext));
	}

	MappingContext->MapKey(JumpAction, EKeys::SpaceBar);
	MappingContext->MapKey(JumpAction, EKeys::W);
	MappingContext->MapKey(JumpAction, EKeys::Up);
	MappingContext->MapKey(JumpAction, EKeys::Gamepad_FaceButton_Bottom);

	MappingContext->MapKey(FireAction, EKeys::F);
	MappingContext->MapKey(FireAction, EKeys::J);
	MappingContext->MapKey(FireAction, EKeys::LeftControl);
	MappingContext->MapKey(FireAction, EKeys::Gamepad_FaceButton_Left);
}

void APlatformerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	CreateInputObjects();
	if (const APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void APlatformerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	CreateInputObjects();
	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlatformerCharacter::Move);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		Input->BindAction(FireAction, ETriggerEvent::Started, this, &APlatformerCharacter::Fire);
	}
}

void APlatformerCharacter::Move(const FInputActionValue& Value)
{
	AddMovementInput(FVector::ForwardVector, Value.Get<float>());
}

void APlatformerCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Golpe con la cabeza: la normal del impacto apunta hacia abajo.
	if (bSelfMoved && Hit.ImpactNormal.Z < -0.5f)
	{
		if (APlatformerBlock* Block = Cast<APlatformerBlock>(Other))
		{
			Block->HitFromBelow(this);
		}
	}
}

void APlatformerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Caer a un foso: por debajo del suelo del nivel (Z = 0) no hay nada.
	if (!bDead && GetActorLocation().Z < -150.f)
	{
		Die(false);
	}

	if (InvulnerableTime > 0.f)
	{
		InvulnerableTime -= DeltaSeconds;
		// Parpadeo mientras dura la invulnerabilidad.
		const bool bVisible = InvulnerableTime <= 0.f || FMath::Fmod(InvulnerableTime, 0.2f) > 0.1f;
		VisualRoot->SetVisibility(bVisible, true);
	}
}

void APlatformerCharacter::ReceiveDamage()
{
	if (bDead || IsInvulnerable())
	{
		return;
	}

	if (PowerLevel > 0)
	{
		PowerLevel = 0;
		InvulnerableTime = 2.f;
		UpdatePowerVisuals();
	}
	else
	{
		Die();
	}
}

void APlatformerCharacter::BounceOffEnemy()
{
	LaunchCharacter(FVector(0.f, 0.f, 750.f), false, true);
}

void APlatformerCharacter::Die(bool bJumpOut)
{
	if (bDead)
	{
		return;
	}
	bDead = true;
	InvulnerableTime = 0.f;
	VisualRoot->SetVisibility(true, true);

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}

	// Animación clásica: salta hacia arriba y cae atravesando el escenario.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->SetMovementMode(MOVE_Falling);
	Movement->Velocity = FVector(0.f, 0.f, bJumpOut ? 900.f : 0.f);
	// La cámara se queda quieta mientras la jugadora cae.
	const FVector BoomLocation = CameraBoom->GetComponentLocation();
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->SetUsingAbsoluteLocation(true);
	CameraBoom->SetWorldLocation(BoomLocation);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &APlatformerCharacter::FinishDying, 2.f);
}

void APlatformerCharacter::CelebrateGoal()
{
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		DisableInput(PC);
	}
	InvulnerableTime = 0.f;
	VisualRoot->SetVisibility(true, true);
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(FVector(0.f, 0.f, 600.f), true, true);
}

void APlatformerCharacter::FinishDying()
{
	if (APlatformerGameMode* GameMode = GetWorld()->GetAuthGameMode<APlatformerGameMode>())
	{
		GameMode->OnPlayerDied(this);
	}
}
