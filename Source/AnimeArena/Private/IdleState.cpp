#include "IdleState.h"
#include "../BasePlayerController.h"
#include "KinematicController.h"

UIdleState::UIdleState()
{
	StateDisplayName = "Idle";
}

void UIdleState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Variables
	// ---------
	auto pKinematicController{ GetKinematicController() };
	pKinematicController->SetShouldMove(false);

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UIdleState::Move);
		pController->GetJumpEvent()->AddUObject(this, &UIdleState::Jump);
		pController->GetDashEvent()->AddUObject(this, &UIdleState::Dash);
	}
}
void UIdleState::OnExit()
{
	// Unsubscribe from events
	// -----------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetJumpEvent()->RemoveAll(this);
		pController->GetDashEvent()->RemoveAll(this);
	}
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