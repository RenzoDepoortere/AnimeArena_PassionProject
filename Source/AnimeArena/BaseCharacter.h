// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

class ABasePlayerController;

UCLASS()
class ANIMEARENA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	void SetUsedAirAbility(bool usedAirAbility) { m_UsedAirAbility = usedAirAbility; }
	bool GetUsedAirAbility() const { return m_UsedAirAbility; }

	void SetUsedAirDash(bool usedAirDash) { m_UsedAirDash = usedAirDash; }
	bool GetUsedAirDash() const { return m_UsedAirDash; }

public:
	ABaseCharacter();

	// Settings
	// --------

	// Controllability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		bool CanBeControlled = true;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float MaxMovementSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float SprintBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float DashBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float DashCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		bool HasAirOption;

	UFUNCTION(BlueprintCallable, Category = Movement)
	const FVector2D& GetLastMovementInput() const { return m_LastMovementInput; }

	// Components
	// ----------

	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		class UCameraComponent* FollowCamera;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* LockAction;

protected:
	ABasePlayerController* const GetPlayerController() { return m_pController; }

private:
	// Member variables
	ABasePlayerController* m_pController;
	FVector2D m_LastMovementInput;

	bool m_UsedAirAbility;
	bool m_UsedAirDash;

	bool m_IsLocked;
	ABaseCharacter* m_pLockedCharacter;

	// Member functions
	// ----------------
	void Move(const FInputActionValue& value);
	void StopMove();

	void Look(const FInputActionValue& value);
	void LockToggle();

	void FollowLockedCharacter();
};
