// Shoot Them Up Game, All Rights Reserved

#include "AI/Tasks/STUNextLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"

USTUNextLocationTask::USTUNextLocationTask()
{
    NodeName = "Next Location";
}

EBTNodeResult::Type USTUNextLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const auto Controller = OwnerComp.GetAIOwner();
    const auto BlackBoard = OwnerComp.GetBlackboardComponent();
    if (!Controller || !BlackBoard)
    {
        return EBTNodeResult::Failed;
    }

    const auto Pawn = Controller->GetPawn();
    if (!Pawn)
    {
        return EBTNodeResult::Failed;
    }
    
    const auto NavSystem = UNavigationSystemV1::GetCurrent(Pawn);
    if (!NavSystem)
    {
        return EBTNodeResult::Failed;
    }

    FNavLocation NavLocation;
    
    const auto Found = NavSystem->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), Radius, NavLocation);
    if (!Found)
    {
        return EBTNodeResult::Failed;
    }

    BlackBoard->SetValueAsVector(AimLocationKey.SelectedKeyName, NavLocation.Location);

    return EBTNodeResult::Succeeded;
}