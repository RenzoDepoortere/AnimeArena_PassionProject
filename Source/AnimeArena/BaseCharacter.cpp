// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BasePlayerController.h"
#include "StateMachineComponent.h"
#include "BasePlayerState.h"
#include "HealthComponent.h"

#include <Kismet/GameplayStatics.h>


// Sets default values
ABaseCharacter::ABaseCharacter()
	: m_pController{ nullptr }
	, m_LastMovementInput{}
	, m_LastAttackWasLight{ false }
	, m_UsedAirAbility{ false }
	, m_UsedAirDash{ false }
	, m_IsLocked{ false }
	, m_pLockedCharacter{ nullptr }
	, m_AttackEndEvent{}
{
	// Init components
	// ***************

	// CharacterControllerComponent
	// ----------------------------

	// Let camera control rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	auto pCharacterMovement{ GetCharacterMovement() };
	pCharacterMovement->bOrientRotationToMovement = true;
	pCharacterMovement->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Default variables
	pCharacterMovement->JumpZVelocity = 700.f;
	pCharacterMovement->AirControl = 0.35f;

	pCharacterMovement->MaxWalkSpeed = 500.f;
	pCharacterMovement->MinAnalogWalkSpeed = 20.f;
	pCharacterMovement->BrakingDecelerationWalking = 2000.f;

	// Return if can't be controlled
	if (CanBeControlled == false) return;

	// Camera
	// ------

	// Create a camera springArm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Other components
	// ----------------

	// StateMachine
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));

	// HealthComponent
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
	HealthComponent->MaxHealth = 100.f;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get controller and stateMachine
	m_pController = Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	// Return if can't be controlled
	if (CanBeControlled == false) return;

	// Add Input Mapping Context
	// ------------------------
	if (m_pController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* pSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(m_pController->GetLocalPlayer()))
		{
			pSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Subscribe to inputEvents
	// ------------------------
	if (m_pController)
	{
		m_pController->GetMoveEvent()->AddUObject(this, &ABaseCharacter::Move);
		m_pController->GetMoveStopEvent()->AddUObject(this, &ABaseCharacter::StopMove);

		m_pController->GetLightAttackEvent()->AddUObject(this, &ABaseCharacter::LightAttack);
		m_pController->GetHeavyAttackEvent()->AddUObject(this, &ABaseCharacter::HeavyAttack);
	}
}
void ABaseCharacter::Destroyed()
{
	// Unsubscribe from inputEvents
	if (m_pController)
	{
		m_pController->GetMoveEvent()->RemoveAll(this);
		m_pController->GetMoveStopEvent()->RemoveAll(this);

		m_pController->GetLightAttackEvent()->RemoveAll(this);
		m_pController->GetHeavyAttackEvent()->RemoveAll(this);
	}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_IsLocked) FollowLockedCharacter();
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Return if can't be controlled
	if (CanBeControlled == false) return;

	// Set up action bindings
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Looking
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		enhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, this, &ABaseCharacter::LockToggle);
	}
}

void ABaseCharacter::FaceLockedCharacter()
{
	// Face lockedChar
	const FVector lockedCharPos{ m_pLockedCharacter->GetActorLocation() };
	const FVector toDirection{ FVector::VectorPlaneProject((lockedCharPos - GetActorLocation()), FVector{0.f, 0.f, 1.f}) };
	const FRotator desiredRotation{ FRotationMatrix::MakeFromX(toDirection).Rotator() };

	SetActorRotation(desiredRotation);
}

void ABaseCharacter::LightAttack()
{ 
	// Store input
	m_LastAttackWasLight = true; 

	// If currentState is attack, return
	if (StateMachineComponent->CurrentState->StateDisplayName == "Attack") return;

	// Check if currentState can be attackCancelable
	if (Cast<UBasePlayerState>(StateMachineComponent->CurrentState)->GetExtraStateInfo().canBeAttackCanceled)
	{
		// Switch to attackState
		StateMachineComponent->SwitchStateByKey("Attack");
	}
}
void ABaseCharacter::HeavyAttack()
{
	// Store input
	m_LastAttackWasLight = false;

	// If currentState is attack, return
	if (StateMachineComponent->CurrentState->StateDisplayName == "Attack") return;

	// Check if currentState can be attackCancelable
	if (Cast<UBasePlayerState>(StateMachineComponent->CurrentState)->GetExtraStateInfo().canBeAttackCanceled)
	{
		// Switch to attackState
		StateMachineComponent->SwitchStateByKey("Attack");
	}
}

void ABaseCharacter::Look(const FInputActionValue& value)
{
	// Return if locked
	if (m_IsLocked) return;

	// Input is a Vector2D
	FVector2D lookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}

void ABaseCharacter::LockToggle()
{
	// If is locked, unlock
	if (m_IsLocked)
	{
		m_IsLocked = false;
		m_pLockedCharacter = nullptr;
		return;
	}

	// Get all baseCharacters
	TArray<AActor*> pCharacters{};
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), pCharacters);

	// Get viewportSize
	int32 screenWidth{};
	int32 screenHeight{};
	m_pController->GetViewportSize(screenWidth, screenHeight);

	auto isOnScreen = [&](const FVector2D& screenPosition)
	{
		return (0 <= screenPosition.X && screenPosition.X <= screenWidth) && (0 <= screenPosition.Y && screenPosition.Y <= screenHeight);
	};

	// Loop through characters
	const FVector actorPos{ GetActorLocation() };
	FVector currentActorPos{};
	
	FVector2D screenPosition{};
	double currentDistance{};

	double closestDistance{ DBL_MAX };
	AActor* pClosestActor{ nullptr };

	for (const auto& currentCharacter : pCharacters)
	{
		// If is on actor, continue
		if (currentCharacter == this) continue;

		// If is not on screen, continue
		currentActorPos = currentCharacter->GetActorLocation();
		if (m_pController->ProjectWorldLocationToScreen(currentActorPos, screenPosition, true) == false) continue;
		if (isOnScreen(screenPosition) == false) continue;

		// Check if is closer then other character
		currentDistance = FVector::DistSquared(actorPos, currentActorPos);
		if (currentDistance < closestDistance)
		{
			// Store distance
			closestDistance = currentDistance;
			pClosestActor = currentCharacter;
		}
	}

	// If found closestCharacter
	if (pClosestActor)
	{
		// Toggle lockstate
		m_IsLocked = true;
		m_pLockedCharacter = Cast<ABaseCharacter>(pClosestActor);
	}
}

void ABaseCharacter::FollowLockedCharacter()
{
	// Return if lockedChar is not valid
	if (m_pLockedCharacter->IsValidLowLevel() == false) return;

	// Calculate desired rotation
	const FVector lockedCharPos{ m_pLockedCharacter->GetActorLocation() };
	const FVector toDirection{ lockedCharPos - GetActorLocation() };

	FRotator controlRotation{ GetControlRotation() };
	const FRotator desiredRotation{ FRotationMatrix::MakeFromX(toDirection).Rotator() };

	const FVector rotationToMove{ (desiredRotation - controlRotation).Euler() };

	// Add input to controller
	m_pController->SetControlRotation(controlRotation.Add(rotationToMove.Y, rotationToMove.Z, rotationToMove.X));
}