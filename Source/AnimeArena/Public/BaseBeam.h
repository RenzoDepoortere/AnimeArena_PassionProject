// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBeam.generated.h"

class ABaseCharacter;
class UCapsuleComponent;
class USphereComponent;

UCLASS()
class ANIMEARENA_API ABaseBeam : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseBeam();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCapsuleComponent* CapsuleCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Beam;

public:
	void SetCharacter(ABaseCharacter* pCharacter) { m_pCharacter = pCharacter; }

private:
	// Member variables
	// ----------------
	ABaseCharacter* m_pCharacter;
};
