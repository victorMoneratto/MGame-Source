// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "Player/MPlayerCameraManager.h"
#include "Character/MCharacter.h"

AMPlayerCameraManager::AMPlayerCameraManager() {
	NormalFOV = 90.f;
	TargetingFOV = 60.f;
	TargetingSpeed = 20.f;
}

void AMPlayerCameraManager::UpdateCamera(float DeltaTime) {
	AMCharacter* Character = PCOwner ? Cast<AMCharacter>(PCOwner->GetPawn()) : nullptr;
	if (Character) {
		float IdealFOV = Character->IsAiming() ? TargetingFOV : NormalFOV;
		float CurrentFOV = FMath::FInterpTo(GetFOVAngle(), IdealFOV, DeltaTime, TargetingSpeed);
		SetFOV(CurrentFOV); // #MoneWhy LockedFOV? DefaultFOV?
	}

	Super::UpdateCamera(DeltaTime);
}
