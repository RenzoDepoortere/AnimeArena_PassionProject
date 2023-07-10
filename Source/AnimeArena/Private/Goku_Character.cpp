// Fill out your copyright notice in the Description page of Project Settings.
#include "Goku_Character.h"
#include "../BasePlayerController.h"

AGoku_Character::AGoku_Character()
	: ABaseCharacter()
	, m_VerticalFlightInput{}
{
}

void AGoku_Character::BeginPlay()
{
	ABaseCharacter::BeginPlay();

	// Subscribe to inputEvents
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

	// Unsubscribe to inputEvents
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
}

void AGoku_Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	ABaseCharacter::SetupPlayerInputComponent(PlayerInputComponent);
}

void AGoku_Character::FlyUp()
{
	m_VerticalFlightInput = 1;
}
void AGoku_Character::StopFlyUp()
{
	m_VerticalFlightInput = 0;
}

void AGoku_Character::FlyDown()
{
	m_VerticalFlightInput = -1;
}
void AGoku_Character::StopFlyDown()
{
	m_VerticalFlightInput = 0;
}