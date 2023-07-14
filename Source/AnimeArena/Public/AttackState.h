// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
#include "../BaseCharacter.h"
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

private:
	// Member variables
	FString m_CurrentAttackString;
	TArray<FAttackStruct> m_PossibleAttackStrings;
	
	// Member functions
	void LightAttack();
	void HeavyAttack();

	void AttackEnded();

	FString ConvertInputToString(FVector2D movementInput) const;
};
