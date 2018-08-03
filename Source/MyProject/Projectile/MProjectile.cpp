// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "Projectile/MProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMProjectile::AMProjectile() {
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	SetReplicates(true);
	SetReplicateMovement(true);
}

