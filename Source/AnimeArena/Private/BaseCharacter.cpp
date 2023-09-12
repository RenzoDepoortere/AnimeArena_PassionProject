#include "BaseCharacter.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "StateMachineComponent.h"
#include "Components/BoxComponent.h"

#include "../BasePlayerController.h"
#include "EnhancedInputSubsystems.h"
#include <Kismet/GameplayStatics.h>

ABaseCharacter::ABaseCharacter()
	// Components
	// ----------

	// Base
	: Collision{}
	, Mesh{}
	// Camera
	, SpringArm{}
	, Camera{}
	// Other
	, StateMachineComponent{}

	// Variables
	// ---------

	// Input
	, DefaultMappingContext{}
	, LookAction{}
	// Movement
	, MaxMovementSpeed{ 1000.f }
	, MoveAccelerationTime{ 0.2f }
	, JumpSpeed{ 500.f }
	, MaxJumpTime{ 0.2f }
	, MaxFallSpeed{ 981.f }
	, FallAccelerationTime{ 0.5f }
	, RotationSpeed{ 20.f }
	// Other
	, CameraRotationSpeed{ 1.f }

	// Privates
	// --------

	// Base
	, m_pController{}
	// Movement
	, m_LastMovementInput{}
	, m_IsInAir{ true }
	, m_ShouldFall{ true }
	, m_TotalVelocity{}
	, m_LandEvent{}
{
 	// Settings
	// --------
	PrimaryActorTick.bCanEverTick = true;

	// Components
	// ----------

	// Base
	// ====

	// Collision
	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName("Pawn");

	RootComponent = Collision;

	// Ground collision
	GroundCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	GroundCollision->SetupAttachment(RootComponent);
	GroundCollision->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnGroundBeginOverlap);
	GroundCollision->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::OnGroundEndOverlap);


	// Mesh
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName("CharacterMesh");

	// Camera
	// ======

	// SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Create a follow camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// Other
	// =====

	// StateMachine
	StateMachineComponent = CreateDefaultSubobject<UStateMachineComponent>(TEXT("StateMachine"));

}
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Get controller
	m_pController = Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0));

	// Add Input Mapping Context
	// ------------------------
	if (m_pController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* pSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(m_pController->GetLocalPlayer()))
		{
			pSubsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Subscribe to events
	// -------------------

	// Input
	if (m_pController)
	{
		m_pController->GetMoveEvent()->AddUObject(this, &ABaseCharacter::Move);
		m_pController->GetMoveStopEvent()->AddUObject(this, &ABaseCharacter::StopMove);
	}
}
void ABaseCharacter::Destroyed()
{
	// Unsubscribe from events
	// -----------------------

	// Input
	if (m_pController)
	{
		m_pController->GetMoveEvent()->RemoveAll(this);
		m_pController->GetMoveStopEvent()->RemoveAll(this);
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleGravity(DeltaTime);
	HandleDisplacement(DeltaTime);
}
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Looking
		enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Look);
		//enhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, this, &ABaseCharacter::LockToggle);
	}
}

void ABaseCharacter::Look(const FInputActionValue& value)
{
	// Input is a Vector2D
	FVector2D lookAxisVector = value.Get<FVector2D>();
	lookAxisVector *= CameraRotationSpeed;

	if (Controller != nullptr)
	{
		// Add yaw and pitch input to controller
		AddControllerYawInput(lookAxisVector.X);
		AddControllerPitchInput(lookAxisVector.Y);
	}
}

void ABaseCharacter::HandleGravity(float deltaTime)
{
	// Check conditions
	if (m_ShouldFall == false) return;

	// Apply gravity
	// -------------
	
	// Air
	if (m_IsInAir)
	{
		// Decrease Z component
		const float fallAcceleration{ MaxFallSpeed / FallAccelerationTime };
		m_TotalVelocity.Z -= fallAcceleration * deltaTime;

		// Make sure not going faster then max
		if (m_TotalVelocity.Z < -MaxFallSpeed) m_TotalVelocity.Z = -MaxFallSpeed;
	}
	// Grounded
	else
	{
		m_TotalVelocity.Z = 0.f;
	}
}
void ABaseCharacter::HandleDisplacement(float deltaTime)
{
	const FVector displacement{ m_TotalVelocity * deltaTime };
	AddActorWorldOffset(displacement, true);
}

void ABaseCharacter::OnGroundBeginOverlap(	UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
											int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	// Check for ground
	// ----------------
	const FName floorTag{ "Floor" };
	if (otherComp->ComponentHasTag(floorTag))
	{
		m_IsInAir = false;

		// Send event
		if (m_LandEvent.IsBound()) m_LandEvent.Broadcast();
	}
}
void ABaseCharacter::OnGroundEndOverlap(	UPrimitiveComponent* overlappedComp, AActor* otherActor, UPrimitiveComponent* otherComp,
											int32 otherBodyIndex)
{
	// Check for ground
	// ----------------
	const FName floorTag{ "Floor" };
	if (otherComp->ComponentHasTag(floorTag))
	{
		m_IsInAir = true;
	}
}