// Fill out your copyright notice in the Description page of Project Settings.
#include "Goku_Character.h"

AGoku_Character::AGoku_Character()
	: ABaseCharacter()
{
}

void AGoku_Character::BeginPlay()
{
	ABaseCharacter::BeginPlay();
}
void AGoku_Character::Destroyed()
{
	ABaseCharacter::Destroyed();
}

void AGoku_Character::Tick(float DeltaTime)
{
	ABaseCharacter::Tick(DeltaTime);
}

void AGoku_Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	ABaseCharacter::SetupPlayerInputComponent(PlayerInputComponent);
}