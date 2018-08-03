// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Powerup/MPowerup.h"
#include "MPowerupOverTime.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMPowerupOverTime : public AMPowerup
{
	GENERATED_BODY()
	
public:
	AMPowerupOverTime();


	//////////////////////////////////////////////////////////////////////////
	// Pickup
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TSubclassOf<class UMPowerupApplierComponent> ApplierClass;

protected:
	virtual void OnPickup(class AMCharacter* Character) override;

};
