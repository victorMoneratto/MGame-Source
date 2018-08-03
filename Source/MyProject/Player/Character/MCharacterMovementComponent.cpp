// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MCharacterMovementComponent.h"

UMCharacterMovementComponent::UMCharacterMovementComponent() {
	MaxSpeedModifier = 1;
}

float UMCharacterMovementComponent::GetMaxSpeed() const {
	return MaxSpeedModifier * Super::GetMaxSpeed();
}
