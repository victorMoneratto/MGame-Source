// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MImpactEffect.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS(Abstract)
class MYPROJECT_API AMImpactEffect : public AActor {
	GENERATED_BODY()

public:	
	AMImpactEffect();

	/** Surface type for effect, should be set when spawning */
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
	
	//////////////////////////////////////////////////////////////////////////
	// Events
protected:
	/** Play effect */
	virtual void BeginPlay() override;


	//////////////////////////////////////////////////////////////////////////
	// Effects
public:
	/** Get particles for surface */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** Get sound for surface */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

protected:
	/** Default effect used when no override defined */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* DefaultFX;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* DefaultSound;

	/** Impact effect on Flesh */
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* FleshFX;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	USoundCue* FleshSound;
};
