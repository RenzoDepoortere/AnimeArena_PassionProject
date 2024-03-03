// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

USTRUCT(BlueprintType)
struct FSpeedLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ChangeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool IsLocked;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpeedSwitch, int, speedLevel);

UCLASS()
class ANIMEARENA_API ABaseCharacter : public APawn
{
	GENERATED_BODY()

public:
	ABaseCharacter();
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: 
	// Components
	// ----------
	
	// Base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Base")
	class UCapsuleComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components|Base")
	class USkeletalMeshComponent* Mesh;

	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Camera")
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Camera")
	class UCameraComponent* Camera;

	// Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Other")
	class UStateMachineComponent* StateMachineComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Other")
	class UKinematicController* KinematicController;

	// Events
	// ------
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSpeedSwitch OnSpeedSwitch;

	// Variables
	// ---------
	
	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	// Movement
	// ========

	// Speed level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|SpeedLevel")
	TArray<FSpeedLevel> SpeedLevels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|SpeedLevel")
	float NoSpeedLimit;

	// Dashing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashTime;

	// Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Other")
	float CameraRotationSpeed;

	// Functions
	// ---------

	// Movement
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	FVector2D GetLastMovementInput() const { return m_LastMovementInput; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	int32 GetCurrentSpeedLevel() const { return m_CurrentSpeedLevel; }

private:
	// Member variables
	// ----------------

	// Base
	class ABasePlayerController* m_pController;

	// Movement
	FVector2D m_LastMovementInput;

	int32 m_CurrentSpeedLevel;
	int32 m_LevelToReach;
	float m_CurrentLevelReachTime;

	// Member functions
	// ----------------
	void Look(const FInputActionValue& value);

	// Movement
	void Move(const FInputActionValue& value) { m_LastMovementInput = value.Get<FVector2D>(); }
	void StopMove() { m_LastMovementInput = {}; }

	void HandleSpeedLimit(float deltaTime);
};
