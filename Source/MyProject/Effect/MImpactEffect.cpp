// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "Effect/MImpactEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/PointLightComponent.h"
#include "MGameTypes.h"

AMImpactEffect::AMImpactEffect() {
	PrimaryActorTick.bCanEverTick = true;
	bAutoDestroyWhenFinished = true; // #MoneWhy it doesn't seem to work
}

UParticleSystem* AMImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const {
	UParticleSystem* ImpactFX = nullptr;
	switch (SurfaceType) {
	case MGAME_SURFACE_FLESH: ImpactFX = FleshFX; break;
	}

	return ImpactFX ? ImpactFX : DefaultFX;
}

USoundCue* AMImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const {
	USoundCue* ImpactSound = nullptr;
	switch (SurfaceType) {
	case MGAME_SURFACE_FLESH: ImpactSound = FleshSound; break;
	}

	return ImpactSound ? ImpactSound : DefaultSound;
}

void AMImpactEffect::BeginPlay() {
	Super::BeginPlay();

	bAutoDestroyWhenFinished = true;

	UParticleSystem* ImpactFX = GetImpactFX(SurfaceType);
	UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());

	USoundCue* ImpactSound = GetImpactSound(SurfaceType);
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}