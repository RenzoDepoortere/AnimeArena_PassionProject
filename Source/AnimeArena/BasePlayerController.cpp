// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ABasePlayerController::ABasePlayerController()
	: m_LastSprintTapTime{}
{
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
		enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABasePlayerController::StopMoving);

		// Sprinting
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABasePlayerController::SprintTap);
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Sprint);
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABasePlayerController::StopSprinting);

		// Jumping
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABasePlayerController::Jump);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABasePlayerController::JumpHold);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABasePlayerController::StopJumping);

		// Dashing
		enhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ABasePlayerController::Dash);

		// Attacks
		enhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Started, this, &ABasePlayerController::LightAttack);
		enhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ABasePlayerController::HeavyAttack);

		// Abilities
		enhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability1);
		enhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability2);
		enhancedInputComponent->BindAction(Ability3Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability3);
		enhancedInputComponent->BindAction(Ability4Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability4);
	}
}

void ABasePlayerController::SprintTap(const FInputActionValue& /*value*/)
{
	// Single tap
	if (m_SprintTapEvent.IsBound())
	{
		m_SprintTapEvent.Broadcast();
	}

	// Double tap
	if (m_SprintDoubleTapEvent.IsBound())
	{
		const float tapDelay{ 0.3f };
		const float currentTime{ static_cast<float>(GetWorld()->GetRealTimeSeconds()) };

		// Check if doubleTapped
		if (currentTime - m_LastSprintTapTime <= tapDelay)
		{
			// Send event
			m_SprintDoubleTapEvent.Broadcast();
		}

		// Store time
		m_LastSprintTapTime = currentTime;
	}
}