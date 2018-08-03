// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Powerup/MPowerup.h"
#include "MPowerupHealth.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMPowerupHealth : public AMPowerup
{
	GENERATED_BODY()
	
public:
	AMPowerupHealth();

	//////////////////////////////////////////////////////////////////////////
	// Healing
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heal")
	float HealAmount;
protected:
	virtual void OnPickup(class AMCharacter* Character) override;

};
