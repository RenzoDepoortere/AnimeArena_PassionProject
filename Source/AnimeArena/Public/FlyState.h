// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "InputActionValue.h"
#include "FlyState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UFlyState : public UBasePlayerState
{
	GENERATED_BODY()

public:
	UFlyState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;

private:


	// Member functions
	void Move(const FInputActionValue& value);
	void StopMove();

	void FlyUp();
	void StopFlyUp();

	void FlyDown();
	void StopFlyDown();

	void StopFly();
};