// Lucas Stern Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GroundTile.generated.h"

class UArrowComponent;
class UBoxComponent;
class AObstacle;

UCLASS()
class LUNAADV_API AGroundTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGroundTile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetAttachPointLocation() const;

	FVector GetTileSize() const;

	/** Return the position of the 3 lanes on the world space */
	UFUNCTION(BlueprintCallable, Category = "Lanes")
	void GetLanesPosition(TArray<FVector>& LanesPos) const { LanesPos = LanesPositions; }

	UFUNCTION(BlueprintCallable, Category = "Obstacles")
	void SpawnRandomObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Amounts of score to give the player whenever they reach the end of a ground tile */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 ScoreToGive{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacles")
	TArray<TSubclassOf<AObstacle>> ObstaclesModels;

	/** Destroy the attached obstacles too */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* AttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* SpawnTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* LeftLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* MidLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* RightLane;

	TArray<FVector> LanesPositions;

	AGameModeBase* GameMode{};

	/* Call the GameMode to spawn a new tile whenever the player hit the end of the current tile */
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void PopulateLanesPosition();

};
