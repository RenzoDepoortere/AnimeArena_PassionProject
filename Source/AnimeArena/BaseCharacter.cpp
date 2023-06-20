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

// Sets default values
ABaseCharacter::ABaseCharacter()
	: m_HasToSlowDown{ false }
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
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Add Input Mapping Context
	// ------------------------
	if (APlayerController* pPlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* pSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pPlayerController->GetLocalPlayer()))
		{
			pSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Get movementSpeed
	// -----------------
	auto pCharacterMovement{ GetCharacterMovement() };
	m_MaxWalkingSpeed = pCharacterMovement->MaxWalkSpeed;
	m_MaxAcceleration = pCharacterMovement->MaxAcceleration;
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(m_HasToSlowDown) Slowdown(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABaseCharacter::StopMove);

		// Sprinting
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABaseCharacter::Sprint);
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABaseCharacter::StopSprinting);

		// Jumping
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Looking
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
	}
}

void ABaseCharacter::Move(const FInputActionValue& value)
{
	// Input is a Vector2D
	FVector2D movementVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator yawRotation{ 0, rotation.Yaw, 0 };

		// Get forward vector
		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

		// Get right vector 
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		AddMovementInput(forwardDirection, movementVector.Y);
		AddMovementInput(rightDirection, movementVector.X);
	}
}
void ABaseCharacter::StopMove(const FInputActionValue& value)
{
	if (Controller == nullptr) return;

	// When no movement, no sprinting as well
	StopSprinting(value);
}

void ABaseCharacter::Sprint(const FInputActionValue& /*value*/)
{
	if (Controller == nullptr) return;
	m_HasToSlowDown = false;

	// Set new maxSpeed and acceleration
	auto pCharacterMovement{ GetCharacterMovement() };
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed * MaxMovementSpeedMult;
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration * MaxAccelerationSpeedMult;
}
void ABaseCharacter::StopSprinting(const FInputActionValue& /*value*/)
{
	if (Controller == nullptr) return;
	m_HasToSlowDown = true;

	// Reset maxSpeed and maxAcceleration
	auto pCharacterMovement{ GetCharacterMovement() };
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration;
}

void ABaseCharacter::Look(const FInputActionValue& value)
{
	// Input is a Vector2D
	FVector2D lookAxisVector = value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}

void ABaseCharacter::Slowdown(float DeltaTime)
{
	// Slowdown after sprinting
	// ------------------------
	auto pCharacterMovement{ GetCharacterMovement() };

	// Calculate new maxWalkSpeed
	const float acceleration{ pCharacterMovement->MaxAcceleration * (MaxAccelerationSpeedMult * 2.f) };
	float maxWalkSpeed{ pCharacterMovement->MaxWalkSpeed };
	maxWalkSpeed -= acceleration * DeltaTime;

	// Check if back to normal
	if (maxWalkSpeed <= m_MaxWalkingSpeed)
	{
		// Reset
		maxWalkSpeed = m_MaxWalkingSpeed;
		m_HasToSlowDown = false;
	}

	// Set speed
	pCharacterMovement->MaxWalkSpeed = maxWalkSpeed;
}