// Fill out your copyright notice in the Description page of Project Settings.
#include "DashState.h"
#include "../BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateMachineComponent.h"
//#include "../BasePlayerController.h"

UDashState::UDashState()
	: m_CurrentTime{}
	, m_StartFriction{}
	, m_HasSetCooldown{ false }
{
	// Set name
	StateDisplayName = "Dash";

	// Set cooldownState
	SetHasCooldown(true);

	// Set invincible
	SetIsInvincible(true);
}

void UDashState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	auto pCharacter{ GetCharacter() };
	auto pCharacterMovement{ pCharacter->GetCharacterMovement() };

	// Cooldown
	// --------
	if (m_HasSetCooldown == false)
	{
		m_HasSetCooldown = true;
		SetMaxCooldown(pCharacter->DashCooldown);
	}

	// Reset variables
	// ---------------
	const float maxDashTime{ 0.5f };
	m_CurrentTime = maxDashTime;

	// Store and set friction
	// ----------------------
	m_StartFriction = pCharacterMovement->GroundFriction;
	pCharacterMovement->GroundFriction = 1.f;

	// Give boost
	// ----------

	// Find out which way is forward
	const FRotator rotation = GetCharacter()->Controller->GetControlRotation();
	const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	// Get direction
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// Get lastInput
	const FVector2D lastInput{ pCharacter->GetLastMovementInput() };

	// Calculate force
	FVector impulseForce = forwardDirection * lastInput.Y + rightDirection * lastInput.X;
	impulseForce *= pCharacter->DashBoost;

	// If not on ground
	if (pCharacterMovement->IsFalling())
	{
		// Remove velocity
		FVector velocity{ pCharacter->GetVelocity() };
		velocity = FVector::VectorPlaneProject(velocity, { 0, 0, 1 });
		impulseForce -= velocity;
	}

	// Add force
	pCharacterMovement->AddImpulse(impulseForce, true);

	// Set animation
	// -------------

	//// Subscribe to inputEvents
	//// ------------------------
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