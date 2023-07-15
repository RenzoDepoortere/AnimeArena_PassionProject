// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class ABaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage, ABaseCharacter*, damageDealer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, ABaseCharacter*, killer);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANIMEARENA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
		float MaxHealth;
	UPROPERTY(BlueprintReadOnly, Category = Settings)
		float CurrentHealth;

	UPROPERTY(BlueprintAssignable, Category = Event)
		FOnDamage OnDamage;

	UPROPERTY(BlueprintAssignable, Category = Event)
		FOnDeath OnDeath;

	UFUNCTION(BlueprintCallable, Category = Health)
		void SetHealth(float amount);

	UFUNCTION(BlueprintCallable, Category = Health)
		bool DealDamage(float amount, ABaseCharacter* pDamageDealer);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
