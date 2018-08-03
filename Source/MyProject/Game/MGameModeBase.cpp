// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "Game/MGameModeBase.h"
#include "Player/Character/MCharacter.h"
#include "Player/MPlayerController.h"
#include "Game/MPlayerState.h"

AMGameModeBase::AMGameModeBase() {
	DefaultPawnClass = AMCharacter::StaticClass();
	PlayerControllerClass = AMPlayerController::StaticClass();
	PlayerStateClass = AMPlayerState::StaticClass();

	PointsPerKill = 25.f;
	PointsPerDeath = 0.f;
}

void AMGameModeBase::Killed(AController* Instigator, AController* Killed) {
	if (Instigator) {
		AMPlayerState* InstigatorPlayerState = Cast<AMPlayerState>(Instigator->PlayerState);
		if (InstigatorPlayerState) {
			InstigatorPlayerState->ScoreKill(PointsPerKill);
		}
	}

	if (Killed) {
		AMPlayerState* KilledPlayerState = Cast<AMPlayerState>(Killed->PlayerState);
		if (KilledPlayerState) {
			KilledPlayerState->ScoreKill(PointsPerDeath);
		}
	}
}

bool AMGameModeBase::CanDealDamage(AMPlayerState* Instigator, AMPlayerState* Damaged) const {
	return true;
}
