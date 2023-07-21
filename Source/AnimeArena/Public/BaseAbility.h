// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseAbility.generated.h"

class ABaseCharacter;

/**
 * 
 */
UCLASS()
class ANIMEARENA_API UBaseAbility : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseAbility();
	virtual ~UBaseAbility() = default;

	virtual void Update(float deltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float MaxCooldownTimer;
	UPROPERTY(BlueprintReadOnly, Category = Ability);
		float CurrentCooldown;

	UFUNCTION(BlueprintCallable, Category = Other)
		void SetCharacter(ABaseCharacter* pCharacter) { m_pCharacter = pCharacter; }
	UFUNCTION(BlueprintCallable, Category = Other)
		bool GetIsActive() const { return m_IsActive; }

	UFUNCTION(BlueprintCallable, Category = Ability)
		virtual void ActivateAbility();
	UFUNCTION(BlueprintCallable, Category = Ability)
		virtual void StopAbility() {}

protected:
	ABaseCharacter* const GetCharacter() { return m_pCharacter; }
	void SetIsActive(bool isActive) { m_IsActive = isActive; }

private:
	// Member variables
	ABaseCharacter* m_pCharacter;
	bool m_IsActive;
};
