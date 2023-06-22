// Fill out your copyright notice in the Description page of Project Settings.
#include "MoveState.h"

void UMoveState::OnEnter(AActor* pStateOwner)
{
	Super::OnEnter(pStateOwner);

	// Set animation

	// Subscribe to inputEvents

}
void UMoveState::OnExit()
{
	// Unsubscribe from inputEvents
}
void UMoveState::Tick(float /*deltaTime*/)
{

}