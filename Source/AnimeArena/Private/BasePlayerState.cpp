#include "BasePlayerState.h"
#include "../BasePlayerController.h"
#include "KinematicController.h"

#include <Kismet/GameplayStatics.h>

void UBasePlayerState::OnEnter(AActor* pStateOwner)
{
	Super::OnEnter(pStateOwner);

	// Get owners
	m_pCharacter = Cast<ABaseCharacter>(pStateOwner);
	m_pPlayerController = Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	m_pKinematicController = Cast<UKinematicController>(m_pCharacter->FindComponentByClass(UKinematicController::StaticClass()));
}

void UBasePlayerState::BaseMove(const FVector2D& direction)
{
	//if (m_pPlayerController == nullptr) return;

	//// Get Directions
	//// --------------

	//// Find out which way is forward
	//const FRotator rotation = m_pCharacter->Controller->GetControlRotation();
	//const FRotator yawRotation{ 0, rotation.Yaw, 0 };

	//// Get forward vector
	//const FVector forwardDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);

	//// Get right vector 
	//const FVector rightDirection = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);

	//// Calculate movement
	//// ------------------



	//// Add movement 
	//m_pCharacter->AddMovementInput(forwardDirection, direction.Y);
	//m_pCharacter->AddMovementInput(rightDirection, direction.X);
}