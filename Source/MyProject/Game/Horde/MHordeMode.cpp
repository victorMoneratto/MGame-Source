// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MHordeMode.h"
#include "Health/MHealthComponent.h"
#include "Game/MHordeGameState.h"
#include "Game/MPlayerState.h"

AMHordeMode::AMHordeMode() {
	GameStateClass = AMHordeGameState::StaticClass();

	TimeBetweenEnemySpawns = 1.f;
	WavePrepareTime = 2.f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	EnemiesToSpawn = 1;
	WaveNumber = 1;
}



void AMHordeMode::PrepareForNextWave() {
	SetWaveState(EWaveState::WaitingToStart);
	GetWorldTimerManager().SetTimer(TimeHandle_BeginWave, this, &AMHordeMode::BeginWave, WavePrepareTime);
	RestartDeadPlayers();
}

void AMHordeMode::RestartDeadPlayers() {
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		APlayerController* PC = It->Get();
		if (PC && !PC->GetPawn()) {
			RestartPlayer(PC);
		}
	}
}

void AMHordeMode::BeginWave() {
	SetWaveState(EWaveState::InProgress);
	EnemiesToSpawn = WaveNumber * 2;
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnEnemy, this, &AMHordeMode::SpawnEnemy, TimeBetweenEnemySpawns, true, 0.f);
}

void AMHordeMode::EndWave() {
	WaveNumber++;
	GetWorldTimerManager().ClearTimer(TimerHandle_SpawnEnemy);
}

bool AMHordeMode::CheckAnyEnemyRemaining() {
	if (EnemiesToSpawn > 0) {
		return true;
	}

	bool bEnemyRemaining = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++) {
		APawn* Pawn = It->Get();
		if (!Pawn || Pawn->IsPlayerControlled()) {
			continue;
		}

		UMHealthComponent* Health = Cast<UMHealthComponent>(Pawn->GetComponentByClass(UMHealthComponent::StaticClass()));
		if (Health && Health->IsAlive()) {
			bEnemyRemaining = true;
			break;
		}
	}

	return bEnemyRemaining;
}

bool AMHordeMode::CheckAnyPlayerAlive() {
	bool bAnyPlayerAlive = false;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++) {
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn()) {
			APawn* Pawn = PC->GetPawn();
			UMHealthComponent* Health = Cast<UMHealthComponent>(Pawn->GetComponentByClass(UMHealthComponent::StaticClass()));
			if (ensure(Health) && Health->IsAlive()) {
				bAnyPlayerAlive = true;
				break;
			}
		}
	}

	return bAnyPlayerAlive ;
}

void AMHordeMode::SpawnEnemy() {
	SpawnEnemyEvent();
	EnemiesToSpawn--;

	if (EnemiesToSpawn <= 0) {
		EndWave();
	}
}

bool AMHordeMode::CanDealDamage(AMPlayerState* Instigator, AMPlayerState* Damaged) const {
	return Instigator && Damaged && (Instigator == Damaged || Instigator->GetTeamNum() != Damaged->GetTeamNum());
}

void AMHordeMode::StartPlay() {
	Super::StartPlay();

	PrepareForNextWave();
}

void AMHordeMode::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);

	// #MoneTodo refactor enemy and player checks into a timer,
	// tick seems overkill and easily overwritten in bp
	bool bPlayerAlive = CheckAnyPlayerAlive();
	if (!bPlayerAlive) {
		GameOver();
	}


	bool bEnemyRemaining = CheckAnyEnemyRemaining();
	if (!bEnemyRemaining && !GetWorldTimerManager().IsTimerActive(TimeHandle_BeginWave)) {
// 		SetWaveState(EWaveState::WaveComplete); // overwritten in preparefornextwave
		PrepareForNextWave();
	}
}

void AMHordeMode::GameOver() {
	EndWave();
	SetWaveState(EWaveState::GameOver);
}

void AMHordeMode::SetWaveState(EWaveState NewState) {
	AMHordeGameState* GS = GetGameState<AMHordeGameState>();
	if (ensureAlways(GS)) {
		GS->SetWaveState(NewState);
	}
}

