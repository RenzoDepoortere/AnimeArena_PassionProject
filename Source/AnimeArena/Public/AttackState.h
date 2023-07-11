// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "AttackState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UAttackState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	UAttackState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;
};
