// Fill out your copyright notice in the Description page of Project Settings.
#include "SprintState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "../BaseCharacter.h"

USprintState::USprintState()
{
	StateDisplayName = "Sprint";
}

void USprintState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Get movementSpeed
	auto pCharacter{ GetCharacter() };

	auto pCharacterMovement{ pCharacter->GetCharacterMovement() };
	m_MaxWalkingSpeed = pCharacterMovement->MaxWalkSpeed;
	m_MaxMoveMult = pCharacter->MaxMovementSpeedMult;

	// Give boost
	FVector impulseForce{ pCharacter->GetVelocity() };
	impulseForce.Normalize();
	impulseForce *= pCharacter->SprintBoost;

	pCharacterMovement->AddImpulse(impulseForce, true);

	// Set new maxSpeed and acceleration
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed * m_MaxMoveMult;

	// Set animation

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &USprintState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &USprintState::StopMove);

		pController->GetSprintStopEvent()->AddUObject(this, &USprintState::StopSprint);
		pController->GetJumpEvent()->AddUObject(this, &USprintState::Jump);
		pController->GetDashEvent()->AddUObject(this, &USprintState::Dash);
	}
}
void USprintState::OnExit()
{
	// Reset moveSpeed
	auto pCharacterMovement{ GetCharacter()->GetCharacterMovement() };
	pCharacterMovement->MaxWalkSpeed = m_MaxWalkingSpeed;

	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetMoveStopEvent()->RemoveAll(this);

		pController->GetSprintStopEvent()->RemoveAll(this);
		pController->GetJumpEvent()->RemoveAll(this);
		pController->GetDashEvent()->RemoveAll(this);
	}
}
void USprintState::Tick(float /*deltaTime*/)
{
	// Check if is falling
	if (GetCharacter()->GetCharacterMovement()->IsFalling())
	{
		// Change to jumpState
		auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Jump" });
	}
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
	// Change state to moveState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Move" });
}
void USprintState::Jump()
{
	// Change to jumpState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Jump" });
}
void USprintState::Dash()
{
	// Change to dashState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Dash" });
}