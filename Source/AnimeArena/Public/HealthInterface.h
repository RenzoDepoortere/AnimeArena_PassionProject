// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthInterface.generated.h"

//class ABaseCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UHealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ANIMEARENA_API IHealthInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = Health)
		virtual void SetHealth(float amount) = 0;

	//UFUNCTION(BlueprintCallable, Category = Health)
	//	virtual bool DealDamage(float amount, ABaseCharacter* pDamageDealer) = 0;
};
