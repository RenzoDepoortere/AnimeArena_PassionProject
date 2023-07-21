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
#include "Components/BoxComponent.h"
#include "BaseAbility.h"
#include <Kismet/GameplayStatics.h>

ABaseCharacter::ABaseCharacter()
	: Abilities{}
	, m_pController{ nullptr }
	, m_LastMovementInput{}
	, m_LastAttackWasLight{ false }
	, m_UsedAirAbility{ false }
	, m_UsedAirDash{ false }
	, m_IsLocked{ false }
	, m_pLockedCharacter{ nullptr }
	, m_AttackEndEvent{}
	, m_HitActors{}
	, m_IsActiveHit{ false }
	, m_CurrentHealth{}
	, m_MeshMaterials{}
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

	// Change collisions
	// -----------------
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

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

	// BoxCollision
	DamageBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBoxCollision"));
	DamageBoxCollision->SetupAttachment(RootComponent);

	// StateMachine
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get controller and stateMachine
	m_pController = Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	// Health
	// ------

	// Set health to max
	m_CurrentHealth = MaxHealth;

	// Store originalMaterials
	m_MeshMaterials = GetMesh()->GetMaterials();

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

	// Subscribe to events
	// -------------------

	// Input
	if (m_pController)
	{
		m_pController->GetMoveEvent()->AddUObject(this, &ABaseCharacter::Move);
		m_pController->GetMoveStopEvent()->AddUObject(this, &ABaseCharacter::StopMove);

		m_pController->GetLightAttackEvent()->AddUObject(this, &ABaseCharacter::LightAttack);
		m_pController->GetHeavyAttackEvent()->AddUObject(this, &ABaseCharacter::HeavyAttack);

		m_pController->GetAbility1Event()->AddUObject(this, &ABaseCharacter::Ability1);
		m_pController->GetAbility2Event()->AddUObject(this, &ABaseCharacter::Ability2);
		m_pController->GetAbility3Event()->AddUObject(this, &ABaseCharacter::Ability3);
		m_pController->GetAbility4Event()->AddUObject(this, &ABaseCharacter::Ability4);
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

		m_pController->GetLightAttackEvent()->RemoveAll(this);
		m_pController->GetHeavyAttackEvent()->RemoveAll(this);

		m_pController->GetAbility1Event()->RemoveAll(this);
		m_pController->GetAbility2Event()->RemoveAll(this);
		m_pController->GetAbility3Event()->RemoveAll(this);
		m_pController->GetAbility4Event()->RemoveAll(this);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_IsActiveHit) HandleAttacks();
	if (m_IsLocked) FollowLockedCharacter();

	if (0 < m_CurrentHitTime)
	{
		m_CurrentHitTime -= DeltaTime;
		if (m_CurrentHitTime <= 0.f) ResetMaterials();
	}

	for (const auto& currentAbility : Abilities)
	{
		if (currentAbility->GetIsActive()) currentAbility->Update(DeltaTime);
	}
}

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

void ABaseCharacter::IsActiveHit(bool isActiveHit)
{
	m_IsActiveHit = isActiveHit;

	// If should start looking for actors
	if (isActiveHit)
	{
		// Include self, you want to ignore
		m_HitActors.Add(this);
	}
	else
	{
		// Clear actors
		m_HitActors.Empty();
	}
}

void ABaseCharacter::SetHealth(float amount)
{
	m_CurrentHealth = amount;
	if (MaxHealth < m_CurrentHealth) m_CurrentHealth = MaxHealth;
}
bool ABaseCharacter::DealDamage(float amount, ABaseCharacter* pDamageDealer)
{
	// If currentState is invincible, return
	if (Cast<UBasePlayerState>(StateMachineComponent->CurrentState)->GetExtraStateInfo().isInvincible) return false;

	m_CurrentHealth -= amount;
	if (amount <= 0)
	{
		amount = 0;

		OnDeath(amount, pDamageDealer);
	}
	else OnDamage(amount, pDamageDealer);

	return true;
}

void ABaseCharacter::FaceActor(AActor* pActor)
{
	// Face lockedChar
	const FVector lockedCharPos{ pActor->GetActorLocation() };
	const FVector toDirection{ FVector::VectorPlaneProject((lockedCharPos - GetActorLocation()), FVector{0.f, 0.f, 1.f}) };
	const FRotator desiredRotation{ FRotationMatrix::MakeFromX(toDirection).Rotator() };

	SetActorRotation(desiredRotation);
}

void ABaseCharacter::OnDamage(float /*amount*/, ABaseCharacter* pDamageDealer)
{
	// Knockback
	// ---------

	// Get directions
	FVector knockbackDirection{};
	FVector damageDealerToSelf{ (GetActorLocation() - pDamageDealer->GetActorLocation()) };
	damageDealerToSelf.Normalize();

	switch (pDamageDealer->CurrentAttack.knockbackDirection)
	{
	case EAttackDirectionEnum::forward:
		knockbackDirection = damageDealerToSelf;
		break;

	case EAttackDirectionEnum::backward:
		knockbackDirection = -damageDealerToSelf;
		break;

	case EAttackDirectionEnum::left:
		knockbackDirection = damageDealerToSelf.Cross({ 0.f, 0.f, 1.f });
		break;

	case EAttackDirectionEnum::right:
		knockbackDirection = -damageDealerToSelf.Cross({ 0.f, 0.f, 1.f });
		break;

	case EAttackDirectionEnum::up:
		knockbackDirection = FVector{ 0.f, 0.f, 1.f };
		break;

	case EAttackDirectionEnum::down:
		knockbackDirection = GetCharacterMovement()->IsFalling() ? FVector{0.f, 0.f, -1.f} : FVector{};
		break;
	}

	// Add knockback
	knockbackDirection *= pDamageDealer->CurrentAttack.knockback;
	GetCharacterMovement()->AddImpulse(knockbackDirection * 1'000);

	// Rotate towards damageDealer
	FaceActor(pDamageDealer);

	// State
	// -----

	// Store hit variables
	LastHitStun = pDamageDealer->CurrentAttack.HitStunTime;
	LastWasFinisher = pDamageDealer->CurrentAttack.isComboEnder;

	// Change to hitState
	StateMachineComponent->SwitchStateByKey("Hit");

	// Materials
	// ---------

	// Change all materials to white
	auto pMesh{ GetMesh() };
	for (int32 idx{}; idx < m_MeshMaterials.Num(); idx++)
	{
		pMesh->SetMaterial(idx, FlickerMaterial);
	}

	// Set currentHitTime
	const float maxHitTime{ 0.1f };
	m_CurrentHitTime = maxHitTime;
}
void ABaseCharacter::OnDeath(float /*amount*/, ABaseCharacter* /*pKiller*/)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, "Ded");
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
void ABaseCharacter::HandleAttacks()
{
	// Return if not hitting player in active attack state
	if (StateMachineComponent->CurrentState->StateDisplayName != "Attack") return;
	if (CurrentAttackState != EAttackEnum::active) return;

	TArray<AActor*> overlappingActors{};
	DamageBoxCollision->GetOverlappingActors(overlappingActors, ABaseCharacter::StaticClass());

	// Return if not overlappingActors
	if (overlappingActors.Num() == 0) return;

	// Go through overlappingActors
	bool hasAlreadyBeenHit{ false };
	for (const auto& currentHitActor : overlappingActors)
	{
		hasAlreadyBeenHit = false;

		// Check if actor already got hit
		for (const auto& storedActor : m_HitActors)
			if (currentHitActor == storedActor) hasAlreadyBeenHit = true;

		if (hasAlreadyBeenHit) continue;

		// Else add to hitActors and dealDamage
		m_HitActors.Add(currentHitActor);
		Cast<ABaseCharacter>(currentHitActor)->DealDamage(CurrentAttack.damage, this);
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

void ABaseCharacter::ResetMaterials()
{
	// Set each materials back to normal
	auto pMesh{ GetMesh() };
	for (int32 idx{}; idx < m_MeshMaterials.Num(); idx++)
	{
		pMesh->SetMaterial(idx, m_MeshMaterials[idx]);
	}
}