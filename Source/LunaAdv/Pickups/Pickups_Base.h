// Lucas Stern Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickups_Base.generated.h"

class USoundCue;
class UBoxComponent;


UCLASS(Abstract)
class LUNAADV_API APickups_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickups_Base();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetPickupAreaBounds() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* What to do when being overlapped */
	virtual void OnPickup() PURE_VIRTUAL(APickups_Base::OnPickup);
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	/* Area then OnPuckup() will be triggered, on being overlap */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* PickupArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USoundCue* PickupSound;
};
