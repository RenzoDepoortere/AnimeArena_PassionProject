// Fill out your copyright notice in the Description page of Project Settings.
#include "HealthComponent.h"
#include "../BaseCharacter.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set currentHealth to max
	CurrentHealth = MaxHealth;
}

void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthComponent::SetHealth(float amount)
{
	CurrentHealth = amount;
	if (MaxHealth < CurrentHealth) CurrentHealth = MaxHealth;
}
bool UHealthComponent::DealDamage(float amount, ABaseCharacter* pDamageDealer)
{
	CurrentHealth -= amount;
	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;

		// Send deathEvent
		OnDeath.Broadcast(pDamageDealer);
	}

	// Send damageEvent
	OnDamage.Broadcast(pDamageDealer);

	return true;
}