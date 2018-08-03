// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/MWeapon.h"
#include "MWeapon_Projectile.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMWeapon_Projectile : public AMWeapon {
	GENERATED_BODY()

public:
	AMWeapon_Projectile();


	//////////////////////////////////////////////////////////////////////////
	// Aiming
protected:
	/** Distance along crosshair for zeroing target, and thus shot direction */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float ZeroingDistance;


	//////////////////////////////////////////////////////////////////////////
	// Firing
protected:
	/** [Server + Local] Fire projectile */
	virtual void Fire() override;

	/** Class of projectile being fired */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AMProjectile> ProjectileClass;

	/** Simulate fire effects (e.g. particles, sound) */
	void SimulateFire();

	/** [Multicast] Call SimulateFire effects for everyone,
	  * except for the local pawn, it simulates on Fire() directly */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSimulateFire();

};
