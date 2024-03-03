// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "DashState.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UDashState : public UBasePlayerState
{
	GENERATED_BODY()
	
public:
	UDashState();

	virtual void OnEnter(AActor* pStateOwner) override;
	virtual void OnExit() override;

private:
	// Member variables
	bool m_HasSetCooldown;
	FTimerHandle m_TimerHandle;

	// Member functions
	void Jump();
};
