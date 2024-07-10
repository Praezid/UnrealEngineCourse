// Shoot Them Up Game, All Rights Reserved


#include "Pickups/STUAmmoPickup.h"

DEFINE_LOG_CATEGORY_STATIC(AmmoPickupLog, All, All);


bool ASTUAmmoPickup::GivePickupTo(AActor* Actor)
{
    UE_LOG(AmmoPickupLog, Display, TEXT("Ammo was taken"));
    return true;
}