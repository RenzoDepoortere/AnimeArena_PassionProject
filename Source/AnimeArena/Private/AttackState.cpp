// Fill out your copyright notice in the Description page of Project Settings.
#include "AttackState.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"

UAttackState::UAttackState()
	: m_CurrentAttackString{}
	, m_PossibleAttackStrings{}
	, m_CurrentAttack{}
	, m_OriginalFriction{}
{
	StateDisplayName = "Attack";
}

void UAttackState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Store and change friction
	auto pCharacter{ GetCharacter() };
	auto pCharMovement{ pCharacter->GetCharacterMovement() };
	m_OriginalFriction = pCharMovement->GroundFriction;
	pCharMovement->GroundFriction = 0.f;

	// AttackString
	// ------------

	// Set currentAttackState to start
	pCharacter->SetAttackState(EAttackEnum::start);

	m_CurrentAttack = 0;

	// Check data
	const FVector2D lastMovementInput{ pCharacter->GetLastMovementInput() };
	const bool wasLightAttack{ pCharacter->GetLastAttackInput() };
	const FString attackLetter{ (wasLightAttack) ? "X " : "Y " };

	// Make currentAttackString
	m_CurrentAttackString = {};
	m_CurrentAttackString += ConvertInputToString(lastMovementInput, attackLetter);
	m_CurrentAttackString += attackLetter;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, m_CurrentAttackString);

	// Loop through attacks
	TArray<FAttackString> possibleAttacks{};
	const bool isInAir{ pCharMovement->IsFalling() };
	for (const auto& currentAttack : pCharacter->Attacks)
	{
		// If doesn't contain attackString, continue
		if (currentAttack.stringPattern.Find(m_CurrentAttackString) != 0) continue;

		// If groundBased and in air, continue
		if (isInAir && currentAttack.groundBased) continue;

		// If not groundBased and not in air, continue
		if (isInAir == false && currentAttack.groundBased == false) continue;

		// If ultOnly and not in ult, continue

		// Add to possibleAttack
		possibleAttacks.Add(currentAttack);
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

	// Faced lockedChar if lockedOn
	if (pCharacter->GetIsLocked()) pCharacter->FaceLockedCharacter();

	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetLightAttackEvent()->AddUObject(this, &UAttackState::LightAttack);
		pController->GetHeavyAttackEvent()->AddUObject(this, &UAttackState::HeavyAttack);
	}

	// Attack
	pCharacter->GetAttackEndEvent()->AddUObject(this, &UAttackState::AttackEnded);
}
void UAttackState::OnExit()
{
	// Reset friction
	auto pCharacter{ GetCharacter() };
	auto pCharMovement{ pCharacter->GetCharacterMovement() };
	pCharMovement->GroundFriction = m_OriginalFriction;

	// Unsubscribe from events
	// -----------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetLightAttackEvent()->RemoveAll(this);
		pController->GetHeavyAttackEvent()->RemoveAll(this);
	}

	// Attack
	pCharacter->GetAttackEndEvent()->RemoveAll(this);
}

void UAttackState::AttackInput(const FString& attackLetter)
{
	// Check if in activeState
	auto pCharacter{ GetCharacter() };
	if (pCharacter->CurrentAttackState != EAttackEnum::active) return;

	// Set currentAttackState to start
	pCharacter->SetAttackState(EAttackEnum::start);

	// Check data
	const FVector2D lastMovementInput{ pCharacter->GetLastMovementInput() };

	// Make currentAttackString
	m_CurrentAttackString += ConvertInputToString(lastMovementInput, attackLetter);
	m_CurrentAttackString += attackLetter;

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, m_CurrentAttackString);

	// Check if attackString is in possibilities
	TArray<FAttackString> possibleAttacks{};
	for (const auto& currentAttack : m_PossibleAttackStrings)
	{
		// Check if contains attackString
		if (currentAttack.stringPattern.Find(m_CurrentAttackString) == 0)
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

	// Faced lockedChar if lockedOn
	if (pCharacter->GetIsLocked()) pCharacter->FaceLockedCharacter();
}
void UAttackState::AttackEnded()
{
	// Change to idleState
	auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
	pStateMachine->SwitchStateByKey({ "Idle" });
}

FString UAttackState::ConvertInputToString(FVector2D movementInput, const FString& attackLetter) const
{
	const float absX{ static_cast<float>(abs(movementInput.X)) };
	const float absY{ static_cast<float>(abs(movementInput.Y)) };
	FString directionString{};

	// Check if there was input
	const bool wasNoInput{ absX <= 0.25f && absY <= 0.25f };
	if (wasNoInput) directionString = "N";
	else
	{
		// Check which axis dominates
		movementInput.Normalize();
		const bool xIsBigger{ absY < absX };

		if (xIsBigger)
		{
			// Check for sign
			if (movementInput.X < 0) directionString = "L";
			else					 directionString = "R";
		}
		else
		{
			// Check for sign
			if (movementInput.Y < 0) directionString = "D";
			else					 directionString = "U";
		}
	}

	// Check if contains such directionAttack
	auto pCharacter{ GetCharacter() };
	FString wouldBeString{ m_CurrentAttackString };
	wouldBeString += directionString + attackLetter;

	for (const auto& currentAttack : pCharacter->Attacks)
	{
		if (currentAttack.stringPattern.Find(wouldBeString) == 0)
		{
			return directionString;
		}
	}

	// Change directionString to neutral
	directionString = "N";
	return directionString;
}