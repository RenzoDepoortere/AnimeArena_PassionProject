// Fill out your copyright notice in the Description page of Project Settings.
#include "Kamehameha_Ability.h"
#include "Goku_Character.h"
#include "StateMachineComponent.h"

UKamehameha_Ability::UKamehameha_Ability()
	: UBaseAbility()
	, m_IsFiring{ false }
	, m_CurrentHoldTime{}
	, m_AnimationRunTime{}
{
}

void UKamehameha_Ability::Update(float deltaTime)
{
	UBaseAbility::Update(deltaTime);

	if (m_IsFiring) HandleKamehameha(deltaTime);
	else			HoldTimeCountdown(deltaTime);
}

void UKamehameha_Ability::ActivateAbility()
{
	UBaseAbility::ActivateAbility();

	// Blacklist attackState
	auto pCharacter{ Cast<AGoku_Character>(GetCharacter()) };
	pCharacter->StateMachineComponent->BlacklistKey("Attack");

	// Set variables
	m_IsFiring = false;
	m_CurrentHoldTime = pCharacter->TimeToReachMaxKamehameha;

	// Start kamehameha animation
	pCharacter->GetMesh()->GetAnimInstance()->Montage_Play(pCharacter->KamehamehaAnimation, 0.f);
}
void UKamehameha_Ability::StopAbility()
{
	// If is not firing
	if (m_IsFiring == false) StartBeam();
}

void UKamehameha_Ability::HoldTimeCountdown(float deltaTime)
{
	if (0 < m_CurrentHoldTime)
	{
		m_CurrentHoldTime -= deltaTime;
		if (m_CurrentHoldTime <= 0) StartBeam();
	}
}
void UKamehameha_Ability::HandleKamehameha(float deltaTime)
{
	auto pCharacter{ Cast<AGoku_Character>(GetCharacter()) };

	// Pause animation at last second
	if (0 < m_AnimationRunTime)
	{
		m_AnimationRunTime -= deltaTime;
		if (m_AnimationRunTime <= 0) pCharacter->GetMesh()->GetAnimInstance()->Montage_Pause(pCharacter->KamehamehaAnimation);
	}


}

void UKamehameha_Ability::StartBeam()
{
	m_IsFiring = true;

	// Start animation
	auto pCharacter{ Cast<AGoku_Character>(GetCharacter()) };
	pCharacter->GetMesh()->GetAnimInstance()->Montage_SetPlayRate(pCharacter->KamehamehaAnimation, 1.f);

	m_AnimationRunTime = pCharacter->KamehamehaAnimationStopTime;

	// Spawn beam
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "Could shoot beam");
}

void UKamehameha_Ability::AbilityEnd()
{
	// Remove attackState from blacklist
	auto pCharacter{ Cast<AGoku_Character>(GetCharacter()) };
	pCharacter->StateMachineComponent->RemoveKeyFromBlacklist("Attack");

	// Stop animation
	pCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, pCharacter->KamehamehaAnimation);

	// Set inactive
	SetIsActive(false);
}