// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseAbility.h"

UBaseAbility::UBaseAbility()
	: /*m_pCharacter{ nullptr }
	,*/ m_IsActive{ false }
{
}

void UBaseAbility::Cooldown(float deltaTime)
{
	if (0 < CurrentCooldown) CurrentCooldown -= deltaTime;
}

void UBaseAbility::ActivateAbility()
{
	CurrentCooldown = MaxCooldownTimer;
	m_IsActive = true;
}