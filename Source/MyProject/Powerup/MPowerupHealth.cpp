// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MPowerupHealth.h"
#include "Player/Character/MCharacter.h"
#include "Health/MHealthComponent.h"

AMPowerupHealth::AMPowerupHealth() {
	HealAmount = 50.f;
}

void AMPowerupHealth::OnPickup(AMCharacter* Character) {
	if (Role == ROLE_Authority) {
		if (Character) {
			Character->GetHealth()->Heal(HealAmount, nullptr, nullptr, GetOwner());
		}
	}
}
