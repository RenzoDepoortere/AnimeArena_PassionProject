// Fill out your copyright notice in the Description page of Project Settings.
#include "IdleState.h"
#include "../BasePlayerController.h"

UIdleState::UIdleState()
{
	StateDisplayName = "Idle";
}

void UIdleState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UIdleState::Move);
		//pController->GetJumpEvent()->AddUObject(this, &UIdleState::Jump);
		//pController->GetDashEvent()->AddUObject(this, &UIdleState::Dash);
	}
}
void UIdleState::OnExit()
{
	// Unsubscribe from events
	// -----------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		//pController->GetJumpEvent()->RemoveAll(this);
		//pController->GetDashEvent()->RemoveAll(this);
	}
}
void UIdleState::Tick(float /*deltaTime*/)
{
	//// Return if can't be controlled
	//auto pCharacter{ GetCharacter() };
	//if (pCharacter->CanBeControlled == false) return;

	//// Check if is falling
	//if (pCharacter->GetCharacterMovement()->IsFalling())
	//{
	//	// Change to jumpState
	//	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	//	pStateMachine->SwitchStateByKey({ "Jump" });
	//}
}

void UIdleState::Move(const FInputActionValue& /*value*/)
{
	// Change to moveState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Move" });
}
//void UIdleState::Jump()
//{
//	// Change to jumpState
//	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
//	pStateMachine->SwitchStateByKey({ "Jump" });
//}
//void UIdleState::Dash()
//{
//	// Change to dashState
//	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
//	pStateMachine->SwitchStateByKey({ "Dash" });
//}