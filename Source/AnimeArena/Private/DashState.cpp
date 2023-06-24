// Fill out your copyright notice in the Description page of Project Settings.
#include "DashState.h"
#include "../BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateMachineComponent.h"
//#include "../BasePlayerController.h"

UDashState::UDashState()
{
	// Set name
	StateDisplayName = "Dash";

	// Set cooldownState
	SetHasCooldown(true);

	const float maxDashCooldown{ 1.f };
	SetMaxCooldown(1.f);
}

void UDashState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	auto pCharacter{ GetCharacter() };
	auto pCharacterMovement{ pCharacter->GetCharacterMovement() };

	// Reset variables
	const float maxDashTime{ 0.5f };
	m_CurrentTime = maxDashTime;

	// Store and set friction
	m_StartFriction = pCharacterMovement->GroundFriction;
	pCharacterMovement->GroundFriction = 1.f;

	// Give boost
	FVector impulseForce{ pCharacter->GetVelocity() };
	impulseForce.Normalize();
	impulseForce *= pCharacter->DashBoost;

	pCharacterMovement->AddImpulse(impulseForce, true);

	// Set animation

	//// Subscribe to inputEvents
	//auto pController{ GetPlayerController() };
	//if (pController)
	//{

	//}
}
void UDashState::OnExit()
{
	// Reset frictino
	GetCharacter()->GetCharacterMovement()->GroundFriction = m_StartFriction;

	//// Unsubscribe from inputEvents
	//auto pController{ GetPlayerController() };
	//if (pController)
	//{

	//}
}
void UDashState::Tick(float deltaTime)
{
	m_CurrentTime -= deltaTime;
	if (m_CurrentTime < 0)
	{
		// Change to idleState
		auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Idle" });
	}
}