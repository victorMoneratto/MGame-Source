// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MProjectile.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMProjectile : public AActor {
	GENERATED_BODY()

public:
	AMProjectile();


	//////////////////////////////////////////////////////////////////////////
	// Components
protected:

	/** Movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovement;
};
