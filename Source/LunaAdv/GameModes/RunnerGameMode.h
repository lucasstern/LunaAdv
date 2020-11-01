// Lucas Stern Games

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LunaAdv/Pickups/Pickups_Base.h"

#include "RunnerGameMode.generated.h"

class AGroundTile;

UCLASS()
class LUNAADV_API ARunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Tile")
	FTransform NextSpawnTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ground Tile")
	AGroundTile* LastSpawnedTile;

	/**
	 * @brief Spawn a new floor title, attached to the last spawned AttachPoint
	 * @param bDestroyOldTile Controls whether it should or not not destroy the back most tile when spawning a new one  
	 */
	UFUNCTION(BlueprintCallable, Category="Ground Tile")
	void SpawnTitle(bool bDestroyOldTile);

	/** Add the Amount to the player score. Pass a negative value if you want to take a score. Clamped to 0 */
	UFUNCTION(BlueprintSetter, Category = "Score")
	void AddScorePoints(const int32 Amount);

	/**
	 * @return Current player score 
	 */
	UFUNCTION(BlueprintGetter, Category = "Score")
	int32 GetScorePoints() const
	{
		return ScorePoints;
	}

	/** Increase player speed over time */
	UFUNCTION(BlueprintCallable, Category = "Difficult")
	void ImproveSpeed() const;

protected:
	/** Type of tile to Spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Tile")
	TSubclassOf<AGroundTile> TileToSpawn;

	/** Number of current tiles on game */ 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ground Tile")
	int32 NumTiles{};

	/** Type of coin to Spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Score")
	TSubclassOf<APickups_Base> CoinType;

	
	/** Save the current player score */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, BlueprintSetter=AddScorePoints, BlueprintGetter=GetScorePoints,
		Category = "Score")
	int32 ScorePoints{};

	/** Base chance to spawn a coin whenever its spawn a new tile */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Score", meta=(ClampMin="0", ClampMax="1"))
	float CoinSpawnChance{ 0.5 };

	/** Base chance to keep spawning a coin after the first one */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Score", meta=(ClampMin="0", ClampMax="1"))
	float CoinComboChance{ 0.75 };
	
	/** Ratio to increase movement speed when passing for each tile	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficult")
	float SpeedImprovementRadio{ 1.02f }; // TODO  Talvez converter para uma float curve

	/** Starting speed of the player pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficult")
	float StartingSpeed{ 600.f };

	/** Max speed of the player pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Difficult")
	float MaxSpeed{ 2500.f };

	virtual void BeginPlay() override;

private:
	/* Controls the current spawned tiles */
	TQueue<AGroundTile*> SpawnedTiles;

	/* The controlled pawn */
	class ALuninhaRunner* PlayerPawn{};

	/* Dequeue and destroy the oldest tile from SpawnedTiles queue */
	UFUNCTION()
	void DestroyOldestTile();

	UFUNCTION()
	void SpawnCoin();
};
