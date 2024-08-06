#include "Components/STUHealthComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "STUUtils.h"
#include "STUGameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(HealthComponentLog, All, All);

USTUHealthComponent::USTUHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTUHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    check(MaxHealth > 0);

    SetHealth(MaxHealth);

    AActor* ComponentOwner = GetOwner();
    if (ComponentOwner)
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
    }
}

void USTUHealthComponent::OnTakeAnyDamage(
    AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || IsDead() || !GetWorld())
    {
        return;
    }

    UE_LOG(HealthComponentLog, Display, TEXT("Damaged actor %s take damage from %s"),
        *Cast<APawn>(DamagedActor)->GetController()->GetName(), *InstigatedBy->GetName());

    if (STUUtils::AreEnemies(Cast<APawn>(DamagedActor)->GetController(), InstigatedBy))
    {
        SetHealth(Health - Damage);

        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
        if (IsDead())
        {
            Killed(InstigatedBy);
            OnDeath.Broadcast();
        }
        else if (AutoHeal)
        {
            GetWorld()->GetTimerManager().SetTimer(
                HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpDateTime, true, HealDelay);
        }

        PlayCameraShake();
    }
}

void USTUHealthComponent::HealUpdate()
{
    SetHealth(Health + HealModifier);

    if (IsHealthFull() && GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
    }
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
    const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
    const auto HealthDelta = NextHealth - Health;

    Health = NextHealth;
    OnHealthChanged.Broadcast(Health, HealthDelta);
}

bool USTUHealthComponent::TryToAddHealth(float HealthAmount)
{
    if (IsDead() || IsHealthFull())
    {
        return false;
    }
    SetHealth(Health + HealthAmount);
    return true;
}

bool USTUHealthComponent::IsHealthFull() const
{
    return FMath::IsNearlyEqual(Health, MaxHealth);
}

void USTUHealthComponent::PlayCameraShake()
{
    if (IsDead())
    {
        return;
    }

    const auto Player = Cast<APawn>(GetOwner());
    if (!Player)
    {
        return;
    }

    const auto Controller = Player->GetController<APlayerController>();
    if (!Controller || !Controller->PlayerCameraManager)
    {
        return;
    }

    Controller->PlayerCameraManager->StartCameraShake(CameraShake);
}

void USTUHealthComponent::Killed(AController* KillerController)
{
    if (!GetWorld())
    {
        return;
    }
    const auto GameMode = Cast<ASTUGameModeBase>(GetWorld()->GetAuthGameMode());

    if (!GameMode)
    {
        return;
    }

    APawn* Player = Cast<APawn>(GetOwner());
    AController* VictimController = Player ? Player->GetController() : nullptr;

    GameMode->Killed(KillerController, VictimController);
}