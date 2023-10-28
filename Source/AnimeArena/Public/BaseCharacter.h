// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class ESpeedLevel : uint8
{
	Level_1		UMETA(DisplayName = "Level_1"),
	Level_2		UMETA(DisplayName = "Level_2"),
	Level_3		UMETA(DisplayName = "Level_3"),
	Level_4		UMETA(DisplayName = "Level_4")
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
	float Level1_SpeedLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|SpeedLevel")
	float Level2_SpeedLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|SpeedLevel")
	float Level3_SpeedLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|SpeedLevel")
	float Level4_SpeedLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|SpeedLevel")
	float SpeedLimitTreshold;

	// Dashing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashPerfectMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement|Dash")
	float DashGraceTime;

	// Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Other")
	float CameraRotationSpeed;

	// Functions
	// ---------

	// Movement
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	FVector2D GetLastMovementInput() const { return m_LastMovementInput; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	ESpeedLevel GetCurrentSpeedLevel() const { return m_CurrentSpeedLevel; }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float GetCurrentDashTime() const { return m_CurrentDashTime; }

public:
	void Dash();


private:
	// Member variables
	// ----------------

	// Base
	class ABasePlayerController* m_pController;

	// Movement
	FVector2D m_LastMovementInput;
	ESpeedLevel m_CurrentSpeedLevel;
	TArray<float> m_SpeedLevelTresholds;

	float m_CurrentDashTime;

	// Member functions
	// ----------------
	void Look(const FInputActionValue& value);

	// Movement
	void Move(const FInputActionValue& value) { m_LastMovementInput = value.Get<FVector2D>(); }
	void StopMove() { m_LastMovementInput = {}; }

	void HandleSpeedLimit();
	void HandleDashTimer(float deltaTime);
};
