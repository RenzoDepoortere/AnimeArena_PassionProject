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
DECLARE_MULTICAST_DELEGATE(FMoveStopEvent);

DECLARE_MULTICAST_DELEGATE(FSprintEvent);
DECLARE_MULTICAST_DELEGATE(FSprintStopEvent);

DECLARE_MULTICAST_DELEGATE(FJumpEvent);
DECLARE_MULTICAST_DELEGATE(FJumpStopEvent);

UCLASS()
class ANIMEARENA_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Input functions
	void Move(const FInputActionValue& value) { if (m_MoveEvent.IsBound()) m_MoveEvent.Broadcast(value); }
	void StopMoving(const FInputActionValue& /*value*/) { if (m_MoveStopEvent.IsBound()) m_MoveStopEvent.Broadcast(); }

	void Sprint(const FInputActionValue& /*value*/) { if (m_SprintEvent.IsBound()) m_SprintEvent.Broadcast(); }
	void StopSprinting(const FInputActionValue& /*value*/) { if (m_SprintStopEvent.IsBound()) m_SprintStopEvent.Broadcast(); }

	void Jump(const FInputActionValue& /*value*/) { if (m_JumpEvent.IsBound()) m_JumpEvent.Broadcast(); }
	void StopJumping(const FInputActionValue& /*value*/) { if (m_JumpStopEvent.IsBound()) m_JumpStopEvent.Broadcast(); }

	// Getters
	FMoveEvent* const GetMoveEvent() { return &m_MoveEvent; }
	FMoveStopEvent* const GetMoveStopEvent() { return &m_MoveStopEvent; }

	FSprintEvent* const GetSprintEvent() { return &m_SprintEvent; }
	FSprintStopEvent* const GetSprintStopEvent() { return &m_SprintStopEvent; }

	FJumpEvent* const GetJumpEvent() { return &m_JumpEvent; }
	FJumpStopEvent* const GetJumpStopEvent() { return &m_JumpStopEvent; }

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
	FMoveStopEvent m_MoveStopEvent;

	FSprintEvent m_SprintEvent;
	FSprintStopEvent m_SprintStopEvent;

	FJumpEvent m_JumpEvent;
	FJumpStopEvent m_JumpStopEvent;
};
