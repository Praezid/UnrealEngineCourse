// Shoot Them Up Game, All Rights Reserved

#include "STUGameModeBase.h"
#include "Player/STUBaseCharacter.h"
#include "Player/STUPlayerController.h"
#include "UI/STUGameHUD.h"
#include "AI/STUAIController.h"
#include "Player/STUPlayerState.h"
#include "STUUtils.h"
#include "Components/STURespawnComponent.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY_STATIC(STUGameModeBaseLog, All, All);

constexpr static int32 MinRoundTimeForRespawn = 5;

ASTUGameModeBase::ASTUGameModeBase()
{
    DefaultPawnClass = ASTUBaseCharacter::StaticClass();
    PlayerControllerClass = ASTUPlayerController::StaticClass();
    HUDClass = ASTUGameHUD::StaticClass();
    PlayerStateClass = ASTUPlayerState::StaticClass();
}

void ASTUGameModeBase::StartPlay()
{
    Super::StartPlay();

    SpawnBots();
    CreateTeamsInfo();
    CurrentRound = 1;

    StartRound();
}

UClass* ASTUGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (InController && InController->IsA<AAIController>())
    {
        return AIPawnClass;
    }

    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ASTUGameModeBase::SpawnBots()
{
    if (!GetWorld())
    {
        return;
    }

    for (int32 i = 0; i < GameData.PlayersNum - 1; ++i)
    {
        FActorSpawnParameters SpawnInfo;
        SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        const auto STUAIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
        RestartPlayer(STUAIController);
    }
}

void ASTUGameModeBase::StartRound()
{
    RoundCountDown = GameData.RoundTime;
    GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &ASTUGameModeBase::GameTimerUpdate, 1.0f, true);
}

void ASTUGameModeBase::GameTimerUpdate()
{

    UE_LOG(STUGameModeBaseLog, Display, TEXT("Time %i"), RoundCountDown);
    UE_LOG(STUGameModeBaseLog, Display, TEXT("Round %i/%i"), CurrentRound, GameData.RoundsNum);

    // const auto TimerRate = GetWorldTimerManager().GetTimerRate(GameRoundTimerHandle);
    // RoundCountDown -= TimerRate;

    if (--RoundCountDown == 0)
    {
        GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);

        if (CurrentRound + 1 <= GameData.RoundsNum)
        {
            ++CurrentRound;
            ResetPlayers();
            StartRound();
        }
        else
        {
            GameOver();
        }
    }
}

void ASTUGameModeBase::ResetPlayers()
{
    if (!GetWorld())
    {
        return;
    }

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ResetOnePlayer(It->Get());
    }
}

void ASTUGameModeBase::ResetOnePlayer(AController* PlayerController)
{

    if (PlayerController && PlayerController->GetPawn())
    {
        PlayerController->GetPawn()->Reset();
    }
    RestartPlayer(PlayerController);
    SetPlayerColor(PlayerController);
}

void ASTUGameModeBase::CreateTeamsInfo()
{
    if (!GetWorld())
    {
        return;
    }

    int32 TeamID = 1;

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto PlayerController = It->Get();
        if (!PlayerController)
        {
            continue;
        }

        const auto PlayerState = Cast<ASTUPlayerState>(PlayerController->PlayerState);
        if (!PlayerState)
        {
            continue;
        }

        PlayerState->SetTeamID(TeamID);
        PlayerState->SetTeamColor(DetermineColorByTeamID(TeamID));
        SetPlayerColor(PlayerController);
        TeamID = TeamID == 1 ? 2 : 1;
    }
}

FLinearColor ASTUGameModeBase::DetermineColorByTeamID(int32 TeamID) const
{
    if (TeamID - 1 < GameData.TeamColors.Num())
    {
        return GameData.TeamColors[TeamID - 1];
    }
    UE_LOG(STUGameModeBaseLog, Warning, TEXT("No color for team id: %i set to default color %s"), TeamID,
        *GameData.DefaultTeamColor.ToString());
    return GameData.DefaultTeamColor;
}

void ASTUGameModeBase::SetPlayerColor(AController* PlayerController)
{
    if (!PlayerController)
    {
        return;
    }

    const auto Character = Cast<ASTUBaseCharacter>(PlayerController->GetPawn());
    if (!Character)
    {
        return;
    }

    const auto PlayerState = Cast<ASTUPlayerState>(PlayerController->PlayerState);
    if (!PlayerState)
    {
        return;
    }

    Character->SetPlayerColor(PlayerState->GetTeamColor());
}

void ASTUGameModeBase::Killed(AController* Killer, AController* Victim)
{
    const auto KillerPlayerState = Killer ? Cast<ASTUPlayerState>(Killer->PlayerState) : nullptr;
    const auto VictimPlayerState = Victim ? Cast<ASTUPlayerState>(Victim->PlayerState) : nullptr;
    if (KillerPlayerState)
    {
        KillerPlayerState->AddKill();
    }

    if (VictimPlayerState)
    {
        VictimPlayerState->AddDeath();
    }

    StartRespawn(Victim);
}

void ASTUGameModeBase::LogPlayerInfo()
{
    if (!GetWorld())
    {
        return;
    }

    for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        const auto PlayerController = It->Get();
        if (!PlayerController)
        {
            continue;
        }

        const auto PlayerState = Cast<ASTUPlayerState>(PlayerController->PlayerState);
        if (!PlayerState)
        {
            continue;
        }

        PlayerState->LogInfo();
    }
}

void ASTUGameModeBase::StartRespawn(AController* Controller)
{
    bool RespawnAvailable = RoundCountDown > MinRoundTimeForRespawn + GameData.RespawnTime;
    if (!RespawnAvailable) return;

    USTURespawnComponent* RespawnComponent = STUUtils::GetSTUPlayerComponent<USTURespawnComponent>(Controller);
    if (!RespawnComponent)
    {
        return;
    }
    RespawnComponent->Respawn(GameData.RespawnTime);
}
void ASTUGameModeBase::RespawnRequest(AController* PlayerToRespawn)
{
    ResetOnePlayer(PlayerToRespawn);
}

void ASTUGameModeBase::GameOver()
{
    UE_LOG(STUGameModeBaseLog, Display, TEXT("<=====GAME OVER=====>"));
    LogPlayerInfo();
    for (APawn* Pawn: TActorRange<APawn>(GetWorld()))
    {
        if (Pawn)
        {
            Pawn->TurnOff();
            Pawn->DisableInput(nullptr);
        }
    }
}
