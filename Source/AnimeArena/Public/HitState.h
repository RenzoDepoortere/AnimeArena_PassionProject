// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "HitState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UHitState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	UHitState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime) override;

private:
	// Member functions
	float m_CurrentStunTime;
};
