// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Public/HealthInterface.h"
#include "BaseCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FAttackEndEvent);

class ABasePlayerController;
class UStateMachineComponent;
class UBoxComponent;
class UBaseAbility;

#pragma region Objects

UENUM(BlueprintType)
enum class EAttackEnum : uint8
{
	start UMETA(DisplayName = "Start"),
	active UMETA(DisplayName = "Active"),
	delay UMETA(DisplayName = "Delay")
};

UENUM(BlueprintType)
enum class EAttackDirectionEnum : uint8
{
	forward UMETA(DisplayName = "Forward"),
	backward UMETA(DisplayName = "Backward"),
	left UMETA(DisplayName = "Left"),
	right UMETA(DisplayName = "Right"),
	up UMETA(DisplayName = "Up"),
	down UMETA(DisplayName = "Down")
};

USTRUCT(BlueprintType)
struct FAttack
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		UAnimMontage* attackAnimationMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float attackDamageStartTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float knockback;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float HitStunTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		EAttackDirectionEnum knockbackDirection;

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

#pragma endregion

UCLASS()
class ANIMEARENA_API ABaseCharacter : public ACharacter, public IHealthInterface
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
	void FaceActor(AActor* pActor);

	FAttackEndEvent* const GetAttackEndEvent() { return &m_AttackEndEvent; }

public:
	ABaseCharacter();

	// Settings
	// --------

	// Controllability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Controllability")
		bool CanBeControlled = true;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Health")
		float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Health")
		UMaterialInterface* FlickerMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Health")
		UAnimMontage* HitMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Health")
		UAnimMontage* BigHitMontage;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		float MaxMovementSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		float SprintBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		float DashBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		float DashCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		bool HasAirOption;

	// Ability
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ability")
		float Ability1_Cooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ability")
		float Ability2_Cooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ability")
		float Ability3_Cooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Ability")
		float Ability4_Cooldown;

	// Components
	// ----------

	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Camera")
		class USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Camera")
		class UCameraComponent* FollowCamera;

	// Others
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Collision")
		class UBoxComponent* DamageBoxCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|States")
		class UStateMachineComponent* StateMachineComponent;

	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		class UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
		class UInputAction* LockAction;

	// Other variables
	// ---------------

	// Attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
		TArray<FAttackString> Attacks;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		EAttackEnum CurrentAttackState;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		FAttack CurrentAttack;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		float LastHitStun;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
		bool LastWasFinisher;

	// Ability
	UPROPERTY(BlueprintReadOnly, Category = "Ability")
		TArray<UBaseAbility*> Abilities;

	// Functions
	// ---------

	UFUNCTION(BlueprintCallable, Category = "Movement")
		const FVector2D& GetLastMovementInput() const { return m_LastMovementInput; }
	UFUNCTION(BlueprintCallable, Category = "Combat")
		bool GetLastAttackInput() const { return m_LastAttackWasLight; }

	// Combat
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void SetAttackState(EAttackEnum newAttackState) { CurrentAttackState = newAttackState; }
	UFUNCTION(BlueprintCallable, Category = "Combat")
		void AttackEnded() { if (m_AttackEndEvent.IsBound()) m_AttackEndEvent.Broadcast(); }

	UFUNCTION(BlueprintCallable, Category = Combat)
		void IsActiveHit(bool isActiveHit);

	// Health
	UFUNCTION(BlueprintCallable, Category = "Health")
		virtual void SetHealth(float amount) override;
	UFUNCTION(BlueprintCallable, Category = "Health")
		virtual bool DealDamage(float amount, ABaseCharacter* pDamageDealer) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetHealth() const { return m_CurrentHealth; }

protected:
	ABasePlayerController* const GetPlayerController() { return m_pController; }

	virtual void OnDamage(float amount, ABaseCharacter* pDamageDealer);
	virtual void OnDeath(float amount, ABaseCharacter* pKiller);

	virtual void Ability1() { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta, "Ability 1"); }
	virtual void Ability1Stop() {};

	virtual void Ability2() { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta, "Ability 2"); }
	virtual void Ability2Stop() {};

	virtual void Ability3() { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta, "Ability 3"); }
	virtual void Ability3Stop() {};

	virtual void Ability4() { GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Magenta, "Ability 4"); }
	virtual void Ability4Stop() {};


private:
	// Member variables
	ABasePlayerController* m_pController;
	FVector2D m_LastMovementInput;
	bool m_LastAttackWasLight;

	bool m_UsedAirAbility;
	bool m_UsedAirDash;

	bool m_IsLocked;
	ABaseCharacter* m_pLockedCharacter;

	FAttackEndEvent m_AttackEndEvent;
	TArray<AActor*> m_HitActors;
	bool m_IsActiveHit;

	float m_CurrentHealth;
	TArray<UMaterialInterface*> m_MeshMaterials;
	float m_CurrentHitTime;

	// Member functions
	// ----------------
	void Move(const FInputActionValue& value) { m_LastMovementInput = value.Get<FVector2D>(); }
	void StopMove() { m_LastMovementInput = {}; }

	void LightAttack();
	void HeavyAttack();
	void HandleAttacks();

	void Look(const FInputActionValue& value);
	void LockToggle();
	void FollowLockedCharacter();

	void ResetMaterials();
};
