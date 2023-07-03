// Fill out your copyright notice in the Description page of Project Settings.
#include "StateMachineComponent.h"

// Sets default values for this component's properties
UStateMachineComponent::UStateMachineComponent()
	: m_HasToSwitchState{ false }
	, m_TimerHandle{}
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

	// Late Start
	GetOwner()->GetWorldTimerManager().SetTimer(m_TimerHandle, this, &UStateMachineComponent::LateStart, 1.f, false);
}
void UStateMachineComponent::LateStart()
{
	// Store basePointers to states
	// ----------------------------
	for (auto it = States.CreateConstIterator(); it; ++it)
	{
		UStateBase* state = NewObject<UStateBase>(this, it->Value, FName{ it->Key });
		StateMap.Add(it->Key, state);
	}

	// Set initState
	// -------------

	// Get initState
	UStateBase* pNewState = StateMap.FindRef(InitialState);
	if (pNewState->IsValidLowLevel() == false)
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
	for (const auto& currentState : StateMap)
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
	UStateBase* pNewState = StateMap.FindRef(stateKey);
	if (pNewState->IsValidLowLevel() == false)
	{
 		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, this->GetOwner()->GetName() + "'s state switch failed. Invalid state!");
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Tried to change from " + CurrentState->StateDisplayName.ToString() + " to " + stateKey);
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