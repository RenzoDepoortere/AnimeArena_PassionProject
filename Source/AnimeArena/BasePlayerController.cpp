// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ABasePlayerController::ABasePlayerController()
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
		enhancedInputComponent->BindAction(Ability1Action, ETriggerEvent::Completed, this, &ABasePlayerController::Ability1Stop);

		enhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability2);
		enhancedInputComponent->BindAction(Ability2Action, ETriggerEvent::Completed, this, &ABasePlayerController::Ability2Stop);

		enhancedInputComponent->BindAction(Ability3Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability3);
		enhancedInputComponent->BindAction(Ability3Action, ETriggerEvent::Completed, this, &ABasePlayerController::Ability3Stop);

		enhancedInputComponent->BindAction(Ability4Action, ETriggerEvent::Started, this, &ABasePlayerController::Ability4);
		enhancedInputComponent->BindAction(Ability4Action, ETriggerEvent::Completed, this, &ABasePlayerController::Ability4Stop);
	}
}