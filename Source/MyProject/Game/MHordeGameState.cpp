// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MHordeGameState.h"
#include "Net/UnrealNetwork.h"


AMHordeGameState::AMHordeGameState() {
	WaveState = EWaveState::WaitingToStart;
}

void AMHordeGameState::SetWaveState(EWaveState NewState) {
	if (Role == ROLE_Authority) {
		EWaveState PrevState = WaveState;
		WaveState = NewState;
		if (NewState != PrevState) {
			OnRep_WaveState(PrevState);
		}
	}
}

void AMHordeGameState::OnRep_WaveState(EWaveState OldState) {
	WaveStateChangedEvent(WaveState, OldState);
}

void AMHordeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMHordeGameState, WaveState);
}
