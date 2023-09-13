// Fill out your copyright notice in the Description page of Project Settings.
#include "MoveState.h"
#include "../BasePlayerController.h"
#include "GameFramework/Controller.h"

UMoveState::UMoveState()
{
	StateDisplayName = "Move";
}

void UMoveState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Variables
	// ---------
	auto pPlayer{ GetCharacter() };

	pPlayer->MoveCharacter(pPlayer->GetLastMovementInput());

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UMoveState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &UMoveState::StopMove);

		pController->GetJumpEvent()->AddUObject(this, &UMoveState::Jump);
		//pController->GetDashEvent()->AddUObject(this, &UMoveState::Dash);
	}

	// Player
	pPlayer->GetNoMovementEvent()->AddUObject(this, &UMoveState::NoMovement);
}
void UMoveState::OnExit()
{
	// Unsubscribe from events
	// -----------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetMoveStopEvent()->RemoveAll(this);

		pController->GetJumpEvent()->RemoveAll(this);
		//pController->GetDashEvent()->RemoveAll(this);
	}

	// Player
	auto pPlayer{ GetCharacter() };
	pPlayer->GetNoMovementEvent()->RemoveAll(this);
}
void UMoveState::Tick(float deltaTime)
{
	//// Check if is falling
	//if (GetCharacter()->GetCharacterMovement()->IsFalling())
	//{
	//	// Change to jumpState
	//	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	//	pStateMachine->SwitchStateByKey({ "Jump" });
	//}
}

void UMoveState::Move(const FInputActionValue& value)
{
	const FVector2D input{ value.Get<FVector2D>() };

	auto pCharacter{ GetCharacter() };
	pCharacter->MoveCharacter(input);
}
void UMoveState::StopMove()
{
	auto pCharacter{ GetCharacter() };
	pCharacter->SetShouldMove(false);
}
void UMoveState::NoMovement()
{
	// Change to idleState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

void UMoveState::Jump()
{
	// Change to jumpState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Jump" });
}

//void UMoveState::Dash()
//{
//	//// Change to dashState
//	//auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
//	//pStateMachine->SwitchStateByKey({ "Dash" });
//}