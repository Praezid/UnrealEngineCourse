// Shoot Them Up Game, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "STUNeedAmmoDecorator.generated.h"

class ASTUBaseWeapon;


UCLASS()
class SHOOTTHEMUP_API USTUNeedAmmoDecorator : public UBTDecorator
{
	GENERATED_BODY()
public:
    USTUNeedAmmoDecorator();

protected:
    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "AI")
    TSubclassOf<ASTUBaseWeapon> WeaponType;

    UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "AI", meta = (ClampMin = "1", ClampMax = "5"))
    int32 MinClips = 1;

    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
