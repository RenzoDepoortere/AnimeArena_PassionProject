// Fill out your copyright notice in the Description page of Project Settings.
#include "JumpState.h"
#include "StateMachineComponent.h"
#include "../BaseCharacter.h"
#include "../BasePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

UJumpState::UJumpState()
{
	StateDisplayName = "Jump";
}

void UJumpState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Check if isn't falling
	auto pCharacterMovement{ GetCharacter()->GetCharacterMovement() };
	if (pCharacterMovement->IsFalling() == false)
	{
		// Jump
		GetCharacter()->Jump();
	}

	// Disable rotation
	pCharacterMovement->bOrientRotationToMovement = false;

	// Set animation

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UJumpState::Move);

		pController->GetJumpEvent()->AddUObject(this, &UJumpState::Jump);
		pController->GetJumpStopEvent()->AddUObject(this, &UJumpState::StopJump);

		pController->GetDashEvent()->AddUObject(this, &UJumpState::Dash);
	}
}
void UJumpState::OnExit() 
{
	auto pCharacter{ GetCharacter() };

	// Stop jumping
	pCharacter->StopJumping();

	// Enable rotations
	pCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;

	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);

		pController->GetJumpEvent()->RemoveAll(this);
		pController->GetJumpStopEvent()->RemoveAll(this);

		pController->GetDashEvent()->RemoveAll(this);
	}
}
void UJumpState::Tick(float deltaTime)
{
	// If reached ground
	if (GetCharacter()->GetCharacterMovement()->IsFalling() == false)
	{
		// Change to idleState
		auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Idle" });
	}
}

void UJumpState::Move(const FInputActionValue& value)
{
	// BaseMove
	const FVector2D movementVector{ value.Get<FVector2D>() };
	BaseMove(movementVector);
}
void UJumpState::Jump()
{
	auto pCharacter{ GetCharacter() };

	// If has air option and didn't use before
	if (pCharacter->HasAirOption && pCharacter->GetUsedAirAbility() == false)
	{
		// Change to airOptionState

	}
}
void UJumpState::StopJump()
{
	// Stop jumping
	GetCharacter()->StopJumping();
}

void UJumpState::Dash()
{
	auto pCharacter{ GetCharacter() };

	// Check if used airDash before
	if (pCharacter->GetUsedAirDash() == false)
	{
		// Set usedAirDash
		pCharacter->SetUsedAirDash(true);

		// Change to dashState
		auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Dash" });
	}
}