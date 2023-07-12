// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseAttack.generated.h"

/**
 * 
 */

enum CurrentAttackState
{
	Start, Active, Delay
};

UCLASS(Blueprintable)
class ANIMEARENA_API UBaseAttack : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseAttack();

	CurrentAttackState GetCurrentAttackState() const { return m_CurrentAttackState; }

public:

	// Properties
	// ----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		UAnimMontage* AttackAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float Knockback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		bool IsComboEnder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		bool IsInvincible;

private:
	// Member variables
	CurrentAttackState m_CurrentAttackState;
};
