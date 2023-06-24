// Fill out your copyright notice in the Description page of Project Settings.
#include "IdleState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "../BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UIdleState::UIdleState()
{
	StateDisplayName = "Idle";
}

void UIdleState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	auto pCharacter{ GetCharacter() };

	// If on ground
	if (pCharacter->GetCharacterMovement()->IsFalling() == false)
	{
		// Reset jumpVariables
		pCharacter->SetUsedAirAbility(false);
		pCharacter->SetUsedAirDash(false);
	}

	// Set animation

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UIdleState::Move);
		pController->GetJumpEvent()->AddUObject(this, &UIdleState::Jump);
		pController->GetDashEvent()->AddUObject(this, &UIdleState::Dash);
	}
}
void UIdleState::OnExit()
{
	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetJumpEvent()->RemoveAll(this);
		pController->GetDashEvent()->RemoveAll(this);
	}
}
void UIdleState::Tick(float /*deltaTime*/)
{
	// Check if is falling
	if (GetCharacter()->GetCharacterMovement()->IsFalling())
	{
		// Change to jumpState
		auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Jump" });
	}
}

void UIdleState::Move(const FInputActionValue& /*value*/)
{
	// Change to moveState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Move" });
}
void UIdleState::Jump()
{
	// Change to jumpState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Jump" });
}
void UIdleState::Dash()
{
	// Change to dashState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Dash" });
}