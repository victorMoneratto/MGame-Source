// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MWeapon_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile/MProjectile.h"
#include "DrawDebugHelpers.h"

AMWeapon_Projectile::AMWeapon_Projectile() {
	ZeroingDistance = 1000.f;
}

void AMWeapon_Projectile::Fire() {
	Super::Fire();

	if (Role == ROLE_Authority) {
		// Aiming
		FVector AimDir = GetAdjustedAim(); // #MoneTodo bullet spread here
		FVector AimLoc = GetShotStartLocation(AimDir);
		FVector ZeroingPoint = AimLoc + AimDir * ZeroingDistance;

		FVector ProjectileStart = GetMuzzleLocation();
		FVector ProjectileDirScaled = ZeroingPoint - ProjectileStart;

		// Spawn projectile
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = Instigator;
		SpawnParams.Owner = Instigator;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AMProjectile>(ProjectileClass, ProjectileStart, ProjectileDirScaled.Rotation(), SpawnParams);

		// Debug draw line
		if (DebugMWeapon >= MDEBUG_LEVEL_DRAW) {
			DrawDebugLine(GetWorld(), ProjectileStart, ZeroingPoint, FColor::Red, false, 2.f, 0, 1.f);
		}

		MulticastSimulateFire();
	} else {
		SimulateFire();
	}
}

void AMWeapon_Projectile::SimulateFire() {
	PlayFireFX();
}

void AMWeapon_Projectile::MulticastSimulateFire_Implementation() {
	AController* Controller = GetInstigatorController();
	if (!Controller || !Controller->IsLocalController()) {
		PlayFireFX();
	}
}
