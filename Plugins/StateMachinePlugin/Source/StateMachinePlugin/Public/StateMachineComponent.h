// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateBase.h"
#include "StateMachineComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STATEMACHINEPLUGIN_API UStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateMachineComponent();

protected:
	// Called when the game starts	
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Properties
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "StateMachine")
	TMap<FString, TSubclassOf<UStateBase>> States;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "StateMachine")
	FString InitialState;

	UPROPERTY()
	TMap<FString, UStateBase*> StateMap;
	
	UPROPERTY(BlueprintReadOnly)
	UStateBase* CurrentState = nullptr;
	UPROPERTY(BluePrintReadOnly)
	FString PreviousState;

	UFUNCTION(BlueprintCallable, Category = "StateMachine")
	void SwitchStateByKey(const FString& stateKey);

private:
	// Member variables
	bool m_HasToSwitchState;
	UStateBase* m_pNewState;
	FString m_NewStateKey;

	FTimerHandle m_TimerHandle;

	// Member functions
	void LateStart();
};
