// Copyright 2018 Victor Moneratto. All Rights Reserved.

#include "MBTService_SelectTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Game/MGameModeBase.h"
#include "Game/MPlayerState.h"

UMBTService_SelectTarget::UMBTService_SelectTarget() {
	NodeName = "Select Sensed Target";
}

void UMBTService_SelectTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* Target = nullptr;
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn) {
		UAIPerceptionComponent* Perception = Cast<UAIPerceptionComponent>(Pawn->GetComponentByClass(UAIPerceptionComponent::StaticClass()));
		if (Perception) {
			TArray<AActor*> KnownActors;
			Perception->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), KnownActors);

			AMGameModeBase* GM = Cast<AMGameModeBase>(GetWorld()->GetAuthGameMode());
			float MinDistSqrd = INFINITY;
			for (int32 i = 0; i < KnownActors.Num(); i++) {
				AActor* Actor = KnownActors[i];
				if (Actor->GetInstigatorController() && Pawn->GetInstigatorController()) {
					bool IsEnemy = GM->CanDealDamage(Cast<AMPlayerState>(Actor->GetInstigatorController()->PlayerState),
													 Cast<AMPlayerState>(Pawn->GetInstigatorController()->PlayerState));
					if (IsEnemy) {
						float DistSqrd = FVector::DistSquared(Actor->GetActorLocation(), Pawn->GetActorLocation());
						if (DistSqrd < MinDistSqrd) {
							MinDistSqrd = DistSqrd;
							Target = Actor;
						}
						
					}
				}
			}
		}
	}

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard) {
		Blackboard->SetValueAsObject(BlackboardKey.SelectedKeyName, Target);
	}
}
