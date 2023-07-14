// Fill out your copyright notice in the Description page of Project Settings.
#include "AttackState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "Animation/AnimInstance.h"

UAttackState::UAttackState()
	: m_CurrentAttackString{}
	, m_PossibleAttackStrings{}
	, m_CurrentAttack{}
{
	StateDisplayName = "Attack";
}

void UAttackState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// AttackString
	// ------------

	// Set currentAttackState to start
	auto pCharacter{ GetCharacter() };
	pCharacter->SetAttackState(EAttackEnum::start);

	m_CurrentAttack = 0;

	// Check data
	const FVector2D lastMovementInput{ pCharacter->GetLastMovementInput() };
	const bool wasLightAttack{ pCharacter->GetLastAttackInput() };

	// Make currentAttackString
	m_CurrentAttackString = ConvertInputToString(lastMovementInput);
	m_CurrentAttackString += (wasLightAttack) ? "X " : "Y ";

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, m_CurrentAttackString);

	// Loop through attacks
	TArray<FAttackString> possibleAttacks{};
	for (const auto& currentAttack : pCharacter->Attacks)
	{
		// Check if contains attackString
		if (currentAttack.stringPattern.Contains(m_CurrentAttackString))
		{
			possibleAttacks.Add(currentAttack);
		}
	}

	// Store attackStrings
	m_PossibleAttackStrings = possibleAttacks;
	if (m_PossibleAttackStrings.Num() == 0)
	{
		// Change to idleState
		auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Idle" });
		return;
	}


	// Play animation
	auto pAnimationMontage{ m_PossibleAttackStrings[0].attacks[0].attackAnimationMontage };
	pCharacter->GetMesh()->GetAnimInstance()->Montage_Play(pAnimationMontage);

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetLightAttackEvent()->AddUObject(this, &UAttackState::AttackInput);
		pController->GetHeavyAttackEvent()->AddUObject(this, &UAttackState::AttackInput);
	}

	// Attack
	pCharacter->GetAttackEndEvent()->AddUObject(this, &UAttackState::AttackEnded);
}
void UAttackState::OnExit()
{
	// Unsubscribe from events
	// ----------------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetLightAttackEvent()->RemoveAll(this);
		pController->GetHeavyAttackEvent()->RemoveAll(this);
	}

	// Attack
	auto pCharacter{ GetCharacter() };
	pCharacter->GetAttackEndEvent()->RemoveAll(this);
}

void UAttackState::AttackInput()
{
	// Check if in activeState
	auto pCharacter{ GetCharacter() };
	if (pCharacter->CurrentAttackState != EAttackEnum::active) return;

	// Set currentAttackState to start
	pCharacter->SetAttackState(EAttackEnum::start);

	// Check data
	const FVector2D lastMovementInput{ pCharacter->GetLastMovementInput() };
	const bool wasLightAttack{ pCharacter->GetLastAttackInput() };

	// Make currentAttackString
	m_CurrentAttackString += ConvertInputToString(lastMovementInput);
	m_CurrentAttackString += (wasLightAttack) ? "X " : "Y ";

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, m_CurrentAttackString);

	// Check if attackString is in possibilities
	TArray<FAttackString> possibleAttacks{};
	for (const auto& currentAttack : m_PossibleAttackStrings)
	{
		// Check if contains attackString
		if (currentAttack.stringPattern.Contains(m_CurrentAttackString))
		{
			possibleAttacks.Add(currentAttack);
		}
	}

	// Store attackStrings
	m_PossibleAttackStrings = possibleAttacks;
	if (m_PossibleAttackStrings.Num() == 0)
	{
		// Do nothing
		return;
	}

	// Set currentAttackState to start
	pCharacter->SetAttackState(EAttackEnum::start);

	// Update index
	++m_CurrentAttack;

	// Play animation
	auto pAnimationMontage{ m_PossibleAttackStrings[0].attacks[m_CurrentAttack].attackAnimationMontage };
	pCharacter->GetMesh()->GetAnimInstance()->Montage_Play(pAnimationMontage);
}
void UAttackState::AttackEnded()
{
	// Change to idleState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

FString UAttackState::ConvertInputToString(FVector2D movementInput) const
{
	const float absX{ static_cast<float>(abs(movementInput.X)) };
	const float absY{ static_cast<float>(abs(movementInput.Y)) };

	// Check if there was input
	const bool wasNoInput{ absX <= 0.25f && absY <= 0.25f };
	if (wasNoInput) return "N";

	// Check which axis dominates
	movementInput.Normalize();
	const bool xIsBigger{ absY < absX };

	if (xIsBigger)
	{
		// Check for sign
		if (movementInput.X < 0) return "L";
		else					 return "R";
	}
	else
	{
		// Check for sign
		if (movementInput.Y < 0) return "D";
		else					 return "U";
	}
}