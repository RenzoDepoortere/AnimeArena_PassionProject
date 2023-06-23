// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "InputActionValue.h"
#include "JumpState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UJumpState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	UJumpState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime) override;

private:


	// Member functions
	void Move(const FInputActionValue& value);
	void Jump();
	void StopJump();
};
