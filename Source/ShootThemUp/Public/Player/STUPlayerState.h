// Shoot Them Up Game, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "STUPlayerState.generated.h"

/**
 *
 */
UCLASS()
class SHOOTTHEMUP_API ASTUPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    int32 GetTeamID() const { return TeamID; }
    void SetTeamID(int32 NewTeamID) { TeamID = NewTeamID; }

    FLinearColor GetTeamColor() const { return TeamColor; }
    void SetTeamColor(const FLinearColor& NewTeamColor) { TeamColor = NewTeamColor; }

    void AddKill() { ++KiilsNum; }
    int32 GetKills() const { return KiilsNum; }

    void AddDeath() { ++DeathsNum; }
    int32 GetDeaths() const { return DeathsNum; }

    void LogInfo();

private:
    int32 TeamID;
    FLinearColor TeamColor;

    int32 KiilsNum = 0;
    int32 DeathsNum = 0;
};
