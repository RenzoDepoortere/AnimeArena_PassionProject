// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateBase.h"
#include "StateMachineComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateSwitch, const FString&, stateKey);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STATEMACHINEPLUGIN_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateMachineComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Properties
	// ----------

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = StateMachine)
	TMap<FString, TSubclassOf<UStateBase>> States;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = StateMachine)
	FString InitialState;

	UPROPERTY()
	TMap<FString, UStateBase*> StateMap;

	UPROPERTY(BlueprintReadOnly)
	UStateBase* CurrentState = nullptr;
	UPROPERTY(BluePrintReadOnly)
	FString PreviousState;

	UPROPERTY(BlueprintAssignable, Category = StateMachine)
	FOnStateSwitch OnStateSwitch;

	// Functions
	// ---------

	UFUNCTION(BlueprintCallable, Category = StateMachine)
	void SwitchStateByKey(const FString& stateKey);

	UFUNCTION(BlueprintCallable, Category = StateMachine)
		void BlacklistKey(const FString& stateKey) { m_Blacklist.Add(stateKey); }
	UFUNCTION(BlueprintCallable, Category = StateMachine)
		void RemoveKeyFromBlacklist(const FString& stateKey) { m_Blacklist.Remove(stateKey); }

private:
	// Member variables
	bool m_HasToSwitchState;
	UStateBase* m_pNewState;
	FString m_NewStateKey;

	TArray<FString> m_Blacklist;
};
