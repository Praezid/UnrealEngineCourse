// Shoot Them Up Game, All Rights Reserved

#include "UI/STUSpectatorWidget.h"
#include "STUUtils.h"
#include "Components/STURespawnComponent.h"

bool USTUSpectatorWidget::GetRespawnTime(int32& CountDownTime) const
{
    USTURespawnComponent* RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(GetOwningPlayer());
    if (RespawnComponent && RespawnComponent->IsRespawnInProgress())
    {
        CountDownTime = RespawnComponent->GetRespawnCountDown();
        return true;
    }
    return false;
}
