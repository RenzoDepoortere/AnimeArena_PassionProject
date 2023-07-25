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

	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Abilities
	// ---------

	// Kamehameha
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha")
		FAttack KamehamehaAttack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		TSubclassOf<ABaseBeam> KamehamehaBeam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		float KamehamehaDamageFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		float KamehamehaMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		float KamehamehaMaxDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		float KamehamehaExistTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		float KamehamehaDisappearSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha|Beam")
		UMaterialInstance* KamehamehaMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha")
		float KamehamehaAnimationStopTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha")
		float TimeToReachMaxKamehameha;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha")
		float MaxKamehamehaScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability|Kamehameha")
		float KamehamehaFlySpeed;

	// Movement
	UFUNCTION(BlueprintCallable, Category = "Movement")
		float GetVerticalFlightInput() const { return m_VerticalFlightInput; }
	virtual bool GetIsInAir() const override;

	UFUNCTION()
		void OnStateSwitch(const FString& newState);

private:
	// Member variables
	// ----------------
	float m_VerticalFlightInput;
	bool m_ShouldStopVelocity;

	// Member functions
	// ----------------
	void FlyUp() { m_VerticalFlightInput = 1; }
	void StopFlyUp() { m_VerticalFlightInput = 0; }
	void FlyDown() { m_VerticalFlightInput = -1; }
	void StopFlyDown() { m_VerticalFlightInput = 0; }

	virtual void Ability1() override;
	virtual void Ability1Stop() override;

	virtual void BeamStop() override;
};
