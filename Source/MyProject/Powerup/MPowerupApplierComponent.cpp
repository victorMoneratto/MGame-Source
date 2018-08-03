// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MPowerupApplierComponent.h"


UMPowerupApplierComponent::UMPowerupApplierComponent() {
	bStartOnBeginPlay = true;
	ActiveTime = 10;

	SetIsReplicated(true);
}

void UMPowerupApplierComponent::StartPowerup() {
	StartPowerupEvent();
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EndPowerupEvent, this, &UMPowerupApplierComponent::EndPowerup, ActiveTime);
}

void UMPowerupApplierComponent::EndPowerup() {
	EndPowerupEvent();
	DestroyComponent();
}

void UMPowerupApplierComponent::BeginPlay() {
	Super::BeginPlay();
	if (bStartOnBeginPlay) {
		StartPowerup();
	}
}
