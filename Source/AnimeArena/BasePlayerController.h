// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */

// Delegates (= Events)
DECLARE_MULTICAST_DELEGATE_OneParam(FMoveEvent, const FInputActionValue&);

UCLASS()
class ANIMEARENA_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Input functions
	void Move(const FInputActionValue& value) { if (m_MoveEvent.IsBound()) m_MoveEvent.Broadcast(value); }
	void StopMoving(const FInputActionValue& value);

	void Sprint(const FInputActionValue& value);
	void StopSprinting(const FInputActionValue& value);

	void Jump(const FInputActionValue& value);
	void StopJumping(const FInputActionValue& value);

	void Look(const FInputActionValue& value);

	// Getters
	FMoveEvent* const GetMoveEvent() { return &m_MoveEvent; }

public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* LookAction;
	
protected:
	virtual void SetupInputComponent() override;

private:
	// Member variables
	// ----------------

	// Events
	FMoveEvent m_MoveEvent;
};
