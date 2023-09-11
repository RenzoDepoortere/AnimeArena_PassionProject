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
	class USkeletalMeshComponent* Mesh;

	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Camera")
	class USpringArmComponent* SpringArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component|Camera")
	class UCameraComponent* Camera;

	// Variables
	// ---------
	
	// Input
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* LookAction;

	// Other
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Other")
	float CameraRotationSpeed;

	// Functions
	// ---------
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector2D GetLastMovementInput() const { return m_LastMovementInput; }

private:
	// Member variables
	// ----------------
	ABasePlayerController* m_pController;

	// Movement
	FVector2D m_LastMovementInput;

	// Member functions
	// ----------------
	void Look(const FInputActionValue& value);

	void Move(const FInputActionValue& value) { m_LastMovementInput = value.Get<FVector2D>(); }
	void StopMove() { m_LastMovementInput = {}; }
};
