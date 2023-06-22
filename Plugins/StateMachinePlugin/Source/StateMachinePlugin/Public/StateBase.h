// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StateBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class STATEMACHINEPLUGIN_API UStateBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* pStateOwner) { m_pStateOwner = pStateOwner; }
	virtual void OnExit() {}
	virtual void Tick(float deltaTime) {}

public:
	// Properties
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName StateDisplayName;

protected:
	AActor* GetStateOwner() const { return m_pStateOwner; }

private:
	// Member variables
	AActor* m_pStateOwner{ nullptr };
};
