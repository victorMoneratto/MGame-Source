// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Game/MGameModeBase.h"
#include "MHordeMode.generated.h"

enum class EWaveState : uint8;

UCLASS()
class MYPROJECT_API AMHordeMode : public AMGameModeBase
{
	GENERATED_BODY()
	
public:
	AMHordeMode();

	//////////////////////////////////////////////////////////////////////////
	// Wave
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave", Meta = (ClampMin = 1))
	int32 WaveNumber;

	float WavePrepareTime;

	void PrepareForNextWave();

	void RestartDeadPlayers();

	FTimerHandle TimeHandle_BeginWave;

	void BeginWave();

	void EndWave();

	bool CheckAnyEnemyRemaining();
	bool CheckAnyPlayerAlive();

	//////////////////////////////////////////////////////////////////////////
	// Spawn enemies
protected:
	float TimeBetweenEnemySpawns;

	int32 EnemiesToSpawn;

	FTimerHandle TimerHandle_SpawnEnemy;

	void SpawnEnemy();

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave", Meta = (DisplayName = "Spawn Enemy"))
	void SpawnEnemyEvent();

	//////////////////////////////////////////////////////////////////////////
	// Team
public:
	virtual bool CanDealDamage(AMPlayerState* Instigator, AMPlayerState* Damaged) const override;


	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	void GameOver();

	//////////////////////////////////////////////////////////////////////////
	// Accessors and mutators
protected:
	void SetWaveState(EWaveState NewState);
};
