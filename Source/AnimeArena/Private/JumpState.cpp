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

	// Jump
	GetCharacter()->Jump();

	// Set animation

	// Subscribe to inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UJumpState::Move);

		pController->GetJumpEvent()->AddUObject(this, &UJumpState::Jump);
		pController->GetJumpStopEvent()->AddUObject(this, &UJumpState::StopJump);
	}
}
void UJumpState::OnExit() 
{
	// Stop jumping
	GetCharacter()->StopJumping();

	// Unsubscribe from inputEvents
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);

		pController->GetJumpEvent()->RemoveAll(this);
		pController->GetJumpStopEvent()->RemoveAll(this);
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
	// Jump
	GetCharacter()->Jump();
}
void UJumpState::StopJump()
{
	// Stop jumping
	GetCharacter()->StopJumping();
}