// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "InputActionValue.h"
#include "IdleState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UIdleState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;

private:
	// Member functions
	void Move(const FInputActionValue& value);
};
