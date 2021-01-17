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
	ACoin();
	
	FVector GetNextCoinLocation() const;

	void SetActorsOverlapping(const TArray<AActor*> &ActorsArray);

protected:
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Position", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> ActorsOverlapping;


protected:
	virtual void OnPickup() override;
};
