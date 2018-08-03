// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/MWeapon.h"
#include "MWeapon_Hitscan.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class MYPROJECT_API AMWeapon_Hitscan : public AMWeapon {
	GENERATED_BODY()

public:
	AMWeapon_Hitscan();


	//////////////////////////////////////////////////////////////////////////
	// Firing
protected:
	virtual void Fire() override;

	FHitResult ConfirmWeaponHit(const FHitResult& WeaponHit, const FVector& OriginalTraceEnd);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	bool IsCriticalHit(const FHitResult& Hit);


	//////////////////////////////////////////////////////////////////////////
	// Effects
protected:
	void SimulateFire(bool bHit, FVector TrailEnd, EPhysicalSurface SurfaceType);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSimulateFire(bool bHit, FVector TrailEnd, EPhysicalSurface SurfaceType);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AMImpactEffect> ImpactFX;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* BulletTrailFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName BulletTrailEndParamName;
};
