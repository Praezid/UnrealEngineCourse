// Shoot Them Up Game, All Rights Reserved

#include "Player/STUPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(PlayerStateLog, All, All);

void ASTUPlayerState::LogInfo()
{
    UE_LOG(PlayerStateLog, Display, TEXT("TeamID: %i, Death num: %i, Kills num: %i"), TeamID, DeathsNum, KiilsNum);
}