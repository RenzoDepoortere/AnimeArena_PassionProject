// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Goku_Character.generated.h"

/**
 * 
 */
UCLASS()
class ANIMEARENA_API AGoku_Character : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	AGoku_Character();

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
