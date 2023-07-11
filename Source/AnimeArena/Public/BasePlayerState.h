// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateBase.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */

class ABaseCharacter;
class ABasePlayerController;

UCLASS()
class ANIMEARENA_API UBasePlayerState : public UStateBase
{
	GENERATED_BODY()
	
public:
	virtual void OnEnter(AActor* pStateOwner);
	virtual void OnExit() {}
	virtual void Tick(float deltaTime) {}

public:
	struct extraStateInfo
	{
		bool isInvincible{ false };
		bool canBeAttackCanceled{ true };
	};
	
	const extraStateInfo& GetExtraStateInfo() const { return m_ExtraStateInfo; }

protected:
	void BaseMove(const FVector2D& direction);

protected:
	ABaseCharacter* GetCharacter() const { return m_pCharacter; }
	ABasePlayerController* GetPlayerController() const { return m_pPlayerController; }

	extraStateInfo* const GetExtraStateInfoPointer() { return &m_ExtraStateInfo; }

private:
	// Member variables
	ABaseCharacter* m_pCharacter;
	ABasePlayerController* m_pPlayerController;

	extraStateInfo m_ExtraStateInfo{};
};
