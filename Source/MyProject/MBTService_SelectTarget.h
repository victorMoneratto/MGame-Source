// Copyright 2018 Victor Moneratto. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "MBTService_SelectTarget.generated.h"

UCLASS()
class MYPROJECT_API UMBTService_SelectTarget : public UBTService_BlackboardBase {
	GENERATED_BODY()

	UMBTService_SelectTarget();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
