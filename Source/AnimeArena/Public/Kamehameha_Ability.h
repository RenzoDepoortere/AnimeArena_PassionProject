// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Kamehameha_Ability.generated.h"

class AGoku_Character;

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UKamehameha_Ability : public UBaseAbility
{
	GENERATED_BODY()
	
public:
	UKamehameha_Ability();
	virtual ~UKamehameha_Ability() = default;

	virtual void Update(float deltaTime) override;

public:
		virtual void ActivateAbility() override;
		virtual void StopAbility() override;

private:
	// Member variables
	// ----------------
	AGoku_Character* m_pCharacter;

	bool m_IsFiring;
	float m_CurrentHoldTime;
	float m_AnimationRunTime;

	// Member functions
	// ----------------
	void HoldTimeCountdown(float deltaTime);
	void HandleKamehameha(float deltaTime);

	void StartBeam();
	void AbilityEnd();
};
