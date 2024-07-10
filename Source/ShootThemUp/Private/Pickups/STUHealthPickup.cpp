// Shoot Them Up Game, All Rights Reserved


#include "Pickups/STUHealthPickup.h"

DEFINE_LOG_CATEGORY_STATIC(HealthPickupLog, All, All);

bool ASTUHealthPickup::GivePickupTo(AActor* Actor)
{
    UE_LOG(HealthPickupLog, Display, TEXT("Health was taken"));
    return true;
}