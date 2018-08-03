// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MPlayerState.h"
#include "Net/UnrealNetwork.h"

AMPlayerState::AMPlayerState() {
	NumKills = 0;
	NumDeaths = 0;
	TeamNum = 0;
}

void AMPlayerState::ScoreKill(int32 Points) {
	NumKills++;
	Score += Points;
}

void AMPlayerState::ScoreDeath(int32 Points) {
	NumDeaths++;
	Score += Points;
}

void AMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMPlayerState, NumKills);
	DOREPLIFETIME(AMPlayerState, NumDeaths);
	DOREPLIFETIME(AMPlayerState, TeamNum);
}
