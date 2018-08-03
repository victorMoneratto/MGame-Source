// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MWeapon_Hitscan.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Effect/MImpactEffect.h"
#include "Physics/MPhysMat.h"
#include "MyProject.h"
#include "Math/RandomStream.h"


AMWeapon_Hitscan::AMWeapon_Hitscan() {
	BulletTrailEndParamName = "BeamEnd";
}

void AMWeapon_Hitscan::Fire() {
	Super::Fire();

	// #MoneTodo Bullet spread is not replicated or deterministic between clients and server,
	// the whole firing mechanic should be remade in a way the client chooses a seed for randomness
	// and replicate to server, letting it validate of course
		// Direction
	float Spread = FMath::DegreesToRadians(.1f); // #MoneTodo follow a curve based on burst count
	FVector ShotDir = WeaponRandomStream.VRandCone(GetAdjustedAim(), Spread, Spread);

	// Start and end points
	FVector ShotStart = GetShotStartLocation(ShotDir);
	FVector ShotEnd = ShotStart + ShotDir * 10'000.f; // #MoneHardcode

	// Hit and confirmation
	FHitResult OriginalHit = WeaponTrace(ShotStart, ShotEnd);
	FHitResult Hit = ConfirmWeaponHit(OriginalHit, ShotEnd);

	if (Hit.GetActor()) {
		ShotEnd = Hit.ImpactPoint;

		// #MoneResearch is applying damage on client a good idea for some effect?
		if (Role == ROLE_Authority) {
				// Damage
			bool bCritical = IsCriticalHit(Hit);
			float Damage = bCritical ? 35.f : 20.f;
			AActor* DamagedActor = Hit.GetActor();
			AController* Controller = Instigator ? Cast<AController>(Instigator->Controller) : nullptr;
			UGameplayStatics::ApplyPointDamage(DamagedActor, Damage, ShotDir, Hit, Controller, this, DamageType);
		}
	}

	// Debug draw line (green until shot start, red then)
	if (DebugMWeapon >= MDEBUG_LEVEL_DRAW) {
		if (APlayerController* PC = Cast<APlayerController>(Instigator->Controller)) {
			FVector ViewLoc; FRotator Unused;
			PC->GetPlayerViewPoint(ViewLoc, Unused);
			DrawDebugLine(GetWorld(), ViewLoc, ShotStart, FColor::Green, false, 2.f, 0, 1.f);
		}
		FVector End = OriginalHit.GetActor() ? OriginalHit.ImpactPoint : OriginalHit.TraceEnd;
		DrawDebugLine(GetWorld(), ShotStart, End, FColor::Red, false, 2.f, 0, 1.f);
	}

	// Simulate fire for other players
	UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
	EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysMat);
	if (Role == ROLE_Authority) {
		MulticastSimulateFire(Hit.GetActor() != nullptr, ShotEnd, SurfaceType);
	} 

	// Simulate fire locally
	AController* Controller = GetInstigatorController();
	if (Controller && Controller->IsLocalController()) {
		SimulateFire(Hit.GetActor() != nullptr, ShotEnd, SurfaceType);
	}

}


/** Trace for obstructions between weapon and hit location,
  * this should look for weapon penetration and general
  * discrepancies from shooting from camera and not weapon muzzle */
FHitResult AMWeapon_Hitscan::ConfirmWeaponHit(const FHitResult& WeaponHit, const FVector& OriginalTraceEnd) {
	FHitResult ResultHit = WeaponHit;

	FVector MuzzleLoc = GetMuzzleLocation();
	FVector MuzzleDir = GetMuzzleRotation().Vector();

	// Confirmation trace starts on the "base" of the weapon, it is a 
	// projection of the actor location on the muzzle direction line
	FVector ConfirmStart = MuzzleLoc + ((GetActorLocation() - MuzzleLoc) | MuzzleDir) * MuzzleDir;
	FVector ConfirmEnd = WeaponHit.bBlockingHit ? WeaponHit.ImpactPoint : OriginalTraceEnd;

	// Trace overlapping pawns and blocking anything else
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.TraceTag = TEXT("Weapon");
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.bTraceAsyncScene = true; // #MoneWhy async scene?
	QueryParams.bFindInitialOverlaps = true;
	FCollisionResponseParams ResponseParams(ECR_Block);
	ResponseParams.CollisionResponse.Pawn = ECR_Overlap;

	TArray<FHitResult> ConfirmHits;
	GetWorld()->LineTraceMultiByChannel(ConfirmHits, ConfirmStart, ConfirmEnd,
										COLLISION_WEAPON, QueryParams, ResponseParams);

	float LowestHitTime = 1.f;
	for (FHitResult& ConfirmHit : ConfirmHits) {
		AActor* HitActor = ConfirmHit.GetActor();

		if (HitActor != nullptr && HitActor != this && HitActor != GetOwner()) {
			if (ConfirmHit.Time < LowestHitTime) {
				LowestHitTime = ConfirmHit.Time;
				ResultHit = ConfirmHit;
			}
		}
	}

	if (DebugMWeapon >= MDEBUG_LEVEL_LOG) {
		FString LogHits;
		for (FHitResult& ConfirmHit : ConfirmHits) {
			AActor* HitActor = ConfirmHit.GetActor();
			if (HitActor) {
				LogHits += HitActor->GetName();
				LogHits += TEXT("(");
				LogHits += FString::SanitizeFloat(ConfirmHit.Time);
				LogHits += TEXT("), ");
			} else {
				LogHits += TEXT("<null>, ");
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Confirm Hits: %s"), *LogHits);
	}

	if (DebugMWeapon >= MDEBUG_LEVEL_DRAW) {
		DrawDebugLine(GetWorld(), ConfirmStart, ResultHit.ImpactPoint, FColor::Blue, false, 2.f, 0, 1.f);
	}

	return ResultHit;
}


bool AMWeapon_Hitscan::IsCriticalHit(const FHitResult& Hit) {
	UMPhysMat* PhysMat = Cast<UMPhysMat>(Hit.PhysMaterial.Get());
	if (PhysMat) {
		if (PhysMat->bVulnerableToCriticalDamage) {
			return true;
		}
	}

	return false;
}

void AMWeapon_Hitscan::SimulateFire(bool bHit, FVector TrailEnd, EPhysicalSurface SurfaceType) {
	// Muzzle
	PlayFireFX();

	FVector MuzzleLoc = GetMuzzleLocation();

	// Trail
	UParticleSystemComponent* BulletTrail =
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrailFX, MuzzleLoc, FRotator::ZeroRotator);
	if (BulletTrail) {
		BulletTrail->SetVectorParameter(BulletTrailEndParamName, TrailEnd);
	}

	// Impact
	if (bHit) {
		FRotator ImpactRotation = (TrailEnd - MuzzleLoc).Rotation();
		FVector ImpactLocation = TrailEnd;
		FTransform ImpactTransform(ImpactRotation, ImpactLocation);

		AMImpactEffect* Effect = GetWorld()->SpawnActorDeferred<AMImpactEffect>(ImpactFX, ImpactTransform);
		if (Effect) {
			Effect->SurfaceType = SurfaceType;
			UGameplayStatics::FinishSpawningActor(Effect, ImpactTransform);
		}
	}
}

void AMWeapon_Hitscan::MulticastSimulateFire_Implementation(bool bHit, FVector TrailEnd, EPhysicalSurface SurfaceType) {
	AController* Controller = GetInstigatorController();
	if (!Controller || !Controller->IsLocalController()) {
		SimulateFire(bHit, TrailEnd, SurfaceType);
	}
}