#include "BaseCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "StateMachineComponent.h"
#include "KinematicController.h"

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
	, KinematicController{}

	// Events
	// ------
	, OnSpeedSwitch{}

	// Variables
	// ---------

	// Input
	, DefaultMappingContext{}
	, LookAction{}
	// Movement
	// ========

	// SpeedLevel
	, Level1_SpeedLimit{ 100.f }
	, Level2_SpeedLimit{ 500.f }
	, Level3_SpeedLimit{ 1000.f }
	, Level4_SpeedLimit{ 1500.f }
	, SpeedLimitTreshold{ 50.f }
	// Dash
	, DashSpeed{ 1500.f }
	, DashPerfectMultiplier{ 1.5f }
	, DashCooldown{ 0.5f }
	, DashGraceTime{ 0.1f }
	// Other
	, CameraRotationSpeed{ 1.f }

	// Privates
	// --------

	// Base
	, m_pController{}
	// Movement
	, m_LastMovementInput{}
	, m_CurrentSpeedLevel{ ESpeedLevel::Level_1 }
	, m_SpeedLevelTresholds{}
	, m_CurrentDashTime{}
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

	// Kinematic controller
	KinematicController = CreateDefaultSubobject<UKinematicController>(TEXT("KinematicController"));

}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get controller
	m_pController = Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	// Calculate speedTresholds
	// ------------------------

	m_SpeedLevelTresholds.Add((Level1_SpeedLimit * Level1_SpeedLimit) - SpeedLimitTreshold);
	m_SpeedLevelTresholds.Add((Level2_SpeedLimit * Level2_SpeedLimit) - SpeedLimitTreshold);
	m_SpeedLevelTresholds.Add((Level3_SpeedLimit * Level3_SpeedLimit) - SpeedLimitTreshold);
	m_SpeedLevelTresholds.Add((Level4_SpeedLimit * Level4_SpeedLimit) - SpeedLimitTreshold);

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

	HandleSpeedLimit();
	HandleDashTimer(DeltaTime);
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

void ABaseCharacter::Dash()
{
	// On cooldown
	if (0 < m_CurrentDashTime) return;

	// Calculate dashSpeed
	const bool isPerfectTimed{  -DashGraceTime <= m_CurrentDashTime };
	const float dashSpeed = isPerfectTimed ? DashSpeed * DashPerfectMultiplier : DashSpeed;

	// Apply speed
	const FVector dashDirection{ KinematicController->ConvertInputToWorld(m_LastMovementInput) * dashSpeed };
	KinematicController->AddForce(dashDirection, true);

	// Reset timer
	m_CurrentDashTime = DashCooldown;

	// Debug
	if (isPerfectTimed) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, "PERFECT!");
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

void ABaseCharacter::HandleSpeedLimit()
{
	// Get current variables
	const FVector totalVelocity{ KinematicController->GetTotalVelocity() };
	const double velocityLength{ FVector::VectorPlaneProject(totalVelocity, FVector::UpVector).SquaredLength() };
	const ESpeedLevel startSpeedLevel{ m_CurrentSpeedLevel };

	// Set speedLevel
	for (int32 idx{}; idx < m_SpeedLevelTresholds.Num(); ++idx)
	{
		if (velocityLength < m_SpeedLevelTresholds[idx])
		{
			m_CurrentSpeedLevel = static_cast<ESpeedLevel>(idx);
			break;
		}
	}

	// Change variables if necessary
	if (m_CurrentSpeedLevel == startSpeedLevel) return;
	if (OnSpeedSwitch.IsBound()) OnSpeedSwitch.Broadcast(static_cast<int>(m_CurrentSpeedLevel) + 1);

	switch (m_CurrentSpeedLevel)
	{
	case ESpeedLevel::Level_1:
		KinematicController->MaxMovementSpeed = Level1_SpeedLimit;
		break;
	case ESpeedLevel::Level_2:
		KinematicController->MaxMovementSpeed = Level2_SpeedLimit;
		break;
	case ESpeedLevel::Level_3:
		KinematicController->MaxMovementSpeed = Level3_SpeedLimit;
		break;
	case ESpeedLevel::Level_4:
		KinematicController->MaxMovementSpeed = Level4_SpeedLimit;
		break;
	}
}
void ABaseCharacter::HandleDashTimer(float deltaTime)
{
	if (m_CurrentDashTime <= -DashGraceTime) return;
	m_CurrentDashTime -= deltaTime;
}