// Lucas Stern Games

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "StepPlatform.generated.h"

UCLASS()
class LUNAADV_API AStepPlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AStepPlatform();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PlatformBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* SurfaceHitTrigger;

	/* draw a box ray trace aboce the platform, returning the hit result of the same */
	FHitResult CheckCollision() const;
};
