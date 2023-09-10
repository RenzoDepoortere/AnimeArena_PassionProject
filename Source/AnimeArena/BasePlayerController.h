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

DECLARE_MULTICAST_DELEGATE(FJumpEvent);
DECLARE_MULTICAST_DELEGATE(FJumpHoldEvent);
DECLARE_MULTICAST_DELEGATE(FJumpStopEvent);

DECLARE_MULTICAST_DELEGATE(FDashEvent);

DECLARE_MULTICAST_DELEGATE(FLightAttackEvent);
DECLARE_MULTICAST_DELEGATE(FHeavyAttackEvent);

DECLARE_MULTICAST_DELEGATE(FAbility1Event);
DECLARE_MULTICAST_DELEGATE(FAbility1StopEvent);

DECLARE_MULTICAST_DELEGATE(FAbility2Event);
DECLARE_MULTICAST_DELEGATE(FAbility2StopEvent);

DECLARE_MULTICAST_DELEGATE(FAbility3Event);
DECLARE_MULTICAST_DELEGATE(FAbility3StopEvent);

DECLARE_MULTICAST_DELEGATE(FAbility4Event);
DECLARE_MULTICAST_DELEGATE(FAbility4StopEvent);


UCLASS()
class ANIMEARENA_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABasePlayerController();

	// Input functions
	// ---------------

	// Movement
	void Move(const FInputActionValue& value) { if (m_MoveEvent.IsBound()) m_MoveEvent.Broadcast(value); }
	void StopMoving(const FInputActionValue& /*value*/) { if (m_MoveStopEvent.IsBound()) m_MoveStopEvent.Broadcast(); }

	void Jump(const FInputActionValue& /*value*/) { if (m_JumpEvent.IsBound()) m_JumpEvent.Broadcast(); }
	void JumpHold(const FInputActionValue& /*value*/) { if (m_JumpHoldEvent.IsBound()) m_JumpHoldEvent.Broadcast(); }
	void StopJumping(const FInputActionValue& /*value*/) { if (m_JumpStopEvent.IsBound()) m_JumpStopEvent.Broadcast(); }

	void Dash(const FInputActionValue& /*value*/) { if (m_DashEvent.IsBound()) m_DashEvent.Broadcast(); }

	// Attack
	void LightAttack(const FInputActionValue& /*value*/) { if (m_LightAttackEvent.IsBound()) m_LightAttackEvent.Broadcast(); }
	void HeavyAttack(const FInputActionValue& /*value*/) { if (m_HeavyAttackEvent.IsBound()) m_HeavyAttackEvent.Broadcast(); }

	// Abilities
	void Ability1(const FInputActionValue& /*value*/) { if (m_Ability1Event.IsBound()) m_Ability1Event.Broadcast(); }
	void Ability1Stop(const FInputActionValue& /*value*/) { if (m_Ability1StopEvent.IsBound()) m_Ability1StopEvent.Broadcast(); }

	void Ability2(const FInputActionValue& /*value*/) { if (m_Ability2Event.IsBound()) m_Ability2Event.Broadcast(); }
	void Ability2Stop(const FInputActionValue& /*value*/) { if (m_Ability2StopEvent.IsBound()) m_Ability2StopEvent.Broadcast(); }

	void Ability3(const FInputActionValue& /*value*/) { if (m_Ability3Event.IsBound()) m_Ability3Event.Broadcast(); }
	void Ability3Stop(const FInputActionValue& /*value*/) { if (m_Ability3StopEvent.IsBound()) m_Ability3StopEvent.Broadcast(); }

	void Ability4(const FInputActionValue& /*value*/) { if (m_Ability4Event.IsBound()) m_Ability4Event.Broadcast(); }
	void Ability4Stop(const FInputActionValue& /*value*/) { if (m_Ability4StopEvent.IsBound()) m_Ability4StopEvent.Broadcast(); }

	// Getters
	// -------

	// Movement
	FMoveEvent* const GetMoveEvent() { return &m_MoveEvent; }
	FMoveStopEvent* const GetMoveStopEvent() { return &m_MoveStopEvent; }

	FJumpEvent* const GetJumpEvent() { return &m_JumpEvent; }
	FJumpHoldEvent* const GetJumpHoldEvent() { return &m_JumpHoldEvent; }
	FJumpStopEvent* const GetJumpStopEvent() { return &m_JumpStopEvent; }

	FDashEvent* const GetDashEvent() { return &m_DashEvent; }

	// Attack
	FLightAttackEvent* const GetLightAttackEvent() { return &m_LightAttackEvent; }
	FHeavyAttackEvent* const GetHeavyAttackEvent() { return &m_HeavyAttackEvent; }

	// Abilities
	FAbility1Event* const GetAbility1Event() { return &m_Ability1Event; }
	FAbility1StopEvent* const GetAbility1StopEvent() { return &m_Ability1StopEvent; }

	FAbility2Event* const GetAbility2Event() { return &m_Ability2Event; }
	FAbility2StopEvent* const GetAbility2StopEvent() { return &m_Ability2StopEvent; }

	FAbility3Event* const GetAbility3Event() { return &m_Ability3Event; }
	FAbility3StopEvent* const GetAbility3StopEvent() { return &m_Ability3StopEvent; }

	FAbility4Event* const GetAbility4Event() { return &m_Ability4Event; }
	FAbility4StopEvent* const GetAbility4StopEvent() { return &m_Ability4StopEvent; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
		class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
		class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement")
		class UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat")
		class UInputAction* LightAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat")
		class UInputAction* HeavyAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ability")
		class UInputAction* Ability1Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ability")
		class UInputAction* Ability2Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ability")
		class UInputAction* Ability3Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Ability")
		class UInputAction* Ability4Action;

protected:
	virtual void SetupInputComponent() override;

private:
	// Member variables
	// ----------------

	// Events
	FMoveEvent m_MoveEvent;
	FMoveStopEvent m_MoveStopEvent;

	FJumpEvent m_JumpEvent;
	FJumpHoldEvent m_JumpHoldEvent;
	FJumpStopEvent m_JumpStopEvent;

	FDashEvent m_DashEvent;

	FLightAttackEvent m_LightAttackEvent;
	FHeavyAttackEvent m_HeavyAttackEvent;

	FAbility1Event m_Ability1Event;
	FAbility1StopEvent m_Ability1StopEvent;

	FAbility2Event m_Ability2Event;
	FAbility2StopEvent m_Ability2StopEvent;

	FAbility3Event m_Ability3Event;
	FAbility3StopEvent m_Ability3StopEvent;

	FAbility4Event m_Ability4Event;
	FAbility4StopEvent m_Ability4StopEvent;
};