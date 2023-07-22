// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAbility.h"
#include "Kamehameha_Ability.generated.h"

class AGoku_Character;
class ABaseBeam;

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

		void AbilityEnd();

private:
	// Member variables
	// ----------------
	AGoku_Character* m_pCharacter;
	ABaseBeam* m_pBeam;

	bool m_IsFiring;
	float m_CurrentHoldTime;
	float m_AnimationRunTime;

	float m_MaxFlySpeed;

	// Member functions
	// ----------------
	void HoldTimeCountdown(float deltaTime);
	void HandleKamehameha(float deltaTime);

	void StartBeam();
};
