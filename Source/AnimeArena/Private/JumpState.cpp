#include "JumpState.h"
#include "../BasePlayerController.h"
#include "KinematicController.h"

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

	auto pKinematicController{ GetKinematicController() };
	const bool wasInput{ !pKinematicController->GetIsInAir() };
	m_IsInput = wasInput;

	// Check if pressed jump
	if (wasInput)
	{
		pKinematicController->GetTotalVelocity().Z = pKinematicController->JumpSpeed;
		pKinematicController->SetShouldFall(false);
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
	pKinematicController->GetLandEvent()->AddUObject(this, &UJumpState::Landed);
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
	auto pKinematicController{ GetKinematicController() };
	pKinematicController->GetLandEvent()->RemoveAll(this);
}
void UJumpState::Tick(float deltaTime)
{
	HandleInput(deltaTime);
}

void UJumpState::Move(const FInputActionValue& value)
{
	const FVector2D input{ value.Get<FVector2D>() };

	auto pKinematicController{ GetKinematicController() };
	pKinematicController->MoveCharacter(input);
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
	auto pKinematicController{ GetKinematicController() };
	pKinematicController->SetShouldFall(true);
}

void UJumpState::HandleInput(float deltaTime)
{
	// Keep going up with input
	if (m_IsInput)
	{
		auto pKinematicController{ GetKinematicController() };
		pKinematicController->GetTotalVelocity().Z = pKinematicController->JumpSpeed;

		// After some time, stop going up
		m_CurrentJumpTime += deltaTime;
		if (pKinematicController->MaxJumpTime < m_CurrentJumpTime)
		{
			m_IsInput = false;
			pKinematicController->SetShouldFall(true);
		}
	}
}
void UJumpState::Landed()
{
	// Fall
	auto pKinematicController{ GetKinematicController() };
	pKinematicController->SetShouldFall(true);

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