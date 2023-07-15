// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Public/BaseAttack.h"
#include "BaseCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FAttackEndEvent);

class ABasePlayerController;
class UStateMachineComponent;

USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		UAnimMontage* attackAnimationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float knockback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		bool isComboEnder;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		bool isInvincible;
};

USTRUCT(BlueprintType)
struct FAttackString
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Main)
		FString stringPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Main)
		TArray<FAttack> attacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Options)
		bool groundBased;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Options)
		bool ultOnly;
};

UENUM(BlueprintType)
enum class EAttackEnum : uint8
{
	start UMETA(DisplayName = "Start"),
	active UMETA(DisplayName = "Active"),
	delay UMETA(DisplayName = "Delay")
};

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

	bool GetIsLocked() const { return m_IsLocked; }
	ABaseCharacter* GetLockedCharacter() const { return m_pLockedCharacter; }
	void FaceLockedCharacter();

	FAttackEndEvent* const GetAttackEndEvent() { return &m_AttackEndEvent; }

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
	UFUNCTION(BlueprintCallable, Category = Combat)
		bool GetLastAttackInput() const { return m_LastAttackWasLight; }

	UFUNCTION(BlueprintCallable, Category = Combat)
		void SetAttackState(EAttackEnum newAttackState) { CurrentAttackState = newAttackState; }
	UFUNCTION(BlueprintCallable, Category = Combat)
		void AttackEnded() { if (m_AttackEndEvent.IsBound()) m_AttackEndEvent.Broadcast(); }

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

	// Attacks
	// -------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		TArray<FAttackString> Attacks;
	UPROPERTY(BlueprintReadOnly, Category = Combat)
		EAttackEnum CurrentAttackState;

protected:
	ABasePlayerController* const GetPlayerController() { return m_pController; }

private:
	// Member variables
	ABasePlayerController* m_pController;
	FVector2D m_LastMovementInput;
	bool m_LastAttackWasLight;

	bool m_UsedAirAbility;
	bool m_UsedAirDash;

	bool m_IsLocked;
	ABaseCharacter* m_pLockedCharacter;

	UStateMachineComponent* m_pStateMachine;

	FAttackEndEvent m_AttackEndEvent;

	// Member functions
	// ----------------
	void Move(const FInputActionValue& value) { m_LastMovementInput = value.Get<FVector2D>(); }
	void StopMove() { m_LastMovementInput = {}; }

	void LightAttack();
	void HeavyAttack();

	void Look(const FInputActionValue& value);
	void LockToggle();

	void FollowLockedCharacter();
};
