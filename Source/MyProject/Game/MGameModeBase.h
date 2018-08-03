// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MGameModeBase.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMGameModeBase();

	UPROPERTY(EditDefaultsOnly, Category = "Points")
	float PointsPerKill;
	
	UPROPERTY(EditDefaultsOnly, Category = "Points")
	float PointsPerDeath;
	
	void Killed(AController* Instigator, AController* Killed);

	virtual bool CanDealDamage(class AMPlayerState* Instigator, class AMPlayerState* Damaged) const;
};
