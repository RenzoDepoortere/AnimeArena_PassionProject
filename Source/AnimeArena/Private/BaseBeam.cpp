// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseBeam.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

ABaseBeam::ABaseBeam()
	: m_pCharacter{ nullptr }
{
	PrimaryActorTick.bCanEverTick = true;

	// Components
	// ----------

	// Default scene root
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRoot->SetupAttachment(RootComponent);
	SetRootComponent(DefaultSceneRoot);

	// Collision
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	CapsuleCollision->SetupAttachment(DefaultSceneRoot);

	// Beam
	Beam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Beam"));
	Beam->SetupAttachment(CapsuleCollision);
}

void ABaseBeam::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

