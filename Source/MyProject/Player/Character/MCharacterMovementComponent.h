// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MCharacterMovementComponent.generated.h"

UCLASS()
class MYPROJECT_API UMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UMCharacterMovementComponent();

	//////////////////////////////////////////////////////////////////////////
	// Speed
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Speed")
	float MaxSpeedModifier;

	virtual float GetMaxSpeed() const override;

};
