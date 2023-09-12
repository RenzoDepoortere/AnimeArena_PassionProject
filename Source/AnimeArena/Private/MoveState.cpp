// Fill out your copyright notice in the Description page of Project Settings.
#include "MoveState.h"
#include "../BasePlayerController.h"
#include "GameFramework/Controller.h"

UMoveState::UMoveState()
	: m_IsInput{}
	, m_LastInput{}
	, m_CurrentDirection{}
	, m_MoveSpeed{}
	, m_DesiredRotation{}
{
	StateDisplayName = "Move";
}

void UMoveState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Variables
	// ---------
	auto pPlayer{ GetCharacter() };

	m_IsInput = true;
	m_LastInput = pPlayer->GetLastMovementInput();

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetMoveEvent()->AddUObject(this, &UMoveState::Move);
		pController->GetMoveStopEvent()->AddUObject(this, &UMoveState::StopMove);

		pController->GetJumpEvent()->AddUObject(this, &UMoveState::Jump);
		//pController->GetDashEvent()->AddUObject(this, &UMoveState::Dash);
	}
}
void UMoveState::OnExit()
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
		//pController->GetDashEvent()->RemoveAll(this);
	}
}
void UMoveState::Tick(float deltaTime)
{
	HandleInput(deltaTime);

	//// Check if is falling
	//if (GetCharacter()->GetCharacterMovement()->IsFalling())
	//{
	//	// Change to jumpState
	//	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	//	pStateMachine->SwitchStateByKey({ "Jump" });
	//}
}

void UMoveState::Move(const FInputActionValue& value)
{
	m_IsInput = true;
	m_LastInput = value.Get<FVector2D>();
}

void UMoveState::Jump()
{
	// Change to jumpState
	auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Jump" });
}

void UMoveState::HandleInput(float deltaTime)
{
	auto pPlayer{ GetCharacter() };
	auto pController{ GetPlayerController() };

	// Get Directions
	// --------------

	// Get rotations
	const FRotator rotation = pController->GetControlRotation();
	const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	// Get directions
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// Calculate movement
	// ------------------
	const float moveAcceleration{ pPlayer->MaxMovementSpeed / pPlayer->MoveAccelerationTime };

	// If input
	if (m_IsInput)
	{
		// Calculate currentDirection
		m_CurrentDirection = forwardDirection * m_LastInput.Y + rightDirection * m_LastInput.X;
		m_CurrentDirection.Normalize();

		// New rotation
		m_DesiredRotation = FRotationMatrix::MakeFromX(m_CurrentDirection).Rotator();

		// Speed up
		m_MoveSpeed += moveAcceleration * deltaTime;
		if (pPlayer->MaxMovementSpeed < m_MoveSpeed) m_MoveSpeed = pPlayer->MaxMovementSpeed;
	}
	// No input
	else
	{
		// Slow down
		m_MoveSpeed -= moveAcceleration * deltaTime;
		if (m_MoveSpeed < 0)
		{
			m_MoveSpeed = 0;

			// Change to idleState
			auto pStateMachine{ pPlayer->GetComponentByClass<UStateMachineComponent>() };
			pStateMachine->SwitchStateByKey({ "Idle" });
		}
	}

	// Set total veloctity
	// -------------------
	const FVector horizontalVelocity{ m_CurrentDirection * m_MoveSpeed };

	FVector& totalVelocity{ pPlayer->GetTotalVelocity() };
	totalVelocity.X = horizontalVelocity.X;
	totalVelocity.Y = horizontalVelocity.Y;

	// Set rotation
	// ------------
	const FRotator currentRotation{ pPlayer->GetActorRotation() };
	const float rotationSpeed{ pPlayer->RotationSpeed * deltaTime };

	const FRotator newRotation{ FMath::Lerp(currentRotation, m_DesiredRotation, rotationSpeed) };
	pPlayer->SetActorRotation(newRotation);
}

//void UMoveState::Dash()
//{
//	//// Change to dashState
//	//auto pStateMachine{ GetCharacter()->GetComponentByClass<UStateMachineComponent>() };
//	//pStateMachine->SwitchStateByKey({ "Dash" });
//}