// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BaseCharacter.generated.h"

UCLASS()
class ANIMEARENA_API ABaseCharacter : public APawn
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	// Input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Components
	// ----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UCapsuleComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMesh* Mesh;

protected:
	virtual void BeginPlay() override;


};
