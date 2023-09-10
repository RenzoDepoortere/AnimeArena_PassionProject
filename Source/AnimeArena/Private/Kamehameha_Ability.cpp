// Fill out your copyright notice in the Description page of Project Settings.
#include "Kamehameha_Ability.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BasePlayerState.h"
#include "BaseBeam.h"
//#include "../BaseCharacter.h"

UKamehameha_Ability::UKamehameha_Ability()
	: UBaseAbility()
	, m_pCharacter{ nullptr }
	, m_pBeam{ nullptr }
	, m_IsFiring{ false }
	, m_CurrentHoldTime{}
	, m_AnimationRunTime{}
	, m_MaxFlySpeed{}
{
}

void UKamehameha_Ability::Update(float deltaTime)
{
	if (m_IsFiring) HandleKamehameha(deltaTime);
	else			HoldTimeCountdown(deltaTime);

	//m_pCharacter->RotateTowardsCamera();
}

void UKamehameha_Ability::ActivateAbility()
{
	UBaseAbility::ActivateAbility();

	//// Blacklist idleState
	//m_pCharacter = GetCharacter();
	//m_pCharacter->StateMachineComponent->BlacklistKey("Idle");

	//// Switch to flyState
	//const FString flyString{ "AirOption" };
	//m_pCharacter->StateMachineComponent->SwitchStateByKey(flyString);
	//Cast<UBasePlayerState>(m_pCharacter->StateMachineComponent->StateMap[flyString])->GetExtraStateInfo()->canBeAttackCanceled = false;

	//// Limit flySpeed
	//auto pCharMovement{ m_pCharacter->GetCharacterMovement() };
	//m_MaxFlySpeed = pCharMovement->MaxFlySpeed;
	//pCharMovement->MaxFlySpeed = m_pCharacter->KamehamehaFlySpeed;

	//// Set variables
	//m_IsFiring = false;
	//m_CurrentHoldTime = m_pCharacter->TimeToReachMaxKamehameha;

	//// Limit rotation
	//m_pCharacter->CameraRotationMultiplier = 0.25f;
	//pCharMovement->bOrientRotationToMovement = false;

	//// Start kamehameha animation
	//auto pAnimInstance{ m_pCharacter->GetMesh()->GetAnimInstance() };
	//pAnimInstance->Montage_Play(m_pCharacter->KamehamehaAttack.attackAnimationMontage);
	//pAnimInstance->Montage_Pause(m_pCharacter->KamehamehaAttack.attackAnimationMontage);
}
void UKamehameha_Ability::StopAbility()
{
	// If is not firing
	if (m_IsFiring == false) StartBeam();
}

void UKamehameha_Ability::AbilityEnd()
{
	//// Remove idleState from blacklist
	//m_pCharacter->StateMachineComponent->RemoveKeyFromBlacklist("Idle");

	//// Switch to idle
	//m_pCharacter->StateMachineComponent->SwitchStateByKey("Idle");
	//Cast<UBasePlayerState>(m_pCharacter->StateMachineComponent->StateMap["AirOption"])->GetExtraStateInfo()->canBeAttackCanceled = true;

	//// Reset flySpeed
	//auto pCharMovement{ m_pCharacter->GetCharacterMovement() };
	//pCharMovement->MaxFlySpeed = m_MaxFlySpeed;

	//// Reset rotation
	//m_pCharacter->CameraRotationMultiplier = 1.f;
	//pCharMovement->bOrientRotationToMovement = true;

	//FRotator newRotation{ m_pCharacter->GetActorRotation() };
	//newRotation.Pitch = 0;
	//newRotation.Roll = 0;
	//m_pCharacter->SetActorRotation(newRotation);

	//// Stop animation
	//m_pCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, m_pCharacter->KamehamehaAttack.attackAnimationMontage);

	// Stop beam if isn't deleted
	if (m_pBeam->IsValidLowLevel())
	{
		m_pBeam->StopMove(true);
	}

	// Set inactive
	SetIsActive(false);
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
	//// Pause animation at last second
	//if (0 < m_AnimationRunTime)
	//{
	//	m_AnimationRunTime -= deltaTime;
	//	if (m_AnimationRunTime <= 0) m_pCharacter->GetMesh()->GetAnimInstance()->Montage_Pause(m_pCharacter->KamehamehaAttack.attackAnimationMontage);
	//}
}

void UKamehameha_Ability::StartBeam()
{
	//m_IsFiring = true;

	//// Start animation
	//// ---------------
	//m_pCharacter->GetMesh()->GetAnimInstance()->Montage_Resume(m_pCharacter->KamehamehaAttack.attackAnimationMontage);
	//m_AnimationRunTime = m_pCharacter->KamehamehaAnimationStopTime;

	//// Spawn beam
	//// ----------

	//// Create
	//m_pBeam = GetWorld()->SpawnActor<ABaseBeam>(m_pCharacter->KamehamehaBeam, FTransform{});
	//m_pBeam->AttachToActor(m_pCharacter, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	//m_pBeam->SetCharacter(m_pCharacter);

	//// Transform
	//const float chargeMultiplier{ ((m_pCharacter->TimeToReachMaxKamehameha - m_CurrentHoldTime) / m_pCharacter->TimeToReachMaxKamehameha) * m_pCharacter->MaxKamehamehaScale + 1 };
	//FVector scale{ m_pBeam->GetActorScale3D() };
	//scale.X *= chargeMultiplier;
	//scale.Y *= chargeMultiplier;

	//m_pBeam->SetActorRelativeLocation(FVector{ 100.f, 0.f, 0.f });
	//m_pBeam->SetActorRelativeRotation(FRotator{ -90.f, 0.f, 0.f });
	//m_pBeam->SetActorScale3D(scale);

	//// Beam settings
	//const float damage{ m_pCharacter->KamehamehaAttack.damage * chargeMultiplier };
	//const float maxDistance = (chargeMultiplier <= 2.f) ? m_pCharacter->KamehamehaExistTime : m_pCharacter->KamehamehaExistTime * chargeMultiplier / 2.f;
	//const float existTime = (chargeMultiplier <= 1.1f) ? m_pCharacter->KamehamehaExistTime : m_pCharacter->KamehamehaExistTime * chargeMultiplier;

	//m_pBeam->Damage = damage;
	//m_pBeam->DamageFrequency = m_pCharacter->KamehamehaDamageFrequency;
	//m_pBeam->MovementSpeed = m_pCharacter->KamehamehaMovementSpeed;
	//m_pBeam->MaxDistance = m_pCharacter->KamehamehaMaxDistance;
	//m_pBeam->MaxExistTime = existTime;
	//m_pBeam->DisappearSpeed = m_pCharacter->KamehamehaDisappearSpeed;
	//m_pBeam->BeamMaterial = m_pCharacter->KamehamehaMaterial;

	//m_pBeam->SetVariables();

	//// Set character variables
	//// -----------------------

	//// Attack
	//m_pCharacter->CurrentAttack = m_pCharacter->KamehamehaAttack;

	//// Rotation
	//m_pCharacter->CameraRotationMultiplier = 0.01f;
}