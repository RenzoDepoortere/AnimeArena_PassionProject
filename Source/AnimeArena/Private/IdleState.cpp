// Fill out your copyright notice in the Description page of Project Settings.
#include "IdleState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include <Kismet/GameplayStatics.h>

UIdleState::UIdleState()
{
	StateDisplayName = "Idle";
}

void UIdleState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Set animation

	// Subscribe to inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
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
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetJumpEvent()->RemoveAll(this);
		pController->GetDashEvent()->RemoveAll(this);
	}
}
void UIdleState::Tick(float deltaTime)
{
	// Check is falling

	// Change to jumpState
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