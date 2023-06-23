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

protected:
	void BaseMove(const FVector2D& direction);

protected:
	ABaseCharacter* GetCharacter() const { return m_pCharacter; }
	ABasePlayerController* GetPlayerController() const { return m_pPlayerController; }

private:
	// Member variables
	ABaseCharacter* m_pCharacter;
	ABasePlayerController* m_pPlayerController;
};
