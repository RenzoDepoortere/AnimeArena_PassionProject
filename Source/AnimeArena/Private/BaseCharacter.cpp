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
	, SpeedLevels{}
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
	, m_CurrentSpeedLevel{ 0 }
	, m_LevelToReach{ 0 }
	, m_CurrentLevelReachTime{}
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

	int treshold{};
	for (int32 idx{}; idx < SpeedLevels.Num(); ++idx)
	{
		treshold = (SpeedLevels[idx].SpeedLimit * SpeedLevels[idx].SpeedLimit) - SpeedLimitTreshold;
		m_SpeedLevelTresholds.Add(treshold);
	}

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

	HandleSpeedLimit(DeltaTime);
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

void ABaseCharacter::HandleSpeedLimit(float deltaTime)
{
	// Check currentSpeed
	// ==================

	// Get current variables
	const FVector totalVelocity{ KinematicController->GetTotalVelocity() };
	const double velocityLength{ FVector::VectorPlaneProject(totalVelocity, FVector::UpVector).SquaredLength() };
	const int32 startSpeedLevel{ m_LevelToReach };

	// Set speedLevel
	for (int32 idx{}; idx < m_SpeedLevelTresholds.Num(); ++idx)
	{
		if (velocityLength < m_SpeedLevelTresholds[idx])
		{
			m_LevelToReach = idx;
			break;
		}
	}
	
	// Check levelSwitch
	// =================

	// If max, return
	const int32 maxLevels{ SpeedLevels.Num() - 1 };
	if (m_LevelToReach == maxLevels && m_CurrentSpeedLevel == maxLevels) return;

	// If wants to go up
	if ((m_CurrentSpeedLevel == m_LevelToReach) || (m_CurrentSpeedLevel + 1 == m_LevelToReach))
	{
		// Cooldown
		m_CurrentLevelReachTime += deltaTime;
		if (m_CurrentLevelReachTime < SpeedLevels[m_CurrentSpeedLevel + 1].ChangeTime) return;
	}

	// Set level and reset time
	m_CurrentSpeedLevel = m_LevelToReach;
	m_CurrentLevelReachTime = 0.f;

	// Event
	if (OnSpeedSwitch.IsBound()) OnSpeedSwitch.Broadcast(m_CurrentSpeedLevel + 1);


	// Set variables
	// =============
	
	// Set speedLimit
	KinematicController->MaxMovementSpeed = SpeedLevels[m_CurrentSpeedLevel].SpeedLimit;
}
void ABaseCharacter::HandleDashTimer(float deltaTime)
{
	if (m_CurrentDashTime <= -DashGraceTime) return;
	m_CurrentDashTime -= deltaTime;
}