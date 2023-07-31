// Fill out your copyright notice in the Description page of Project Settings.
#include "StateMachineComponent.h"
#include "GameFramework/Character.h"

UStateMachineComponent::UStateMachineComponent()
	: States{}
	, InitialState{}
	, StateMap{}
	, CurrentState{ nullptr }
	, PreviousState{}
	, OnStateSwitch{}
	, m_HasToSwitchState{ false }
	, m_pNewState{ nullptr }
	, m_NewStateKey{}
	, m_Blacklist{}
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

void UStateMachineComponent::BeginPlay()
{
	Super::BeginPlay();

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

		PreviousState = CurrentState->StateDisplayName.ToString();

		// Switch state
		CurrentState->OnExit();
		CurrentState = m_pNewState;
		CurrentState->OnEnter(GetOwner());
	}
}

void UStateMachineComponent::SwitchStateByKey(const FString& stateKey)
{
	// Check if key in blackList
	for (const auto& currentKey : m_Blacklist)
	{
		if (currentKey == stateKey) return;
	}

	// Get state
	UStateBase* pNewState = StateMap.FindRef(stateKey);
	if (pNewState->IsValidLowLevel() == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "State switch failed. Invalid state!");
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Tried to change from " + CurrentState->StateDisplayName.ToString() + " to " + stateKey);
		return;
	}

	// Check if state in cooldown
	if (pNewState->GetHasCooldown() && 0 < pNewState->GetCurrentCooldown())
	{
		// Sound queu?


		return;
	}

	// Notify server
	ServerSwitchStateByKey(stateKey);
}
void UStateMachineComponent::ServerSwitchStateByKey_Implementation(const FString& stateKey)
{
	MulticastSwitchStateByKey(stateKey);
}
void UStateMachineComponent::MulticastSwitchStateByKey_Implementation(const FString& stateKey)
{
	// Get state
	UStateBase* pNewState = StateMap.FindRef(stateKey);

	// Store state
	m_HasToSwitchState = true;

	m_NewStateKey = stateKey;
	m_pNewState = pNewState;

	// Send switchEvent
	OnStateSwitch.Broadcast(stateKey);
}

void UStateMachineComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}