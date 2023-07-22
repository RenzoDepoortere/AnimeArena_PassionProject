// Fill out your copyright notice in the Description page of Project Settings.
#include "Kamehameha_Ability.h"
#include "Goku_Character.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BasePlayerState.h"

UKamehameha_Ability::UKamehameha_Ability()
	: UBaseAbility()
	, m_pCharacter{ nullptr }
	, m_IsFiring{ false }
	, m_CurrentHoldTime{}
	, m_AnimationRunTime{}
	, m_MaxFlySpeed{}
{
}

void UKamehameha_Ability::Update(float deltaTime)
{
	UBaseAbility::Update(deltaTime);

	if (m_IsFiring) HandleKamehameha(deltaTime);
	else			HoldTimeCountdown(deltaTime);

	m_pCharacter->RotateTowardsCamera();
}

void UKamehameha_Ability::ActivateAbility()
{
	UBaseAbility::ActivateAbility();

	// Blacklist idleState
	m_pCharacter = Cast<AGoku_Character>(GetCharacter());
	m_pCharacter->StateMachineComponent->BlacklistKey("Idle");

	// Switch to flyState
	const FString flyString{ "AirOption" };
	m_pCharacter->StateMachineComponent->SwitchStateByKey(flyString);
	Cast<UBasePlayerState>(m_pCharacter->StateMachineComponent->StateMap[flyString])->GetExtraStateInfo()->canBeAttackCanceled = false;

	// Limit flySpeed
	auto pCharMovement{ m_pCharacter->GetCharacterMovement() };
	m_MaxFlySpeed = pCharMovement->MaxFlySpeed;
	pCharMovement->MaxFlySpeed = m_pCharacter->KamehamehaFlySpeed;

	// Set variables
	m_IsFiring = false;
	m_CurrentHoldTime = m_pCharacter->TimeToReachMaxKamehameha;

	// Limit rotation
	m_pCharacter->CameraRotationMultiplier = 0.25f;
	pCharMovement->bOrientRotationToMovement = false;

	// Start kamehameha animation
	auto pAnimInstance{ m_pCharacter->GetMesh()->GetAnimInstance() };
	pAnimInstance->Montage_Play(m_pCharacter->KamehamehaAnimation);
	pAnimInstance->Montage_Pause(m_pCharacter->KamehamehaAnimation);
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
	// Pause animation at last second
	if (0 < m_AnimationRunTime)
	{
		m_AnimationRunTime -= deltaTime;
		if (m_AnimationRunTime <= 0) m_pCharacter->GetMesh()->GetAnimInstance()->Montage_Pause(m_pCharacter->KamehamehaAnimation);
	}


}

void UKamehameha_Ability::StartBeam()
{
	m_IsFiring = true;

	// Start animation
	m_pCharacter->GetMesh()->GetAnimInstance()->Montage_Resume(m_pCharacter->KamehamehaAnimation);

	m_AnimationRunTime = m_pCharacter->KamehamehaAnimationStopTime;

	// Spawn beam
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "Could shoot beam");
}

void UKamehameha_Ability::AbilityEnd()
{
	// Remove idleState from blacklist
	m_pCharacter->StateMachineComponent->RemoveKeyFromBlacklist("Idle");

	// Switch to idle
	m_pCharacter->StateMachineComponent->SwitchStateByKey("Idle");
	Cast<UBasePlayerState>(m_pCharacter->StateMachineComponent->StateMap["AirOption"])->GetExtraStateInfo()->canBeAttackCanceled = true;

	// Reset flySpeed
	auto pCharMovement{ m_pCharacter->GetCharacterMovement() };
	pCharMovement->MaxFlySpeed = m_MaxFlySpeed;

	// Reset rotation
	m_pCharacter->CameraRotationMultiplier = 1.f;
	pCharMovement->bOrientRotationToMovement = true;

	FRotator newRotation{ m_pCharacter->GetActorRotation() };
	newRotation.Pitch = 0;
	newRotation.Roll = 0;
	m_pCharacter->SetActorRotation(newRotation);

	// Stop animation
	m_pCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, m_pCharacter->KamehamehaAnimation);

	// Set inactive
	SetIsActive(false);
}