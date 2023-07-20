// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseAbility.h"

UBaseAbility::UBaseAbility()
	: m_pCharacter{ nullptr }
{
}

void UBaseAbility::Update(float deltaTime)
{
	if (0 < CurrentCooldown) CurrentCooldown -= deltaTime;
}