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
	, NoSpeedLimit{ 10.f }
	// Dash
	, DashMultiplier{ 1.5f }
	, DashCooldown{ 0.5f }
	, DashTime{ 0.2f }
	, DashRotationSpeed{ 30.f }
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

	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;

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

	// If has no speed, set level to 0
	const bool hasNoSpeed{ velocityLength < NoSpeedLimit * NoSpeedLimit };
	if (hasNoSpeed) m_LevelToReach = 0;
	else
	{
		m_LevelToReach = m_CurrentSpeedLevel + 1;
	}

	// Check levelSwitch
	// =================

	// If max, return
	const int32 maxLevels{ SpeedLevels.Num() - 1 };
	if (m_CurrentSpeedLevel == maxLevels) return;

	// If locked, return
	if (SpeedLevels[m_LevelToReach].IsLocked) return;

	// If wants to go up and isn't locked
	if (0 < m_LevelToReach && !SpeedLevels[m_LevelToReach].IsLocked)
	{
		// Cooldown
		m_CurrentLevelReachTime += deltaTime;
		if (m_CurrentLevelReachTime < SpeedLevels[m_LevelToReach].ChangeTime) return;
	}

	// Set level and reset time
	m_CurrentSpeedLevel = m_LevelToReach;
	m_CurrentLevelReachTime = 0.f;

	// If back to 0
	if (m_CurrentSpeedLevel == 0)
	{
		// Lock second speedLevel
		SpeedLevels[1].IsLocked = true;
	}

	// Event
	if (OnSpeedSwitch.IsBound()) OnSpeedSwitch.Broadcast(m_CurrentSpeedLevel);


	// Set variables
	// =============
	
	KinematicController->MaxMovementSpeed = SpeedLevels[m_CurrentSpeedLevel].SpeedLimit;
}