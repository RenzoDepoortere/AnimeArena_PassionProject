// Fill out your copyright notice in the Description page of Project Settings.
#include "DashState.h"
#include "../BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateMachineComponent.h"
//#include "../BasePlayerController.h"

UDashState::UDashState()
	: m_CurrentTime{}
	, m_StartFriction{}
	, m_WasInput{ true }
	, m_HasSetCooldown{ false }
{
	// Set name
	StateDisplayName = "Dash";

	// Set cooldownState
	SetHasCooldown(true);

	// Set invincible
	GetExtraStateInfoPointer()->isInvincible = true;
}

void UDashState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	auto pCharacter{ GetCharacter() };
	auto pCharacterMovement{ pCharacter->GetCharacterMovement() };

	// Set gravity to 0
	pCharacterMovement->GravityScale = 0;

	// Cooldown
	// --------
	if (m_HasSetCooldown == false)
	{
		m_HasSetCooldown = true;
		SetMaxCooldown(pCharacter->DashCooldown);
	}

	// Reset variables
	// ---------------
	const float maxDashTime{ 0.3f };
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

	// Check if was input
	const bool negligibleXValue{ -0.2f <= lastInput.X && lastInput.X <= 0.2f };
	const bool negligibleYValue{ -0.2f <= lastInput.Y && lastInput.Y <= 0.2f };

	if (negligibleXValue && negligibleYValue) m_WasInput = false;
	else									  m_WasInput = true;
}
void UDashState::OnExit()
{
	auto pCharacterMovement{ GetCharacter()->GetCharacterMovement() };

	// Reset friction
	pCharacterMovement->GroundFriction = m_StartFriction;

	// Reset gravity
	pCharacterMovement->GravityScale = 1;
}
void UDashState::Tick(float deltaTime)
{
	// If was no input
	if (m_WasInput == false)
	{
		// Remove velocity
		GetCharacter()->GetVelocity().Set(0.f, 0.f, 0.f);
	}

	// Cooldown
	m_CurrentTime -= deltaTime;
	if (m_CurrentTime < 0)
	{
		// Change to idleState
		auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Idle" });
	}
}