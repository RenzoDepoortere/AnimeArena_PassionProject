#include "BaseCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "StateMachineComponent.h"
#include "Components/BoxComponent.h"

#include "../BasePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <Kismet/GameplayStatics.h>

ABaseCharacter::ABaseCharacter()
	// Components
	// ----------

	// Base
	: Collision{}
	, Mesh{}
	// Camera
	, SpringArm{}
	, Camera{}
	// Other
	, StateMachineComponent{}

	// Variables
	// ---------

	// Input
	, DefaultMappingContext{}
	, LookAction{}
	// Movement
	, MaxMovementSpeed{ 1000.f }
	, MoveAccelerationTime{ 0.2f }
	, JumpSpeed{ 500.f }
	, MaxJumpTime{ 0.2f }
	, AirControl{ 0.5f }
	, MaxFallSpeed{ 981.f }
	, FallAccelerationTime{ 0.5f }
	, RotationSpeed{ 20.f }
	, MaxSlopeAngle{ 60.f }
	// Other
	, CameraRotationSpeed{ 1.f }
	, SkinWidth{ 0.1f }
	, WallCollisionLength{ 15.f }
	, GroundCollisionLength{ 10.f }

	// Privates
	// --------

	// Base
	, m_pController{}
	// Movement
	, m_LastMovementInput{}
	, m_ShouldMove{ false }
	, m_CurrentDirection{}
	, m_DesiredRotation{}
	, m_MoveSpeed{}
	, m_NoMovementEvent{}
	, m_IsInAir{ true }
	, m_ShouldFall{ true }
	, m_LandEvent{}
	, m_TotalVelocity{}
{
 	// Settings
	// --------
	PrimaryActorTick.bCanEverTick = true;

	// Components
	// ----------

	// Base
	// ====

	// Collision
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName("Pawn");

	RootComponent = Collision;

	// Mesh
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName("CharacterMesh");

	// Camera
	// ======

	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Other
	// =====

	// StateMachine
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));

}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get controller
	m_pController = Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	// Add Input Mapping Context
	// ------------------------
	if (m_pController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* pSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(m_pController->GetLocalPlayer()))
		{
			pSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Subscribe to events
	// -------------------

	// Input
	if (m_pController)
	{
		m_pController->GetMoveEvent()->AddUObject(this, &ABaseCharacter::Move);
		m_pController->GetMoveStopEvent()->AddUObject(this, &ABaseCharacter::StopMove);
	}
}
void ABaseCharacter::Destroyed()
{
	// Unsubscribe from events
	// -----------------------

	// Input
	if (m_pController)
	{
		m_pController->GetMoveEvent()->RemoveAll(this);
		m_pController->GetMoveStopEvent()->RemoveAll(this);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckGround(DeltaTime);

	HandleGravity(DeltaTime);
	HandleMovement(DeltaTime);

	HandleWallCollision(DeltaTime);
	HandleDisplacement(DeltaTime);
}
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Looking
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		//enhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, this, &ABaseCharacter::LockToggle);
	}
}

void ABaseCharacter::MoveCharacter(const FVector2D& input, float inputMultiplier, bool rotateCharacter)
{
	// Set variables
	// -------------
	m_ShouldMove = true;

	// Calculate direction
	// -------------------

	// Get rotations
	const FRotator rotation = m_pController->GetControlRotation();
	const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	// Get directions
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// Calculate direction
	m_CurrentDirection = forwardDirection * input.Y + rightDirection * input.X;
	m_CurrentDirection.Normalize();

	// Calculate desiredRotation
	// -------------------------
	if (rotateCharacter) m_DesiredRotation = FRotationMatrix::MakeFromX(m_CurrentDirection).Rotator();

	// Limit input as requested
	m_CurrentDirection *= inputMultiplier;
}
void ABaseCharacter::RotateCharacter(const FVector2D& input)
{
	if (input.IsNearlyZero()) return;

	// Get rotations
	const FRotator rotation = m_pController->GetControlRotation();
	const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	// Get directions
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// Calculate direction
	FVector currentDirection{ forwardDirection * input.Y + rightDirection * input.X };
	currentDirection.Normalize();

	// Calculate desiredRotation
	// -------------------------
	m_DesiredRotation = FRotationMatrix::MakeFromX(currentDirection).Rotator();
}

void ABaseCharacter::Look(const FInputActionValue& value)
{
	// Input is a Vector2D
	FVector2D lookAxisVector = value.Get<FVector2D>();
	lookAxisVector *= CameraRotationSpeed;

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}

void ABaseCharacter::CheckGround(float deltaTime)
{
	const bool previousAirState{ m_IsInAir };

	// Raycast params
	// --------------

	// Hitresult
	FHitResult hitResult{};

	// Position
	const FVector startPos{ GetActorLocation() };
	const FVector endPos{ startPos + FVector::DownVector * (GroundCollisionLength + SkinWidth) };

	// Params
	const FCollisionShape capsuleShape{ Collision->GetCollisionShape(SkinWidth) };
	const FCollisionQueryParams traceParam{ "Raycast", true, this };

	// Raycast
	// -------
	if (GetWorld()->SweepSingleByChannel(hitResult, startPos, endPos, FQuat::Identity, ECollisionChannel::ECC_Visibility, capsuleShape, traceParam))
	{
		const double surfaceAngle{ FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, hitResult.Normal))) };

		// On ground or slope
		if (surfaceAngle <= MaxSlopeAngle)	m_IsInAir = false;
		// On Wall
		else								m_IsInAir = true;
	}
	// Nothing hit
	else
	{
		m_IsInAir = true;
	}

	// If landed on ground
	// -------------------
	if (previousAirState != m_IsInAir && m_IsInAir == false)
	{
		// Send event
		if (m_LandEvent.IsBound()) m_LandEvent.Broadcast();
	}
}

void ABaseCharacter::HandleGravity(float deltaTime)
{
	// Check conditions
	if (m_ShouldFall == false) return;

	// Apply gravity
	// -------------
	
	// Air
	if (m_IsInAir)
	{
		// Decrease Z component
		const float fallAcceleration = FMath::IsNearlyEqual(FallAccelerationTime, 0.f) ? MaxFallSpeed : MaxFallSpeed / FallAccelerationTime;
		m_TotalVelocity.Z -= fallAcceleration * deltaTime;

		// Make sure not going faster then max
		if (m_TotalVelocity.Z < -MaxFallSpeed) m_TotalVelocity.Z = -MaxFallSpeed;
	}
	// Grounded
	else
	{
		m_TotalVelocity.Z = 0.f;
	}
}
void ABaseCharacter::HandleMovement(float deltaTime)
{
	// Calculate movement
	// ------------------
	const float moveAcceleration = FMath::IsNearlyEqual(MoveAccelerationTime, 0.f) ? MaxMovementSpeed : MaxMovementSpeed / MoveAccelerationTime;

	// If input
	if (m_ShouldMove)
	{
		// Speed up
		m_MoveSpeed += moveAcceleration * deltaTime;
		if (MaxMovementSpeed < m_MoveSpeed) m_MoveSpeed = MaxMovementSpeed;
	}
	// No input
	else
	{
		// Slow down
		m_MoveSpeed -= moveAcceleration * deltaTime;
		if (m_MoveSpeed < 0)
		{
			m_MoveSpeed = 0;

			// Send event
			if (m_NoMovementEvent.IsBound()) m_NoMovementEvent.Broadcast();
		}
	}

	// Set total veloctity
	// -------------------
	const FVector horizontalVelocity{ m_CurrentDirection * m_MoveSpeed };

	m_TotalVelocity.X = horizontalVelocity.X;
	m_TotalVelocity.Y = horizontalVelocity.Y;

	// Set rotation
	// ------------
	const FRotator currentRotation{ GetActorRotation() };
	const float rotationSpeed{ RotationSpeed * deltaTime };

	const FRotator newRotation{ FMath::Lerp(currentRotation, m_DesiredRotation, rotationSpeed) };
	SetActorRotation(newRotation);
}

void ABaseCharacter::HandleWallCollision(float /*deltaTime*/)
{
	// Raycast params
	// --------------

	// Basics
	const FVector adjustedVelocity{ m_TotalVelocity.X, m_TotalVelocity.Y, 0 };
	const double velocitySize{ adjustedVelocity.Size() };
	const FVector velocityDirection{ adjustedVelocity.GetSafeNormal() };

	// Hitresult
	FHitResult hitResult{};

	// Position
	const FVector startPos{ GetActorLocation() };
	const FVector endPos{ startPos + velocityDirection * (WallCollisionLength + SkinWidth) };

	// Params
	const FCollisionShape capsuleShape{ Collision->GetCollisionShape(SkinWidth) };
	const FCollisionQueryParams traceParam{ "Raycast", true, this };

	// Raycast
	// -------
	if (GetWorld()->SweepSingleByChannel(hitResult, startPos, endPos, FQuat::Identity, ECollisionChannel::ECC_Visibility, capsuleShape, traceParam))
	{
		const double surfaceAngle{ FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, hitResult.Normal))) };
	
		// Wall
		if (MaxSlopeAngle < surfaceAngle)	SlideAlongWall(adjustedVelocity, hitResult.Normal);
		// Slope
		else								SlideAlongSlope(adjustedVelocity, hitResult.Normal);
	}
}
void ABaseCharacter::HandleDisplacement(float deltaTime)
{
	const FVector displacement{ m_TotalVelocity * deltaTime };
	AddActorWorldOffset(displacement, true);
}

void ABaseCharacter::SlideAlongWall(const FVector& velocity, const FVector& hitNormal)
{
	FVector hitSurface{ FVector::CrossProduct(FVector::UpVector, hitNormal) };
	hitSurface.Normalize();

	const FVector desiredVelocity{ velocity.ProjectOnTo(hitSurface) };
	m_TotalVelocity.X = desiredVelocity.X;
	m_TotalVelocity.Y = desiredVelocity.Y;
}
void ABaseCharacter::SlideAlongSlope(const FVector& velocity, const FVector& hitNormal)
{
	const FVector desiredVelocity{ FVector::VectorPlaneProject(velocity, hitNormal) };
	m_TotalVelocity.X = desiredVelocity.X;
	m_TotalVelocity.Y = desiredVelocity.Y;
	m_TotalVelocity.Z = desiredVelocity.Z;
}