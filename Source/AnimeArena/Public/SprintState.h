// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateBase.h"
#include "InputActionValue.h"
#include "SprintState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API USprintState : public UStateBase
{
	GENERATED_BODY()
	
public:
	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime) override;

private:
	// Member variables
	// ----------------
	ACharacter* m_pCharacter;

	float m_MaxWalkingSpeed;
	float m_MaxAcceleration;
	bool m_HasToSlowDown;

	float m_MaxMoveMult;
	float m_MaxAccelMult;


	// Member functions
	// ----------------
	void Move(const FInputActionValue& value);
	void StopMove();

	void StopSprint();

	void Slowdown(float DeltaTime);
};
