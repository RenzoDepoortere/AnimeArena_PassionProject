// Fill out your copyright notice in the Description page of Project Settings.
#include "SprintState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "../BaseCharacter.h"

#include <Kismet/GameplayStatics.h>

void USprintState::OnEnter(AActor* pStateOwner)
{
	Super::OnEnter(pStateOwner);

	// Reset variables
	m_pCharacter = Cast<ACharacter>(pStateOwner);
	m_HasToSlowDown = false;

	// Get movementSpeed
	auto pCharacterMovement{ m_pCharacter->GetCharacterMovement() };
	m_MaxWalkingSpeed = pCharacterMovement->MaxWalkSpeed;
	m_MaxAcceleration = pCharacterMovement->MaxAcceleration;

	auto pBaseChar{ Cast<ABaseCharacter>(m_pCharacter) };
	m_MaxMoveMult = pBaseChar->MaxMovementSpeedMult;
	m_MaxAccelMult = pBaseChar->MaxAccelerationSpeedMult;

	// Set new maxSpeed and acceleration
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed * m_MaxMoveMult;
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration * m_MaxAccelMult;

	// Set animation

	// Subscribe to inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &USprintState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &USprintState::StopMove);

		pController->GetSprintStopEvent()->AddUObject(this, &USprintState::StopSprint);
	}
}
void USprintState::OnExit()
{
	// Reset moveSpeed and accelSpeed
	auto pCharacterMovement{ m_pCharacter->GetCharacterMovement() };
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed;
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration;

	// Unsubscribe from inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetMoveStopEvent()->RemoveAll(this);

		pController->GetSprintStopEvent()->RemoveAll(this);
	}
}

void USprintState::Tick(float deltaTime)
{
	if (m_HasToSlowDown) Slowdown(deltaTime);
}

void USprintState::Move(const FInputActionValue& value)
{
	// Input is a Vector2D
	FVector2D movementVector = value.Get<FVector2D>();

	if (m_pCharacter->Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator rotation = m_pCharacter->Controller->GetControlRotation();
		const FRotator yawRotation{ 0, rotation.Yaw, 0 };

		// Get forward vector
		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

		// Get right vector 
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		m_pCharacter->AddMovementInput(forwardDirection, movementVector.Y);
		m_pCharacter->AddMovementInput(rightDirection, movementVector.X);
	}
}
void USprintState::StopMove()
{
	// Change to idleState
	auto pStateMachine{ m_pCharacter->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

void USprintState::StopSprint()
{
	// Reset maxSpeed and maxAcceleration
	auto pCharacterMovement{ m_pCharacter->GetCharacterMovement() };
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration;

	m_HasToSlowDown = true;
}

void USprintState::Slowdown(float deltaTime)
{
	// Slowdown after sprinting
	// ------------------------
	auto pCharacterMovement{ m_pCharacter->GetCharacterMovement() };

	// Calculate new maxWalkSpeed
	const float acceleration{ pCharacterMovement->MaxAcceleration * (m_MaxAccelMult * 2.f) };
	float maxWalkSpeed{ pCharacterMovement->MaxWalkSpeed };
	maxWalkSpeed -= acceleration * deltaTime;

	// Check if back to normal
	if (maxWalkSpeed <= m_MaxWalkingSpeed)
	{
		// Reset
		maxWalkSpeed = m_MaxWalkingSpeed;
		m_HasToSlowDown = false;

		// Change state to moveState
		auto pStateMachine{ m_pCharacter->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Move" });
	}

	// Set speed
	pCharacterMovement->MaxWalkSpeed = maxWalkSpeed;
}