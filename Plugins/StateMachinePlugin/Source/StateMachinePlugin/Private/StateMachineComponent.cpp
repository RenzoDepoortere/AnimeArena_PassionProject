// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachineComponent.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
	: m_HasToSwitchState{ false }
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

	// Store basePointers to states
	// ----------------------------
	for (const auto& currentState : States)
	{
		UStateBase* State = NewObject<UStateBase>(this, currentState.Value);
		m_StateMap.Add(currentState.Key, State);
	}

	// Set initState
	// -------------

	// Get initState
	UStateBase* pNewState = m_StateMap.FindRef(InitialState);
	if (pNewState->IsValidLowLevel() == false || pNewState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s initial state set failed. " + "Invalid state!");
		return;
	}

	// Set currentState
	PreviousState = InitialState;

	CurrentState = pNewState;
	CurrentState->OnEnter(GetOwner());
}


// Called every frame
void UStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Return if no currentState
	if (CurrentState == nullptr) return;

	// Update
	CurrentState->Tick(DeltaTime);

	// Cooldown
	for (const auto& currentState : m_StateMap)
	{
		if (currentState.Value->GetHasCooldown()) currentState.Value->Cooldown(DeltaTime);
	}

	// Check if needs to switch state
	if (m_HasToSwitchState)
	{
		m_HasToSwitchState = false;

		// Switch state
		PreviousState = m_NewStateKey;

		CurrentState->OnExit();
		CurrentState = m_pNewState;
		CurrentState->OnEnter(GetOwner());
	}
}

void UStateMachineComponent::SwitchStateByKey(const FString& stateKey)
{
	// Get state
	UStateBase* pNewState = m_StateMap.FindRef(stateKey);
	if (pNewState->IsValidLowLevel() == false || pNewState == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed. " + "Invalid state!");
		return;
	}

	// Check if state in cooldown
	if (pNewState->GetHasCooldown() && 0 < pNewState->GetCurrentCooldown())
	{
		// Sound queu?


		return;
	}

	// Store state
	m_HasToSwitchState = true;

	m_NewStateKey = stateKey;
	m_pNewState = pNewState;
}