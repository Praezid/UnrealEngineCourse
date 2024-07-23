// Shoot Them Up Game, All Rights Reserved

#include "AI/EQS/EnvQueryTest_AmmoCouldBeTaken.h"
#include "Weapon/STUBaseWeapon.h"
#include "Pickups/STUAmmoPickup.h"

void UEnvQueryTest_AmmoCouldBeTaken::RunTest(FEnvQueryInstance& QueryInstance) const
{
    const auto DataOwner = QueryInstance.Owner.Get();
    BoolValue.BindData(DataOwner, QueryInstance.QueryID);

    const bool WantsBeTakeble = BoolValue.GetValue();

    for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
    {
        const auto ItemActor = GetItemActor(QueryInstance, It.GetIndex());
        const auto PickupActor = Cast<ASTUAmmoPickup>(ItemActor);
        if (!PickupActor || PickupActor->GetWeaponType() != WeaponType) continue;
        //if (!PickupActor) continue;
        const auto CouldBeTaken = PickupActor->CouldBeTaken();
        It.SetScore(TestPurpose, FilterType, CouldBeTaken, WantsBeTakeble);
    }
}