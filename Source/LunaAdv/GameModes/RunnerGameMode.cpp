// Lucas Stern Games

#include "RunnerGameMode.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LunaAdv/Architecture/GroundTile.h"
#include "LunaAdv/Pawns/LuninhaRunner.h"
#include "Kismet/KismetMathLibrary.h"

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

APickups_Base* ARunnerGameMode::SpawnCoin(AGroundTile* TileToAttach, FVector SpawnLocation)
{
	if(!TileToAttach) return nullptr;

	//Spawn a new coin
	APickups_Base* Coin = GetWorld()->SpawnActor<APickups_Base>(CoinType, SpawnLocation, FRotator(),
	                                                            FActorSpawnParameters());

	Coin->AttachToActor(TileToAttach, FAttachmentTransformRules::KeepWorldTransform);

	return Coin;
}

void ARunnerGameMode::SpawnTitle(bool bDestroyOldTile)
{
	//Spawn a new tile
	LastSpawnedTile = GetWorld()->SpawnActor<AGroundTile>(TileToSpawn, NextSpawnTransform);

	//Offset Spawn Location so the tiles do not overlap each other
	FVector NewSpawnLocation{ LastSpawnedTile->GetAttachPointLocation() };
	NewSpawnLocation.Y += -LastSpawnedTile->GetTileSize().Y;

	//Set the location to spawn the next tile
	NextSpawnTransform.SetLocation(NewSpawnLocation);

	// Add the spawned tiles our control queue 
	SpawnedTiles.Enqueue(LastSpawnedTile);

	LastSpawnedTile->SpawnRandomObstacle();
	NumTiles++;
	
	//Spawn first coin based on chance
	if(UKismetMathLibrary::RandomBoolWithWeight(CoinSpawnChance))
	{
		TArray<FVector> LanesPos;
		LastSpawnedTile->GetLanesPosition(LanesPos);

		// Random choose the lane to spawn the coin
		FVector SpawnLocation{ LanesPos[UKismetMathLibrary::RandomIntegerInRange(0, LanesPos.Num() - 1)] };
		
		APickups_Base* LastCoin = SpawnCoin(LastSpawnedTile, SpawnLocation);

		while(UKismetMathLibrary::RandomBoolWithWeight(CoinComboChance))
		{

			GEngine->AddOnScreenDebugMessage(12, 15.f, FColor::Black,
                                 FString::Printf(TEXT("Tile %d will spawn a COMBO coin. Chance: %f"), NumTiles, CoinComboChance));
			const FVector NewLocationOffset(LastCoin->GetActorLocation().X, LastCoin->GetActorLocation().Y - CoinYOffset, LastCoin->GetActorLocation().Z);
			
			LastCoin = SpawnCoin(LastSpawnedTile, NewLocationOffset);
			
			SetCoinComboChance(CoinComboChancePenalty);
		}
		ResetCoinComboChance();
	} else
	{
		GEngine->AddOnScreenDebugMessage(10, 15.f, FColor::Red,
		                                 FString::Printf(TEXT("Tile %d will not spawn a coin"), NumTiles));
	}

	if(bDestroyOldTile) DestroyOldestTile();
}

void ARunnerGameMode::AddScorePoints(const int32 Amount)
{
	ScorePoints = FMath::Clamp(ScorePoints + Amount, 0, TNumericLimits<int32>::Max());
}

void ARunnerGameMode::ImproveSpeed() const
{
	if(PlayerPawn)
	{
		float NewWalkSpeed{ PlayerPawn->GetCharacterMovement()->MaxWalkSpeed *= SpeedImprovementRadio };
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
