// Fill out your copyright notice in the Description page of Project Settings.
#include "JumpState.h"
#include "../BasePlayerController.h"

UJumpState::UJumpState()
	: m_IsInput{}
	, m_CurrentJumpTime{}
{
	StateDisplayName = "Jump";
}

void UJumpState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Variables
	// ---------
	m_CurrentJumpTime = 0.f;

	auto pCharacter{ GetCharacter() };
	const bool wasInput{ !pCharacter->GetIsInAir() };
	m_IsInput = wasInput;

	// Check if pressed jump
	if (wasInput)
	{
		pCharacter->GetTotalVelocity().Z = pCharacter->JumpSpeed;
		pCharacter->SetShouldFall(false);
	}
	else
	{

	}

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UJumpState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &UJumpState::StopMove);

		pController->GetJumpEvent()->AddUObject(this, &UJumpState::Jump);
		pController->GetJumpStopEvent()->AddUObject(this, &UJumpState::StopJump);

		//pController->GetDashEvent()->AddUObject(this, &UJumpState::Dash);
	}

	// Player
	pCharacter->GetLandEvent()->AddUObject(this, &UJumpState::Landed);
}
void UJumpState::OnExit() 
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
		pController->GetJumpStopEvent()->RemoveAll(this);

		pController->GetDashEvent()->RemoveAll(this);
	}

	// Player
	auto pCharacter{ GetCharacter() };
	pCharacter->GetLandEvent()->RemoveAll(this);
}
void UJumpState::Tick(float deltaTime)
{
	HandleInput(deltaTime);
}

void UJumpState::Move(const FInputActionValue& value)
{
	const FVector2D input{ value.Get<FVector2D>() };

	auto pCharacter{ GetCharacter() };
	pCharacter->MoveCharacter(input, pCharacter->AirControl);
}
void UJumpState::StopMove()
{
	//auto pCharacter{ GetCharacter() };
	//pCharacter->SetShouldMove(false);
}

void UJumpState::Jump()
{
	// NOTE: This is for doubleJump and such
}
void UJumpState::StopJump()
{
	m_IsInput = false;
	auto pCharacter{ GetCharacter() };
	pCharacter->SetShouldFall(true);
}

void UJumpState::HandleInput(float deltaTime)
{
	// Keep going up with input
	if (m_IsInput)
	{
		auto pCharacter{ GetCharacter() };
		pCharacter->GetTotalVelocity().Z = pCharacter->JumpSpeed;

		// After some time, stop going up
		m_CurrentJumpTime += deltaTime;
		if (pCharacter->MaxJumpTime < m_CurrentJumpTime)
		{
			m_IsInput = false;
			pCharacter->SetShouldFall(true);
		}
	}
}
void UJumpState::Landed()
{
	// Fall
	auto pCharacter{ GetCharacter() };
	pCharacter->SetShouldFall(true);

	// Change to idleState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

//void UJumpState::Dash()
//{
//	//auto pCharacter{ GetCharacter() };
//
//	//// Check if used airDash before
//	//if (pCharacter->GetUsedAirDash() == false)
//	//{
//	//	// Set usedAirDash
//	//	pCharacter->SetUsedAirDash(true);
//
//	//	// Change to dashState
//	//	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
//	//	pStateMachine->SwitchStateByKey({ "Dash" });
//	//}
//}