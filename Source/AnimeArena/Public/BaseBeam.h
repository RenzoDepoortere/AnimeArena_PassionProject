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
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		USceneComponent* DefaultSceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UCapsuleComponent* CapsuleCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
		UStaticMeshComponent* Beam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Damage")
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Damage")
		float DamageFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		float MovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Movement")
		float MaxDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Other")
		float DisappearSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Other")
		UMaterialInstance* BeamMaterial;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Beam")
		void StopMove();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void SetCharacter(ABaseCharacter* pCharacter) { m_pCharacter = pCharacter; }

private:
	// Member variables
	// ----------------
	ABaseCharacter* m_pCharacter;
	float m_CurrentDamageTime;
	bool m_CanMove;
	float m_MovedDistance;

	// Member functions
	// ----------------
	void DealDamage();
	void Move(float deltaTime);
	void ScaleDown(float deltaTime);
};
