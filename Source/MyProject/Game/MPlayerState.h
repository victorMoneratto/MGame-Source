// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MPlayerState.generated.h"

UCLASS()
class MYPROJECT_API AMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	AMPlayerState();

	//////////////////////////////////////////////////////////////////////////
	// Team
public:
	FORCEINLINE int32 GetTeamNum() const { return TeamNum; };
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Replicated, Category = "Team")
	int32 TeamNum;

	//////////////////////////////////////////////////////////////////////////
	// Score
public:
	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	void ScoreKill(int32 Points);

	void ScoreDeath(int32 Points);


	//////////////////////////////////////////////////////////////////////////
	// Replication
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

};
