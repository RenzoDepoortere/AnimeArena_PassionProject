#include "DashState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "StateMachineComponent.h"
#include "../BasePlayerController.h"
#include "KinematicController.h"

//// Calculate dashSpeed
//const bool isPerfectTimed{ -DashGraceTime <= m_CurrentDashTime };
//const float dashSpeed = isPerfectTimed ? DashSpeed * DashPerfectMultiplier : DashSpeed;

//// Debug
//if (isPerfectTimed) GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Purple, "PERFECT!");

UDashState::UDashState()
	: m_HasSetCooldown{ false }
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

	// Change kinematicControllers
	// ---------------------------

	// Force
	const FSpeedLevel currentSpeedLevel{ pCharacter->SpeedLevels[pCharacter->GetCurrentSpeedLevel()] };
	const float dashSpeed{ currentSpeedLevel.SpeedLimit * pCharacter->DashMultiplier };
	const FVector2D movementInput{ pCharacter->GetLastMovementInput() };
	const FVector dashDirection{ pCharacter->KinematicController->ConvertInputToWorld(movementInput) };

	pCharacter->KinematicController->AddForce(dashDirection * dashSpeed, true);

	// Set rotations
	pCharacter->KinematicController->UseForwardVector = false;
	pCharacter->KinematicController->RotateCharacter(movementInput);
	pCharacter->KinematicController->RotationSpeed = pCharacter->DashRotationSpeed;

	// Set timerHandle
	// ---------------

	// Set variables
	FTimerHandle timerHandle{};
	FTimerDelegate timerDelegate{};
	timerDelegate.BindLambda([&]()
	{
		// Change to idleState
		auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Idle" });
	});

	// Set timer
	GetWorld()->GetTimerManager().SetTimer(timerHandle, timerDelegate, pCharacter->DashTime, false);
}
void UDashState::OnExit()
{
	// Reset rotations
	auto pCharacter{ GetCharacter() };
	const FSpeedLevel currentSpeedLevel{ pCharacter->SpeedLevels[pCharacter->GetCurrentSpeedLevel()] };

	pCharacter->KinematicController->UseForwardVector = true;
	pCharacter->KinematicController->RotationSpeed = currentSpeedLevel.RotationSpeed;
}