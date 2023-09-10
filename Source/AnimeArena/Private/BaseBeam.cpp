// Fill out your copyright notice in the Description page of Project Settings.
#include "BaseBeam.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
//#include "../BaseCharacter.h"

ABaseBeam::ABaseBeam()
	: Damage{}
	, DamageFrequency{ 0.1f }
	, MovementSpeed{ 0.1f }
	, MaxDistance{ 500 }
	, MaxExistTime{ 2.5f }
	, DisappearSpeed{ 0.1f }
	, BeamMaterial{ nullptr }
	, m_pCharacter{ nullptr }
	, m_CurrentDamageTime{}
	, m_CanMove{ true }
	, m_MovedDistance{}
	, m_CharWasHit{ false }
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
	if (m_CurrentDamageTime <= 0 && 0 < m_CurrentExistTime) DealDamage();

	if (m_CanMove) Move(DeltaTime);
	
	if (0 < m_CurrentExistTime)
	{
		m_CurrentExistTime -= DeltaTime;
		if (m_CurrentExistTime <= 0) StopMove();
	}

	if (m_CurrentExistTime <= 0) ScaleDown(DeltaTime);
}

void ABaseBeam::SetVariables()
{
	// Set material
	Beam->SetMaterial(0, BeamMaterial);

	// Set currentExistTime to max
	m_CurrentExistTime = MaxExistTime;
}

void ABaseBeam::StopMove(bool charWasHit)
{
	m_CanMove = false;
	m_CharWasHit = charWasHit;

	// Detach from owner
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// Stop ability of owner
	if (m_CharWasHit) return;
	//m_pCharacter->BeamStop();
}

void ABaseBeam::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	// Check if otherActor has Wall tag
	if (OtherActor->ActorHasTag("Wall")) StopMove();
}

void ABaseBeam::DealDamage()
{
	//m_CurrentDamageTime = DamageFrequency;

	//// Loop through overlapping actors
	//TArray<AActor*> overlappingActors{};
	//CapsuleCollision->GetOverlappingActors(overlappingActors, ABaseCharacter::StaticClass());
	//for (const auto& currentChar : overlappingActors)
	//{
	//	// Deal damage if not self
	//	if (currentChar == m_pCharacter) continue;
	//	Cast<ABaseCharacter>(currentChar)->DealDamage(Damage, m_pCharacter);
	//}
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
	if (MaxDistance <= m_MovedDistance) m_CanMove = false;
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