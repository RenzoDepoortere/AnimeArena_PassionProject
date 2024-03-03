#include "DashState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateMachineComponent.h"
#include "../BasePlayerController.h"
#include "KinematicController.h"

UDashState::UDashState()
	: m_HasSetCooldown{ false }
	, m_TimerHandle{}
{
	// Set name
	StateDisplayName = "Dash";

	// Set cooldownState
	SetHasCooldown(true);
}

void UDashState::OnEnter(AActor* pStateOwner) 
{
	UBasePlayerState::OnEnter(pStateOwner);

	auto pCharacter{ GetCharacter() };

	// Cooldown
	// --------
	if (m_HasSetCooldown == false)
	{
		m_HasSetCooldown = true;
		SetMaxCooldown(pCharacter->DashCooldown);
	}

	// Unlock second speedLevel
	// ------------------------
	pCharacter->SpeedLevels[1].IsLocked = false;

	// Change kinematicControllers
	// ---------------------------

	// Check input
	const FVector2D movementInput{ pCharacter->GetLastMovementInput() };
	if (!movementInput.Equals(FVector2D::ZeroVector))
	{
		const FSpeedLevel currentSpeedLevel{ pCharacter->SpeedLevels[pCharacter->GetCurrentSpeedLevel()] };
		const float dashSpeed{ currentSpeedLevel.Speed * pCharacter->DashMultiplier };
		const FVector dashDirection{ pCharacter->KinematicController->ConvertInputToWorld(movementInput) };

		pCharacter->KinematicController->AddImpulse(dashDirection * dashSpeed, true);
		pCharacter->KinematicController->RotateCharacter(movementInput, true);
		pCharacter->KinematicController->SetShouldFall(false);
	}

	pCharacter->KinematicController->SetKeepMomentum(true);

	// Set timerHandle
	// ---------------

	// Set variables
	FTimerDelegate timerDelegate{};
	timerDelegate.BindLambda([&]()
	{
		// Change to idleState
		auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey(pStateMachine->PreviousState);
	});

	// Set timer
	GetWorld()->GetTimerManager().SetTimer(m_TimerHandle, timerDelegate, pCharacter->DashTime, false);

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetJumpEvent()->AddUObject(this, &UDashState::Jump);
	}
}
void UDashState::OnExit()
{
	// Reset kinematicController
	// -------------------------

	auto pCharacter{ GetCharacter() };
	pCharacter->KinematicController->SetShouldFall(true);
	pCharacter->KinematicController->SetKeepMomentum(false);


	// Unsubscribe from events
	// -----------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetJumpEvent()->RemoveAll(this);
	}
}

void UDashState::Jump()
{
	// Change to jumpState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Jump" });

	// Stop timer
	GetWorld()->GetTimerManager().ClearTimer(m_TimerHandle);
}