// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ABasePlayerController::Sprint);
		enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABasePlayerController::StopSprinting);

		// Jumping
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Jump);
		enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABasePlayerController::StopJumping);

		// Looking
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Look);
	}
}

void ABasePlayerController::Move(const FInputActionValue& value)
{

}
void ABasePlayerController::StopMoving(const FInputActionValue& value)
{

}

void ABasePlayerController::Sprint(const FInputActionValue& value)
{

}
void ABasePlayerController::StopSprinting(const FInputActionValue& value)
{

}

void ABasePlayerController::Jump(const FInputActionValue& value)
{
	
}
void ABasePlayerController::StopJumping(const FInputActionValue& value)
{

}

void ABasePlayerController::Look(const FInputActionValue& value)
{

}