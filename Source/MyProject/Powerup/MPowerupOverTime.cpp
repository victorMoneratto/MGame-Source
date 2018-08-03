// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MPowerupOverTime.h"
#include "Powerup/MPowerupApplierComponent.h"
#include "Player/Character/MCharacter.h"

AMPowerupOverTime::AMPowerupOverTime() {

}

void AMPowerupOverTime::OnPickup(class AMCharacter* Character) {
	Super::OnPickup(Character);

	if (Role == ROLE_Authority) {
		if (Character && ApplierClass) {
			UMPowerupApplierComponent* ApplierComp = NewObject<UMPowerupApplierComponent>(Character, *ApplierClass);
			if (ApplierComp) {
				ApplierComp->RegisterComponent();
			}
		}
	}
}
