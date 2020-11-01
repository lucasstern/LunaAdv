// Lucas Stern Games

#pragma once

#include "CoreMinimal.h"
#include "Pickups_Base.h"
#include "Coin.generated.h"

class UArrowComponent;

UCLASS()
class LUNAADV_API ACoin : public APickups_Base
{
	GENERATED_BODY()

public:
	FVector GetNextCoinLocation() const;
	
private:

public:
	ACoin();
};
