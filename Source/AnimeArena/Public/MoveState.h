// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "MoveState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UMoveState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	UMoveState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;
	virtual void Tick(float deltaTime) override;

private:
	// Member variables
	// ----------------
	bool m_IsInput;
	FVector2D m_LastInput;
	FVector m_CurrentDirection;

	float m_MoveSpeed;
	FRotator m_DesiredRotation;

	// Member functions
	// ----------------
	void Move(const FInputActionValue& value);
	void StopMove() { m_IsInput = false; }

	void HandleInput(float deltaTime);

	//void Jump();
	//void Dash();
};
