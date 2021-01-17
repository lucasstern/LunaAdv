// Lucas Stern Games

#include "RunnerGameMode.h"


#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LunaAdv/Architecture/GroundTile.h"
#include "LunaAdv/Pawns/LuninhaRunner.h"
#include "LunaAdv/Pickups/Coin.h"

void ARunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = Cast<ALuninhaRunner>(GetWorld()->GetFirstPlayerController()->GetPawn());

	//Set initial player speed
	PlayerPawn->GetCharacterMovement()->MaxWalkSpeed = StartingSpeed;

	CoinComboChance = InitialCoinComboChance;
	CoinSpawnChance = InitialCoinSpawnChance;

	// Spawn a initial set of grounds
	for
	(int i = 0; i < 10; ++i)
	{
		SpawnTitle(false);
	}
}

void ARunnerGameMode::DestroyOldestTile()
{
	AGroundTile* TileToDestroy{};

	SpawnedTiles.Dequeue(TileToDestroy);

	TileToDestroy->Destroy();
	NumTiles--;
}

ACoin* ARunnerGameMode::SpawnCoin(AGroundTile* TileToAttach, FVector SpawnLocation) const
{
	if (!TileToAttach) return nullptr;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = TileToAttach;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	//Spawn a new coin
	ACoin* Coin = GetWorld()->SpawnActor<ACoin>(
		CoinType, FVector(SpawnLocation.X, SpawnLocation.Y, TileToAttach->GetActorLocation().Z + 50.f),
		FRotator(0.f, 0.f, 0.f),
		SpawnInfo);

	//Spawned Successful
	if (Coin != nullptr)
	{
		//Attach Coin to tile (this way when to tile is destroyed, the coin is too)
		Coin->AttachToActor(TileToAttach, FAttachmentTransformRules::KeepWorldTransform);
	}
	return Coin;
}

void ARunnerGameMode::SpawnTitle(bool bDestroyOldTile)
{
	//Spawn a new tile
	LastSpawnedTile = GetWorld()->SpawnActor<AGroundTile>(TileToSpawn, NextSpawnTransform);

	//Offset Spawn Location so the tiles do not overlap each other
	FVector NewSpawnLocation{LastSpawnedTile->GetAttachPointLocation()};
	NewSpawnLocation.Y += -LastSpawnedTile->GetTileSize().Y;

	//Set the location to spawn the next tile
	NextSpawnTransform.SetLocation(NewSpawnLocation);

	// Add the spawned tiles our control queue 
	SpawnedTiles.Enqueue(LastSpawnedTile);

	LastSpawnedTile->SpawnRandomObstacle();
	NumTiles++;

	//Spawn first coin based on chance
	if (UKismetMathLibrary::RandomBoolWithWeight(CoinSpawnChance))
	{
		// Randomically choose the lane to spawn the coin
		TArray<FVector> LanesPos;
		LastSpawnedTile->GetLanesPosition(LanesPos);
		FVector SpawnLocation{LanesPos[UKismetMathLibrary::RandomIntegerInRange(0, LanesPos.Num() - 1)]};

		ACoin* LastCoin{SpawnCoin(LastSpawnedTile, SpawnLocation)};

		// Spawn more coins based on chance
		while (UKismetMathLibrary::RandomBoolWithWeight(CoinComboChance) && LastCoin)
		{
			// Make the new coin spawn a little farther than the last one
			const FVector NewLocationOffset(LastCoin->GetActorLocation().X,
			                                LastCoin->GetActorLocation().Y - CoinYOffset,
			                                LastCoin->GetActorLocation().Z);

			LastCoin = SpawnCoin(LastSpawnedTile, NewLocationOffset);

			// Lower the chance to spawn the next coin
			SetCoinComboChance(CoinComboChancePenalty);
		}
		ResetCoinComboChance();
	}
	if (bDestroyOldTile) DestroyOldestTile();
}

void ARunnerGameMode::AddScorePoints(const int32 Amount)
{
	ScorePoints = FMath::Clamp(ScorePoints + Amount, 0, TNumericLimits<int32>::Max());
}

void ARunnerGameMode::ImproveSpeed() const
{
	if (PlayerPawn)
	{
		float NewWalkSpeed{PlayerPawn->GetCharacterMovement()->MaxWalkSpeed *= SpeedImprovementRadio};
		PlayerPawn->GetCharacterMovement()->MaxWalkSpeed = FMath::Clamp(NewWalkSpeed, StartingSpeed, MaxSpeed);
	}
}

void ARunnerGameMode::SetCoinComboChance(const float Value)
{
	FMath::Clamp<float>(CoinComboChance += Value, 0.f, 1.f);
}

void ARunnerGameMode::SetCoinSpawnChance(const float Value)
{
	FMath::Clamp<float>(CoinSpawnChance += Value, 0.f, 1.f);
}
