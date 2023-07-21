// Fill out your copyright notice in the Description page of Project Settings.
#include "Kamehameha_Ability.h"
#include "Goku_Character.h"
#include "StateMachineComponent.h"

UKamehameha_Ability::UKamehameha_Ability()
	: UBaseAbility()
{
}

void UKamehameha_Ability::Update(float deltaTime)
{
	UBaseAbility::Update(deltaTime);
}

void UKamehameha_Ability::ActivateAbility()
{
	UBaseAbility::ActivateAbility();

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Purple, "Ability start");

	// Blacklist attackState
	auto pCharacter{ GetCharacter() };
	pCharacter->StateMachineComponent->BlacklistKey("Attack");
}
void UKamehameha_Ability::StopAbility()
{

}

void UKamehameha_Ability::AbilityEnd()
{
	// Remove attackState from blacklist
	auto pCharacter{ GetCharacter() };
	pCharacter->StateMachineComponent->RemoveKeyFromBlacklist("Attack");
}