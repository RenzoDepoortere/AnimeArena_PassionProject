// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseBeam.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

ABaseBeam::ABaseBeam()
	: Damage{}
	, DamageFrequency{ 0.1f }
	, MovementSpeed{ 0.1f }
	, MaxDistance{ 500 }
	, DisappearSpeed{ 0.1f }
	, m_pCharacter{ nullptr }
	, m_CurrentDamageTime{}
	, m_CanMove{ true }
	, m_MovedDistance{}
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
	Beam->SetupAttachment(DefaultSceneRoot);
}

void ABaseBeam::BeginPlay()
{
	Super::BeginPlay();

	// Set material
	Beam->SetMaterial(0, BeamMaterial);

	// Subscribe to beginOverlap
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseBeam::OnOverlap);
}
void ABaseBeam::Destroyed()
{
	// Unsubscribe to beginOverlap
	CapsuleCollision->OnComponentBeginOverlap.RemoveAll(this);
}

void ABaseBeam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	m_CurrentDamageTime -= DeltaTime;
	if (m_CurrentDamageTime <= 0) DealDamage();

	if (m_CanMove) Move(DeltaTime);
	else		   ScaleDown(DeltaTime);
}

void ABaseBeam::StopMove()
{
	m_CanMove = false;

	// Stop ability of owner
}

void ABaseBeam::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	// Check if otherActor has Wall tag
	if (OtherActor->ActorHasTag("Wall")) StopMove();
}

void ABaseBeam::DealDamage()
{
	m_CurrentDamageTime = DamageFrequency;

}
void ABaseBeam::Move(float deltaTime)
{
	// Calculate movement
	const float movement{ MovementSpeed * deltaTime };

	// Scale
	FVector scale{ GetActorScale3D() };
	scale.Z += movement;
	SetActorScale3D(scale);

	// Check if exceeded treshold
	m_MovedDistance += movement;
	if (MaxDistance <= m_MovedDistance) StopMove();
}
void ABaseBeam::ScaleDown(float deltaTime)
{
	// Calculate movement
	const float movement{ DisappearSpeed * deltaTime };

	// Scale
	FVector scale{ GetActorScale3D() };
	scale.X -= movement;
	scale.Y -= movement;
	SetActorScale3D(scale);

	// Check if exceeded treshold
	if (scale.X <= 0) Destroy();
}