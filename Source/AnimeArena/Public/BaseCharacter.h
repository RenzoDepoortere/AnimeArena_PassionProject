// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

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
	class UBoxComponent* GroundCollision;
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

	// Variables
	// ---------
	
	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float MaxMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float MoveAccelerationTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float MaxFallSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
	float FallAccelerationTime;

	// Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Other")
	float CameraRotationSpeed;

	// Functions
	// ---------

	// Movement
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector2D GetLastMovementInput() const { return m_LastMovementInput; }
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool GetIsInAir() const { return m_IsInAir; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector& GetTotalVelocity() { return m_TotalVelocity; }
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetShouldFall(bool shouldFall) { m_ShouldFall = shouldFall; }

private:
	// Member variables
	// ----------------

	// Base
	class ABasePlayerController* m_pController;

	// Movement
	FVector2D m_LastMovementInput;
	bool m_IsInAir;
	bool m_ShouldFall;

	FVector m_TotalVelocity;

	// Member functions
	// ----------------
	void Look(const FInputActionValue& value);

	// Movement
	void Move(const FInputActionValue& value) { m_LastMovementInput = value.Get<FVector2D>(); }
	void StopMove() { m_LastMovementInput = {}; }

	void HandleGravity(float deltaTime);
	void HandleDisplacement(float deltaTime);

	// Overlap
	UFUNCTION()
	void OnGroundBeginOverlap(	UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
								int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);
	UFUNCTION()
	void OnGroundEndOverlap(	UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
								int32 otherBodyIndex);
};
