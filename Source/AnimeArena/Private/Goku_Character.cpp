// Fill out your copyright notice in the Description page of Project Settings.
#include "Goku_Character.h"
#include "../BasePlayerController.h"
#include "StateMachineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kamehameha_Ability.h"
#include "BasePlayerState.h"

AGoku_Character::AGoku_Character()
	: ABaseCharacter()
	, KamehamehaAttack{}
	, KamehamehaBeam{}
	, KamehamehaDamageFrequency{ 0.1f }
	, KamehamehaMovementSpeed{ 10.f }
	, KamehamehaMaxDistance{ 100.f }
	, KamehamehaExistTime{ 2.5f }
	, KamehamehaDisappearSpeed{ 0.1f }
	, KamehamehaMaterial{ nullptr }
	, KamehamehaAnimationStopTime{}
	, TimeToReachMaxKamehameha{ 3.f }
	, MaxKamehamehaScale{ 3.f }
	, KamehamehaFlySpeed{ 300 }
	, m_VerticalFlightInput{}
	, m_ShouldStopVelocity{ false }
{
}

void AGoku_Character::BeginPlay()
{
	ABaseCharacter::BeginPlay();

	// Abilities
	// ---------

	// Kamehameha
	auto pKamehamehaAbility{ NewObject<UKamehameha_Ability>(this, TEXT("Kamehameha")) };
	pKamehamehaAbility->SetCharacter(this);
	pKamehamehaAbility->MaxCooldownTimer = Ability1_Cooldown;

	Abilities.Add(pKamehamehaAbility);

	// Kaioken

	// Instant transmission

	// Rush-Grab


	// Subscribe to events
	// -------------------

	// StateSwitch
	StateMachineComponent->OnStateSwitch.AddDynamic(this, &AGoku_Character::OnStateSwitch);

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

	// StateSwitch
	StateMachineComponent->OnStateSwitch.RemoveAll(this);

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

	if (m_ShouldStopVelocity)
	{
		auto pCharMovement{ GetCharacterMovement() };
		pCharMovement->Velocity.Z = 0;
	}
}

bool AGoku_Character::GetIsInAir() const
{
	const bool isFalling{ ABaseCharacter::GetIsInAir() };
	const bool isFlying{ StateMachineComponent->PreviousState == "Flying" };

	return isFlying || isFalling;
}

void AGoku_Character::OnStateSwitch(const FString& newState)
{
	// Handle attacking in air
	// -----------------------

	if (newState == "Idle" && StateMachineComponent->CurrentState->StateDisplayName == "Attack" && StateMachineComponent->PreviousState == "Flying")
	{
		// Go to flyState instead
		StateMachineComponent->SwitchStateByKey("AirOption");
	}
	else
	{
		m_ShouldStopVelocity = false;
	}

	// Handle getting hit
	// ------------------

	if (newState != "Hit") return;
	
	// Stop kamehameha
	if (Abilities[0]->GetIsActive())
	{
		auto pKamehameha{ Cast<UKamehameha_Ability>(Abilities[0]) };
		pKamehameha->AbilityEnd();
	}
}

void AGoku_Character::Ability1()
{
	// Check if is in cooldown
	if (0 < Abilities[0]->CurrentCooldown) return;

	// Check if is active
	if (Abilities[0]->GetIsActive()) return;

	auto pCurrentState{ Cast<UBasePlayerState>(StateMachineComponent->CurrentState) };

	// Check if currentState can be canceled
	if (pCurrentState->GetExtraStateInfo()->canBeAttackCanceled == false) return;

	// Activate ability
	Abilities[0]->ActivateAbility();
}
void AGoku_Character::Ability1Stop()
{
	// Check if is active
	if (Abilities[0]->GetIsActive() == false) return;

	// Stop ability
	Abilities[0]->StopAbility();
}

void AGoku_Character::BeamStop()
{
	// Check if is active
	if (Abilities[0]->GetIsActive() == false) return;

	// End ability
	auto pKamehameha{ Cast<UKamehameha_Ability>(Abilities[0]) };
	pKamehameha->AbilityEnd();
}