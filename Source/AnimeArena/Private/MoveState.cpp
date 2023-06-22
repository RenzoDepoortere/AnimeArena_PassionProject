// Fill out your copyright notice in the Description page of Project Settings.
#include "MoveState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "../BaseCharacter.h"

#include <Kismet/GameplayStatics.h>

void UMoveState::OnEnter(AActor* pStateOwner)
{
	Super::OnEnter(pStateOwner);

	// Get CharacterPtr
	m_pCharacter = Cast<ACharacter>(pStateOwner);

	// Set animation

	// Subscribe to inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UMoveState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &UMoveState::StopMove);

		pController->GetSprintEvent()->AddUObject(this, &UMoveState::Sprint);
	}
}
void UMoveState::OnExit()
{
	// Unsubscribe from inputEvents
	auto pController{ Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0)) };
	if (pController)
	{
		pController->GetMoveEvent()->RemoveAll(this);
		pController->GetMoveStopEvent()->RemoveAll(this);

		pController->GetSprintEvent()->RemoveAll(this);
	}
}

void UMoveState::Move(const FInputActionValue& value)
{
	// Input is a Vector2D
	FVector2D movementVector = value.Get<FVector2D>();

	if (m_pCharacter->Controller != nullptr)
	{
		// Find out which way is forward
		const FRotator rotation = m_pCharacter->Controller->GetControlRotation();
		const FRotator yawRotation{ 0, rotation.Yaw, 0 };

		// Get forward vector
		const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

		// Get right vector 
		const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

		// Add movement 
		m_pCharacter->AddMovementInput(forwardDirection, movementVector.Y);
		m_pCharacter->AddMovementInput(rightDirection, movementVector.X);
	}
}
void UMoveState::StopMove()
{
	// Change to idleState
	auto pStateMachine{ m_pCharacter->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

void UMoveState::Sprint()
{
	// Change to sprintState
	auto pStateMachine{ m_pCharacter->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Sprint" });
}