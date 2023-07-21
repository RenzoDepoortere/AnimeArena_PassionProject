// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Goku_Character.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API AGoku_Character : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AGoku_Character();

	// Publics
	UFUNCTION(BlueprintCallable, Category = Movement)
	float GetVerticalFlightInput() const { return m_VerticalFlightInput; }

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Member variables
	// ----------------
	float m_VerticalFlightInput;
	bool m_WasFlying;

	FName m_PreviousState;

	// Member functions
	// ----------------
	void FlyUp() { m_VerticalFlightInput = 1; }
	void StopFlyUp() { m_VerticalFlightInput = 0; }
	void FlyDown() { m_VerticalFlightInput = -1; }
	void StopFlyDown() { m_VerticalFlightInput = 0; }

	void OnStateSwitch(const FString& newState);

	virtual void Ability1() override;
};
