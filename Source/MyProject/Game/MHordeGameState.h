// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MHordeGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8 {
	WaitingToStart,

	InProgress,

	WaveComplete,

	GameOver,
};

UCLASS(Abstract)
class MYPROJECT_API AMHordeGameState : public AGameStateBase
{
	GENERATED_BODY()

	AMHordeGameState();
	
	//////////////////////////////////////////////////////////////////////////
	// State
public:
	void SetWaveState(EWaveState NewState);

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "GameState", Meta = (DisplayName = "Wave State Changed"))
	void WaveStateChangedEvent(EWaveState CurrentState, EWaveState OldState);
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

};
