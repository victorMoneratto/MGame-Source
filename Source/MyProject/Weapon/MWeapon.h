// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MWeapon.generated.h"

UCLASS(Abstract)
class MYPROJECT_API AMWeapon : public AActor {
	GENERATED_BODY()

public:
	AMWeapon();


	//////////////////////////////////////////////////////////////////////////
	// Components
protected:
	/** Weapon body mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* Mesh;


	//////////////////////////////////////////////////////////////////////////
	// Firing
public:
	/** [Server + Local] Start fire (auto, semi-auto, burst etc.) */
	virtual void StartFire();

	/** [Server + Local] Stop current fire */
	virtual void StopFire();

protected:
	/** [Server] Input to start fire */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire(int32 ClientSeed);

	/** [Server] Input to stop fire */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	/**  */
	virtual void Fire();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	uint32 bAutomaticFire : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category ="Weapon", Meta = (EditCondition = bAutomaticFire))
	float RoundsPerMinute;

	/** Derived from RoundsPerMinute */
	float TimeBetweenRounds;

	float LastFireTime;
	
	FTimerHandle TimerHandle_Burst;

	FRandomStream WeaponRandomStream;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FHitResult WeaponTrace(const FVector& Start, const FVector& End, bool bIgnoreSelfAndOwner = true);


	//////////////////////////////////////////////////////////////////////////
	// Aiming
protected:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FVector GetAdjustedAim() const;

	FVector GetShotStartLocation(const FVector& AimDir) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class UCameraShake> FireCameraShake;


	/////////////////////////////////////////////////////////////////////////////
	// Muzzle
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleAttachPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	class UParticleSystem* MuzzleFX;

	void PlayFireFX();


	//////////////////////////////////////////////////////////////////////////
	// Events
	virtual void BeginPlay() override;


	//////////////////////////////////////////////////////////////////////////
	// Data accessor
public:
	UFUNCTION(BlueprintCallable, Category = "Components")
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; };

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FRotator GetMuzzleRotation() const;
};

extern int32 DebugMWeapon;
extern FAutoConsoleVariableRef CVar_DebugWeapon;
#define MDEBUG_LEVEL_LOG 1
#define MDEBUG_LEVEL_DRAW 2