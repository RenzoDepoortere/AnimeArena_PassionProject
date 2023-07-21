// Fill out your copyright notice in the Description page of Project Settings.
#include "Goku_Character.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kamehameha_Ability.h"

AGoku_Character::AGoku_Character()
	: ABaseCharacter()
	, m_VerticalFlightInput{}
	, m_WasFlying{ false }
	, m_PreviousState{}
{
}

void AGoku_Character::BeginPlay()
{
	ABaseCharacter::BeginPlay();

	// Abilities
	// ---------

	// Kamehameha
	auto pKamehamehaAbility{ CreateDefaultSubobject<UKamehameha_Ability>(TEXT("Kamehameha")) };
	pKamehamehaAbility->SetCharacter(this);
	pKamehamehaAbility->MaxCooldownTimer = 10.f;

	Abilities.Add(pKamehamehaAbility);

	// Kaioken

	// Instant transmission

	// Grab


	// Subscribe to events
	// -------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetJumpHoldEvent()->AddUObject(this, &AGoku_Character::FlyUp);
		pController->GetJumpStopEvent()->AddUObject(this, &AGoku_Character::StopFlyUp);

		pController->GetSprintEvent()->AddUObject(this, &AGoku_Character::FlyDown);
		pController->GetSprintStopEvent()->AddUObject(this, &AGoku_Character::StopFlyDown);
	}
}
void AGoku_Character::Destroyed()
{
	ABaseCharacter::Destroyed();

	// Unsubscribe from events
	// -----------------------

	// Input
	auto pController{ GetPlayerController() };
	if (pController)
	{
		pController->GetJumpHoldEvent()->RemoveAll(this);
		pController->GetJumpStopEvent()->RemoveAll(this);

		pController->GetSprintEvent()->RemoveAll(this);
		pController->GetSprintStopEvent()->RemoveAll(this);
	}
}

void AGoku_Character::Tick(float DeltaTime)
{
	ABaseCharacter::Tick(DeltaTime);

	// Manual onStateSwitch (not optimal)
	if (m_PreviousState != StateMachineComponent->CurrentState->StateDisplayName) OnStateSwitch(StateMachineComponent->CurrentState->StateDisplayName.ToString());
	m_PreviousState = StateMachineComponent->CurrentState->StateDisplayName;
}

void AGoku_Character::OnStateSwitch(const FString& newState)
{
	const FString attackString{ "Attack" };
	const FString flyString{ "Flying" };

	// If switching from attack to idle and was flying before, go to flying instead
	if (StateMachineComponent->PreviousState == attackString && m_WasFlying)
	{
		StateMachineComponent->SwitchStateByKey("AirOption");
		m_WasFlying = false;
	}
	// If is going to attack and was flying, let player fly
	else if (StateMachineComponent->PreviousState == flyString && newState == attackString)
	{
		auto pCharMovement{ GetCharacterMovement() };
		pCharMovement->MovementMode = EMovementMode::MOVE_Flying;
		m_WasFlying = true;
	}
	else
	{
		m_WasFlying = false;
	}
}

void AGoku_Character::Ability1()
{
	ABaseCharacter::Ability1();
}