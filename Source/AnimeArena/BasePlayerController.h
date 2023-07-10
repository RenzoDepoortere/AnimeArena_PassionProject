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
DECLARE_MULTICAST_DELEGATE(FSprintTapEvent);
DECLARE_MULTICAST_DELEGATE(FSprintDoubleTapEvent);
DECLARE_MULTICAST_DELEGATE(FSprintStopEvent);

DECLARE_MULTICAST_DELEGATE(FJumpEvent);
DECLARE_MULTICAST_DELEGATE(FJumpHoldEvent);
DECLARE_MULTICAST_DELEGATE(FJumpStopEvent);

DECLARE_MULTICAST_DELEGATE(FDashEvent);

UCLASS()
class ANIMEARENA_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor
	ABasePlayerController();

	// Input functions
	void Move(const FInputActionValue& value) { if (m_MoveEvent.IsBound()) m_MoveEvent.Broadcast(value); }
	void StopMoving(const FInputActionValue& /*value*/) { if (m_MoveStopEvent.IsBound()) m_MoveStopEvent.Broadcast(); }

	void Sprint(const FInputActionValue& /*value*/) { if (m_SprintEvent.IsBound()) m_SprintEvent.Broadcast(); }
	void SprintTap(const FInputActionValue& /*value*/);
	void StopSprinting(const FInputActionValue& /*value*/) { if (m_SprintStopEvent.IsBound()) m_SprintStopEvent.Broadcast(); }

	void Jump(const FInputActionValue& /*value*/) { if (m_JumpEvent.IsBound()) m_JumpEvent.Broadcast(); }
	void JumpHold(const FInputActionValue& /*value*/) { if (m_JumpHoldEvent.IsBound()) m_JumpHoldEvent.Broadcast(); }
	void StopJumping(const FInputActionValue& /*value*/) { if (m_JumpStopEvent.IsBound()) m_JumpStopEvent.Broadcast(); }

	void Dash(const FInputActionValue& /*value*/) { if (m_DashEvent.IsBound()) m_DashEvent.Broadcast(); }

	// Getters
	FMoveEvent* const GetMoveEvent() { return &m_MoveEvent; }
	FMoveStopEvent* const GetMoveStopEvent() { return &m_MoveStopEvent; }

	FSprintEvent* const GetSprintEvent() { return &m_SprintEvent; }
	FSprintTapEvent* const GetSprintTapEvent() { return &m_SprintTapEvent; }
	FSprintDoubleTapEvent* const GetSprintDoubleTapEvent() { return &m_SprintDoubleTapEvent; }
	FSprintStopEvent* const GetSprintStopEvent() { return &m_SprintStopEvent; }

	FJumpEvent* const GetJumpEvent() { return &m_JumpEvent; }
	FJumpHoldEvent* const GetJumpHoldEvent() { return &m_JumpHoldEvent; }
	FJumpStopEvent* const GetJumpStopEvent() { return &m_JumpStopEvent; }

	FDashEvent* const GetDashEvent() { return &m_DashEvent; }

public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UInputAction* DashAction;
	
protected:
	virtual void SetupInputComponent() override;

private:
	// Member variables
	// ----------------

	// Events
	FMoveEvent m_MoveEvent;
	FMoveStopEvent m_MoveStopEvent;

	FSprintEvent m_SprintEvent;
	FSprintTapEvent m_SprintTapEvent;
	FSprintDoubleTapEvent m_SprintDoubleTapEvent;
	FSprintStopEvent m_SprintStopEvent;

	FJumpEvent m_JumpEvent;
	FJumpHoldEvent m_JumpHoldEvent;
	FJumpStopEvent m_JumpStopEvent;

	FDashEvent m_DashEvent;

	// Times
	float m_LastSprintTapTime;
};
