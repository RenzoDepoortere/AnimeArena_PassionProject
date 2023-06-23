// Fill out your copyright notice in the Description page of Project Settings.
#include "SprintState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "../BaseCharacter.h"

void USprintState::OnEnter(AActor* pStateOwner)
{
	Super::OnEnter(pStateOwner);

	// Reset variables
	m_HasToSlowDown = false;

	// Get movementSpeed
	auto pCharacter{ GetCharacter() };

	auto pCharacterMovement{ pCharacter->GetCharacterMovement() };
	m_MaxWalkingSpeed = pCharacterMovement->MaxWalkSpeed;
	m_MaxAcceleration = pCharacterMovement->MaxAcceleration;

	m_MaxMoveMult = pCharacter->MaxMovementSpeedMult;
	m_MaxAccelMult = pCharacter->MaxAccelerationSpeedMult;

	// Set new maxSpeed and acceleration
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed * m_MaxMoveMult;
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration * m_MaxAccelMult;

	// Set animation

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
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
	auto pCharacterMovement{ GetCharacter()->GetCharacterMovement() };
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed;
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration;

	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
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
	// BaseMove
	const FVector2D movementVector{ value.Get<FVector2D>() };
	BaseMove(movementVector);
}
void USprintState::StopMove()
{
	// Change to idleState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>()};
	pStateMachine->SwitchStateByKey({ "Idle" });
}

void USprintState::StopSprint()
{
	// Reset maxSpeed and maxAcceleration
	auto pCharacterMovement{ GetCharacter()->GetCharacterMovement() };
	pCharacterMovement->MaxAcceleration = m_MaxAcceleration;

	m_HasToSlowDown = true;
}

void USprintState::Slowdown(float deltaTime)
{
	// Slowdown after sprinting
	// ------------------------
	auto pCharacterMovement{ GetCharacter()->GetCharacterMovement() };

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
		auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Move" });
	}

	// Set speed
	pCharacterMovement->MaxWalkSpeed = maxWalkSpeed;
}