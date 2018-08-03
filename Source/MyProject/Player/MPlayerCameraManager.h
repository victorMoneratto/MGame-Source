// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "MPlayerCameraManager.generated.h"

UCLASS()
class MYPROJECT_API AMPlayerCameraManager : public APlayerCameraManager {
	GENERATED_BODY()

public:
	AMPlayerCameraManager();


	//////////////////////////////////////////////////////////////////////////
	// Events
public:
	/** Modify FOV for aiming */
	virtual void UpdateCamera(float DeltaTime) override;


	//////////////////////////////////////////////////////////////////////////
	// Field of view
protected:

	/** FOV when idle */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float NormalFOV;

	/** FOV when Targeting */
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingFOV;

	/** FOV transition speed for targeting */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = 0.f) , Category = "Targeting")
	float TargetingSpeed;

};
