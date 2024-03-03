#include "JumpState.h"
#include "../BasePlayerController.h"
#include "KinematicController.h"

UJumpState::UJumpState()
	: m_HasStoppedFall{ false }
	, m_CurrentJumpTime{}
	, m_UsedForward{}
{
	StateDisplayName = "Jump";
}

void UJumpState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Variables
	// ---------
	m_CurrentJumpTime = 0.f;
	m_HasStoppedFall = false;

	auto pKinematicController{ GetKinematicController() };
	const bool wasInput{ !pKinematicController->GetIsInAir() };

	// Kinematic controller
	// --------------------

	// Check if pressed jump
	if (wasInput)
	{
		pKinematicController->AddImpulse(FVector{ 0.f, 0.f, pKinematicController->JumpSpeed });
		pKinematicController->SetShouldFall(false);
	}

	m_UsedForward = pKinematicController->UseForwardVector;
	pKinematicController->UseForwardVector = true;
	pKinematicController->SetCheckGround(false);

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetJumpEvent()->AddUObject(this, &UJumpState::Jump);
		pController->GetJumpStopEvent()->AddUObject(this, &UJumpState::StopJump);

		pController->GetDashEvent()->AddUObject(this, &UJumpState::Dash);
	}

	// Player
	pKinematicController->GetLandEvent()->AddUObject(this, &UJumpState::Landed);
}
void UJumpState::OnExit() 
{
	// Kinematic controller
	// --------------------

	// Fall
	auto pKinematicController{ GetKinematicController() };
	pKinematicController->SetShouldFall(true);
	pKinematicController->SetCheckGround(true);

	pKinematicController->UseForwardVector = m_UsedForward;

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
	pKinematicController->GetLandEvent()->RemoveAll(this);
}
void UJumpState::Tick(float deltaTime)
{
	m_CurrentJumpTime += deltaTime;
	HandleJumpTime(deltaTime);
}

void UJumpState::Jump()
{
	// NOTE: This is for doubleJump and such
}
void UJumpState::StopJump()
{
	// NOTE: This for charge jump
}

void UJumpState::HandleJumpTime(float deltaTime)
{
	if (m_HasStoppedFall) return;

	// Jump buffer
	auto pKinematicController{ GetKinematicController() };
	if (pKinematicController->JumpGroundBufferTime <= m_CurrentJumpTime)
	{
		pKinematicController->SetCheckGround(true);
	}
	
	// Fall down after time
	if (pKinematicController->MaxJumpTime <= m_CurrentJumpTime)
	{
		m_HasStoppedFall = true;
		pKinematicController->SetShouldFall(true);
	}
}

void UJumpState::Landed()
{
	// Change to idleState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

void UJumpState::Dash()
{
	// Change to dashState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Dash" });
}