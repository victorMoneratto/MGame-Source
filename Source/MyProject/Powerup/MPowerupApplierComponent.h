// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPowerupApplierComponent.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECT_API UMPowerupApplierComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UMPowerupApplierComponent();

	//////////////////////////////////////////////////////////////////////////
	// Powerup
protected:
	UFUNCTION()
	void StartPowerup();
	UFUNCTION()
	void EndPowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup", Meta = (DisplayName = "Start Powerup"))
	void StartPowerupEvent();
	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup", Meta = (DisplayName = "End Powerup"))
	void EndPowerupEvent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Powerup")
	uint32 bStartOnBeginPlay : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Powerup")
	float ActiveTime;

	FTimerHandle TimerHandle_EndPowerupEvent;

	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	virtual void BeginPlay() override;
};
