// Fill out your copyright notice in the Description page of Project Settings.
#include "HitState.h"
#include "../BaseCharacter.h"
#include "StateMachineComponent.h"

UHitState::UHitState()
	: m_CurrentStunTime{}
{
	StateDisplayName = "Hit";
	GetExtraStateInfoPointer()->canBeAttackCanceled = false;
}

void UHitState::OnEnter(AActor* pStateOwner)
{
	UBasePlayerState::OnEnter(pStateOwner);

	// Set stunTime
	auto pCharacter{ GetCharacter() };
	m_CurrentStunTime = pCharacter->LastHitStun;

	// Play montage
	UAnimMontage* pMontageToPlay{ pCharacter->HitMontage };
	if (pCharacter->LastWasFinisher) pMontageToPlay = pCharacter->BigHitMontage;

	pCharacter->GetMesh()->GetAnimInstance()->Montage_Play(pMontageToPlay);
}
void UHitState::OnExit()
{
	auto pCharacter{ GetCharacter() };
	pCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.1f);
}
void UHitState::Tick(float deltaTime)
{
	// Countdown
	m_CurrentStunTime -= deltaTime;
	if (m_CurrentStunTime <= 0)
	{
		// Change to idleState
		auto pStateMachine{ GetStateOwner()->GetComponentByClass<UStateMachineComponent>() };
		pStateMachine->SwitchStateByKey({ "Idle" });
	}
}