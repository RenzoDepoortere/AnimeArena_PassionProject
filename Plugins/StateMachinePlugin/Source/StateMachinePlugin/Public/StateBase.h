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
	virtual void OnEnter(AActor* pStateOwner);
	virtual void OnExit() {}
	virtual void Tick(float deltaTime) {}

	void Cooldown(float deltaTime);

public:
	bool GetHasCooldown() const { return m_HasCooldown; }
	float GetCurrentCooldown() const { return m_CurrentCooldown; }

public:
	// Properties
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName StateDisplayName;

protected:
	AActor* GetStateOwner() const { return m_pStateOwner; }

	void SetHasCooldown(bool hasCooldown) { m_HasCooldown = hasCooldown; }
	float GetMaxCooldown() const { return m_MaxCooldown; }
	void SetMaxCooldown(float maxCooldown) { m_MaxCooldown = maxCooldown; }

private:
	// Member variables
	AActor* m_pStateOwner{ nullptr };

	bool m_HasCooldown{ false };
	float m_MaxCooldown{};
	float m_CurrentCooldown{};
};
