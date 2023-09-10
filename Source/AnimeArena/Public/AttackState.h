// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerState.h"
//#include "../BaseCharacter.h"
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

	void LaunchAttack();

private:
	// Member variables
	FString m_CurrentAttackString;
	//TArray<FAttackString> m_PossibleAttackStrings;
	
	int m_CurrentAttack;
	bool m_PreppingAttack;
	//FAttack m_AttackToUse;

	float m_OriginalFriction;

	// Member functions
	void LightAttack() { AttackInput("X"); }
	void HeavyAttack() { AttackInput("Y"); }

	void AttackInput(const FString& attackLetter);
	void AttackEnded();

	FString ConvertInputToString(FVector2D movementInput, const FString& attackLetter) const;
};
