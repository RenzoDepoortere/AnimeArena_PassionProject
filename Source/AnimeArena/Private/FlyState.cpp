// Fill out your copyright notice in the Description page of Project Settings.
#include "FlyState.h"
#include "StateMachineComponent.h"
#include "../BaseCharacter.h"
#include "../BasePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

UFlyState::UFlyState()
{
	StateDisplayName = "Flying";
}

void UFlyState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Set to flying
	auto pCharMovement{ GetCharacter()->GetCharacterMovement() };
	pCharMovement->MovementMode = EMovementMode::MOVE_Flying;

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UFlyState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &UFlyState::StopMove);

		pController->GetJumpHoldEvent()->AddUObject(this, &UFlyState::FlyUp);
		pController->GetJumpStopEvent()->AddUObject(this, &UFlyState::StopFlyUp);

		pController->GetSprintDoubleTapEvent()->AddUObject(this, &UFlyState::StopFly);
		pController->GetSprintEvent()->AddUObject(this, &UFlyState::FlyDown);
		pController->GetSprintStopEvent()->AddUObject(this, &UFlyState::StopFlyDown);
	}
}
void UFlyState::OnExit()
{
	// Stop flying
	auto pCharMovement{ GetCharacter()->GetCharacterMovement() };
	pCharMovement->MovementMode = EMovementMode::MOVE_Walking;

	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetMoveStopEvent()->RemoveAll(this);

		pController->GetJumpHoldEvent()->RemoveAll(this);
		pController->GetJumpStopEvent()->RemoveAll(this);

		pController->GetSprintDoubleTapEvent()->RemoveAll(this);
		pController->GetSprintEvent()->RemoveAll(this);
		pController->GetSprintStopEvent()->RemoveAll(this);
	}
}

void UFlyState::Move(const FInputActionValue& value)
{
	// BaseMove
	const FVector2D movementVector{ value.Get<FVector2D>() };
	BaseMove(movementVector);
}
void UFlyState::StopMove()
{

}

void UFlyState::FlyUp()
{
	auto pCharacter{ GetCharacter() };

	if (pCharacter->Controller == nullptr) return;

	// Get upDirection
	const FVector upDirection{ 0.f, 0.f, 1.f };

	// Add movement 
	pCharacter->AddMovementInput(upDirection);
}
void UFlyState::StopFlyUp()
{

}

void UFlyState::FlyDown()
{
	auto pCharacter{ GetCharacter() };

	if (pCharacter->Controller == nullptr) return;

	// Get downDirection
	const FVector upDirection{ 0.f, 0.f, -1.f };

	// Add movement 
	pCharacter->AddMovementInput(upDirection);
}
void UFlyState::StopFlyDown()
{

}

void UFlyState::StopFly()
{
	// Change to idleState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}