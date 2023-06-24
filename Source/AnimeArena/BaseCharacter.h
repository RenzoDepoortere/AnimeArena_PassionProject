// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ANIMEARENA_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Settings
	// --------

	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float MaxMovementSpeedMult;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float SprintBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float DashBoost;

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

protected:
	// Input	
	void Look(const FInputActionValue& value);
};