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

	UFUNCTION(BlueprintSetter, Category = "Coin Spawn")
	void SetCoinComboChance(const float Value);

	UFUNCTION(BlueprintSetter, Category = "Coin Spawn")
	void SetCoinSpawnChance(const float Value);

	UFUNCTION(BlueprintCallable, Category = "Coin Spawn")
	void ResetCoinComboChance()
	{
		CoinComboChance = InitialCoinComboChance;
	}

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

	/** Base chance to spawn a coin whenever its spawn a new tile */
	UPROPERTY(EditDefaultsOnly, BlueprintSetter=SetCoinSpawnChance, Category = "Coin Spawn", meta=(ClampMin="0", ClampMax="1"
	))
	float InitialCoinSpawnChance{ 0.75 };

	/** Base chance to keep spawning a coin after the first one */
	UPROPERTY(EditDefaultsOnly, BlueprintSetter=SetCoinComboChance, Category = "Coin Spawn", meta=(ClampMin="0", ClampMax="1"
	))
	float InitialCoinComboChance{ 0.75 };

	/** Current chance to spawn a coin whenever its spawn a new tile */
	UPROPERTY(EditInstanceOnly, BlueprintSetter=SetCoinSpawnChance, Category = "Coin Spawn", meta=(ClampMin="0", ClampMax="1"))
	float CoinSpawnChance;

	/** Current chance to keep spawning a coin after the first one */
	UPROPERTY(EditInstanceOnly, BlueprintSetter=SetCoinComboChance, Category = "Coin Spawn", meta=(ClampMin="0", ClampMax="1"))
	float CoinComboChance;

	/** How much to lower to CoinComboSpawnChance whenever a new coin is spawned */
	UPROPERTY(EditAnywhere, Category = "Coin Spawn", meta=(ClampMin="-1", ClampMax="0"))
	float CoinComboChancePenalty{ -0.05f };

	/** Space between each coin spawn */
	UPROPERTY(EditAnywhere, Category = "Coin Spawn", meta=(ClampMin="-1", ClampMax="0"))
	float CoinYOffset{ -50.f };
	
	/* Dequeue and destroy the oldest tile from SpawnedTiles queue */
	UFUNCTION()
	void DestroyOldestTile();

	/**
	 * @param TileToAttach The tile where te coin will spawn and be child of
	 * @param SpawnLocation Location inside the TileToAttach to spawn the coin
	 * @return Pointer to the coin that was spawned
	 */
	UFUNCTION()
	APickups_Base* SpawnCoin(AGroundTile* TileToAttach, FVector SpawnLocation);
};
