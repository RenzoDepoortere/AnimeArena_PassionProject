// Fill out your copyright notice in the Description page of Project Settings.
#include "StateBase.h"

void UStateBase::OnEnter(AActor* pStateOwner)
{ 
	m_pStateOwner = pStateOwner; 
	m_CurrentCooldown = m_MaxCooldown;
}

void UStateBase::Cooldown(float deltaTime)
{
	if (0 < m_CurrentCooldown) m_CurrentCooldown -= deltaTime;
}