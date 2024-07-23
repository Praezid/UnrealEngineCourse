// Shoot Them Up Game, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AI/EQS/EnvQueryTest_PickupCouldBeTaken.h"
#include "EnvQueryTest_AmmoCouldBeTaken.generated.h"

class ASTUBaseWeapon;

UCLASS()
class SHOOTTHEMUP_API UEnvQueryTest_AmmoCouldBeTaken : public UEnvQueryTest_PickupCouldBeTaken
{
	GENERATED_BODY()

public:
    virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TSubclassOf<ASTUBaseWeapon> WeaponType;
};
