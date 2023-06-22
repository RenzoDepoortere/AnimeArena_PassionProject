// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Input actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

	// Input functions
	void Move(const FInputActionValue& value);
	void StopMoving(const FInputActionValue& value);

	void Sprint(const FInputActionValue& value);
	void StopSprinting(const FInputActionValue& value);

	void Jump(const FInputActionValue& value);
	void StopJumping(const FInputActionValue& value);

	void Look(const FInputActionValue& value);
	
protected:
	virtual void SetupInputComponent() override;
};
