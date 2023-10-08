#include "KinematicController.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/GameplayStatics.h>

UKinematicController::UKinematicController()
	// Variables
	// ---------

	// General
	: RotationSpeed{ 20.f }

	// Grounded
	, MaxMovementSpeed{ 1000.f }
	, MoveAccelerationTime{ 0.2f }
	, MaxSlopeAngle{ 60.f }

	// Air
	, JumpSpeed{ 500.f }
	, MaxJumpTime{ 0.2f }
	, AirControl{ 0.5f }
	, MaxFallSpeed{ 981.f }
	, FallAccelerationTime{ 0.5f }

	// Collision
	, SkinWidth{ 0.1f }
	, GroundCollisionLength{ 10.f }
	, WallCollisionLength{ 15.f }

	// Privates
	// --------

	// General
	, m_pActor{ nullptr }
	, m_pCollision{ nullptr }
	, m_pController{ nullptr }
	, m_TotalVelocity{}
	, m_CurrentDirection{}
	, m_DesiredRotation{}

	// Grounded
	, m_MoveSpeed{}
	, m_ShouldMove{ false }

	// Air
	, m_IsInAir{ false }
	, m_ShouldFall{ true }

	// Events
	, m_LandEvent{}
	, m_NoMovementEvent{}
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UKinematicController::BeginPlay()
{
	Super::BeginPlay();


	// Get components
	// --------------
	m_pActor = GetOwner();
	if (!m_pActor)
	{
		PrimaryComponentTick.bCanEverTick = false;
		return;
	}

	m_pCollision = Cast<UCapsuleComponent>(m_pActor->FindComponentByClass(UCapsuleComponent::StaticClass()));
	m_pController = UGameplayStatics::GetPlayerController(this, 0);
}

void UKinematicController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckGround(DeltaTime);

	HandleGravity(DeltaTime);
	HandleMovement(DeltaTime);
	
	HandleWallCollision(DeltaTime);
	HandleDisplacement(DeltaTime);
}

void UKinematicController::RotateCharacter(const FVector2D& input)
{
	if (input.IsNearlyZero()) return;
	if (!m_pController) return;

	// Get rotations
	const FRotator rotation = m_pController->GetControlRotation();
	const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	// Get directions
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// Calculate direction
	FVector currentDirection{ forwardDirection * input.Y + rightDirection * input.X };
	currentDirection.Normalize();

	// Calculate desiredRotation
	// -------------------------
	m_DesiredRotation = FRotationMatrix::MakeFromX(currentDirection).Rotator();
}
void UKinematicController::MoveCharacter(const FVector2D& input, bool rotateCharacter)
{
	if (!m_pController) return;

	// Set variables
	// -------------
	m_ShouldMove = true;

	// Calculate direction
	// -------------------

	// Get rotations
	const FRotator rotation = m_pController->GetControlRotation();
	const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	// Get directions
	const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	// Calculate direction
	m_CurrentDirection = forwardDirection * input.Y + rightDirection * input.X;
	m_CurrentDirection.Normalize();

	// Calculate desiredRotation
	// -------------------------
	if (rotateCharacter) m_DesiredRotation = FRotationMatrix::MakeFromX(m_CurrentDirection).Rotator();
}

void UKinematicController::CheckGround(float deltaTime)
{
	const bool previousAirState{ m_IsInAir };

	// Raycast params
	// --------------

	// Hitresult
	FHitResult hitResult{};

	// Position
	const FVector startPos{ m_pActor->GetActorLocation() };
	const FVector endPos{ startPos + FVector::DownVector * (GroundCollisionLength + SkinWidth) };

	// Params
	const FCollisionShape capsuleShape{ m_pCollision->GetCollisionShape(SkinWidth) };
	const FCollisionQueryParams traceParam{ "Raycast", true, m_pActor };

	// Raycast
	// -------
	if (GetWorld()->SweepSingleByChannel(hitResult, startPos, endPos, FQuat::Identity, ECollisionChannel::ECC_Visibility, capsuleShape, traceParam))
	{
		const double surfaceAngle{ FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, hitResult.Normal))) };

		// On ground or slope
		if (surfaceAngle <= MaxSlopeAngle)	m_IsInAir = false;
		// On Wall
		else								m_IsInAir = true;
	}
	// Nothing hit
	else
	{
		m_IsInAir = true;
	}

	// If landed on ground
	// -------------------
	if (previousAirState != m_IsInAir && m_IsInAir == false)
	{
		// Send event
		if (m_LandEvent.IsBound()) m_LandEvent.Broadcast();
	}
}
void UKinematicController::HandleGravity(float deltaTime)
{
	// Check conditions
	if (m_ShouldFall == false) return;

	// Apply gravity
	// -------------

	// Air
	if (m_IsInAir)
	{
		// Decrease Z component
		const float fallAcceleration = FMath::IsNearlyEqual(FallAccelerationTime, 0.f) ? MaxFallSpeed : MaxFallSpeed / FallAccelerationTime;
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
void UKinematicController::HandleMovement(float deltaTime)
{
	// Calculate movement
	// ------------------
	const float moveAcceleration = FMath::IsNearlyEqual(MoveAccelerationTime, 0.f) ? MaxMovementSpeed : MaxMovementSpeed / MoveAccelerationTime;

	// If input
	if (m_ShouldMove)
	{
		// Speed up
		m_MoveSpeed += moveAcceleration * deltaTime;
		if (MaxMovementSpeed < m_MoveSpeed) m_MoveSpeed = MaxMovementSpeed;
	}
	// No input
	else
	{
		// Slow down
		m_MoveSpeed -= moveAcceleration * deltaTime;
		if (m_MoveSpeed < 0)
		{
			m_MoveSpeed = 0;

			// Send event
			if (m_NoMovementEvent.IsBound()) m_NoMovementEvent.Broadcast();
		}
	}

	// Set total veloctity
	// -------------------
	//const FVector moveDirection = m_IsInAir ? m_CurrentDirection * AirControl : m_CurrentDirection;
	const FVector horizontalVelocity{ m_CurrentDirection * m_MoveSpeed };

	m_TotalVelocity.X = horizontalVelocity.X;
	m_TotalVelocity.Y = horizontalVelocity.Y;

	// Set rotation
	// ------------
	const FRotator currentRotation{ m_pActor->GetActorRotation() };
	const float rotationSpeed{ RotationSpeed * deltaTime };

	const FRotator newRotation{ FMath::Lerp(currentRotation, m_DesiredRotation, rotationSpeed) };
	m_pActor->SetActorRotation(newRotation);
}
void UKinematicController::HandleWallCollision(float /*deltaTime*/)
{
	// Raycast params
	// --------------

	// Basics
	const FVector adjustedVelocity{ m_TotalVelocity.X, m_TotalVelocity.Y, 0 };
	const double velocitySize{ adjustedVelocity.Size() };
	const FVector velocityDirection{ adjustedVelocity.GetSafeNormal() };

	// Hitresult
	FHitResult hitResult{};

	// Position
	const FVector startPos{ m_pActor->GetActorLocation()};
	const FVector endPos{ startPos + velocityDirection * (WallCollisionLength + SkinWidth) };

	// Params
	const FCollisionShape capsuleShape{ m_pCollision->GetCollisionShape(SkinWidth) };
	const FCollisionQueryParams traceParam{ "Raycast", true, m_pActor };

	// Raycast
	// -------
	if (GetWorld()->SweepSingleByChannel(hitResult, startPos, endPos, FQuat::Identity, ECollisionChannel::ECC_Visibility, capsuleShape, traceParam))
	{
		const double surfaceAngle{ FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FVector::UpVector, hitResult.Normal))) };

		// Wall
		if (MaxSlopeAngle < surfaceAngle)	SlideAlongWall(adjustedVelocity, hitResult.Normal);
		// Slope
		else								SlideAlongSlope(adjustedVelocity, hitResult.Normal);
	}
}
void UKinematicController::HandleDisplacement(float deltaTime)
{
	const FVector displacement{ m_TotalVelocity * deltaTime };
	m_pActor->AddActorWorldOffset(displacement, true);
}

void UKinematicController::SlideAlongWall(const FVector& velocity, const FVector& hitNormal)
{
	FVector hitSurface{ FVector::CrossProduct(FVector::UpVector, hitNormal) };
	hitSurface.Normalize();

	const FVector desiredVelocity{ velocity.ProjectOnTo(hitSurface) };
	m_TotalVelocity.X = desiredVelocity.X;
	m_TotalVelocity.Y = desiredVelocity.Y;
}
void UKinematicController::SlideAlongSlope(const FVector& velocity, const FVector& hitNormal)
{
	const FVector desiredVelocity{ FVector::VectorPlaneProject(velocity, hitNormal) };
	m_TotalVelocity.X = desiredVelocity.X;
	m_TotalVelocity.Y = desiredVelocity.Y;
	m_TotalVelocity.Z = desiredVelocity.Z;
}