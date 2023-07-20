// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Kamehameha_Ability.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UKamehameha_Ability : public UBaseAbility
{
	GENERATED_BODY()
	
public:
	UKamehameha_Ability() = default;
	virtual ~UKamehameha_Ability() = default;

	virtual void Update(float deltaTime) override;

public:
		virtual void ActivateAbility() override;
		virtual void StopAbility() override;
};
