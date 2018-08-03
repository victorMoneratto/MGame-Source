// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "MPhysMat.generated.h"

UCLASS()
class MYPROJECT_API UMPhysMat : public UPhysicalMaterial {
	GENERATED_BODY()

public:
	UMPhysMat();

	//////////////////////////////////////////////////////////////////////////
	// Damage
public:
	/** Can apply critical damage */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	uint8 bVulnerableToCriticalDamage : 1;


};
