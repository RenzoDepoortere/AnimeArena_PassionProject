// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KinematicController.generated.h"

DECLARE_MULTICAST_DELEGATE(FLandEvent);
DECLARE_MULTICAST_DELEGATE(FNoMovement);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANIMEARENA_API UKinematicController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKinematicController();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:	

	// Variables
	// ---------

	// General
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|General")
	float ForceDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|General")
	float RotationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|General")
	bool UseForwardVector;

	// Grounded
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Grounded")
	float MaxMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Grounded")
	float MoveAccelerationTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Grounded")
	float AngularDrag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Grounded", meta = (UIMin = 0.0f, UIMax = 90.0f, ClampMin = 0.0f, ClampMax = 90.0f, SliderExponent = 1.f))
	float MaxSlopeAngle;

	// Air
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Air")
	float JumpGroundBufferTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Air")
	float JumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Air")
	float MaxJumpTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Air")
	float MaxJumpHoldTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Air")
	float MaxFallSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Air")
	float FallAccelerationTime;

	// Collision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Collision")
	float SkinWidth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Collision")
	float GroundCollisionLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Collision")
	float WallCollisionLength;

	// Function
	// --------

	UFUNCTION(BlueprintCallable, Category = "General")
	FVector& GetTotalVelocity() { return m_TotalVelocity; }
	UFUNCTION(BlueprintCallable, Category = "General")
	void RotateCharacter(const FVector2D& input, bool instantTurn = false);
	UFUNCTION(BlueprintCallable, Category = "General")
	FVector ConvertInputToWorld(const FVector2D& input);

	UFUNCTION(BlueprintCallable, Category = "General")
	void AddImpulse(const FVector& force, bool resetVelocity = false);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void MoveCharacter(const FVector2D& input, bool rotateCharacter = true);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetShouldMove(bool shouldMove) { m_ShouldMove = shouldMove; }
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetKeepMomentum(bool keepMomentum) { m_KeepMomentum = keepMomentum; }

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMoveSpeed(float moveSpeed) { m_MoveSpeed = moveSpeed; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	float GetMoveSpeed() const { return m_MoveSpeed; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Air")
	bool GetIsInAir() const { return m_IsInAir; }
	UFUNCTION(BlueprintCallable, Category = "Air")
	void SetShouldFall(bool shouldFall) { m_ShouldFall = shouldFall; }
	UFUNCTION(BlueprintCallable, Category = "Air")
	void SetCheckGround(bool checkGround) { m_CheckGround = checkGround; }

public:
	FLandEvent* const GetLandEvent() { return &m_LandEvent; }
	FNoMovement* const GetNoMovementEvent() { return &m_NoMovementEvent; }
		
private:
	// Member variables
	// ----------------

	// General
	class AActor* m_pActor;
	class UCapsuleComponent* m_pCollision;
	class APlayerController* m_pController;

	FVector m_TotalVelocity;
	FVector m_CurrentDirection;
	FRotator m_DesiredRotation;

	// Grounded
	float m_MoveSpeed;
	bool m_ShouldMove;
	bool m_KeepMomentum;

	// Air
	bool m_IsInAir;
	bool m_ShouldFall;
	bool m_CheckGround;

	// Events
	FLandEvent m_LandEvent;
	FNoMovement m_NoMovementEvent;

	// Member functions
	// ----------------
	void CheckGround(float deltaTime);
	void HandleGravity(float deltaTime);
	void HandleMovement(float deltaTime);
	void HandleWallCollision(float deltaTime);
	void HandleDisplacement(float deltaTime);
	void HandleRotation(float deltaTime);

	void SlideAlongWall(const FVector& velocity, const FVector& hitNormal);
	void SlideAlongSlope(const FVector& velocity, const FVector& hitNormal);
};
