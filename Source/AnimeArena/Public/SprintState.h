// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "InputActionValue.h"
#include "SprintState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API USprintState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	USprintState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;

private:
	// Member variables
	// ----------------
	float m_MaxWalkingSpeed;
	float m_MaxMoveMult;

	// Member functions
	// ----------------
	void Move(const FInputActionValue& value);
	void StopMove();

	void StopSprint();
	void Jump();
	void Dash();
};
