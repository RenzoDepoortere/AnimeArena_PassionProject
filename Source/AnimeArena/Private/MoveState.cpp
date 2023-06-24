// Fill out your copyright notice in the Description page of Project Settings.
#include "MoveState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "../BaseCharacter.h"

UMoveState::UMoveState()
{
	StateDisplayName = "Move";
}

void UMoveState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Set animation

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UMoveState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &UMoveState::StopMove);

		pController->GetSprintEvent()->AddUObject(this, &UMoveState::Sprint);

		pController->GetJumpEvent()->AddUObject(this, &UMoveState::Jump);
	}
}
void UMoveState::OnExit()
{
	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetMoveStopEvent()->RemoveAll(this);

		pController->GetSprintEvent()->RemoveAll(this);

		pController->GetJumpEvent()->RemoveAll(this);
	}
}

void UMoveState::Move(const FInputActionValue& value)
{
	// BaseMove
	const FVector2D movementVector{ value.Get<FVector2D>() };
	BaseMove(movementVector);
}
void UMoveState::StopMove()
{
	// Change to idleState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>()};
	pStateMachine->SwitchStateByKey({ "Idle" });
}

void UMoveState::Sprint()
{
	// Change to sprintState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Sprint" });
}

void UMoveState::Jump()
{
	// Change to jumpState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Jump" });
}