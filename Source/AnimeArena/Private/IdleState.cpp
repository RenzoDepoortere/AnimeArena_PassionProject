// Fill out your copyright notice in the Description page of Project Settings.
#include "IdleState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include <Kismet/GameplayStatics.h>

void UIdleState::OnEnter(AActor* pStateOwner)
{
	Super::OnEnter(pStateOwner);

	// Set animation

	// Subscribe to inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UIdleState::Move);
	}
}
void UIdleState::OnExit()
{
	// Unsubscribe from inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
	}
}

void UIdleState::Move(const FInputActionValue& /*value*/)
{
	// Change to moveState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Move" });
}