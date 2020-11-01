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

	// Spawn a initial set of grounds
	for
	(int i = 0; i < 10; ++i)
	{
		SpawnTitle(false);
	}

	//Set initial player speed
	PlayerPawn->GetCharacterMovement()->MaxWalkSpeed = StartingSpeed;
}

void ARunnerGameMode::DestroyOldestTile()
{
	AGroundTile* TileToDestroy{};

	SpawnedTiles.Dequeue(TileToDestroy);

	TileToDestroy->Destroy();
	NumTiles--;
}

void ARunnerGameMode::SpawnCoin()
{
	if(!LastSpawnedTile) return;

	//Spawn based on chance
	if(UKismetMathLibrary::RandomBoolWithWeight(CoinSpawnChance))
	{
		TArray<FVector> LanesPos;
		LastSpawnedTile->GetLanesPosition(LanesPos);

		FVector SpawnLocation{ LanesPos[UKismetMathLibrary::RandomIntegerInRange(0, LanesPos.Num() - 1)] };

		//Spawn a new coin
		APickups_Base* Coin = GetWorld()->SpawnActor<APickups_Base>(CoinType, SpawnLocation, FRotator(), FActorSpawnParameters());
		
		Coin->AttachToActor(LastSpawnedTile, FAttachmentTransformRules::KeepWorldTransform);
		
	} else
	{
		GEngine->AddOnScreenDebugMessage(0, 0.f, FColor::Red,
		                                 FString::Printf(TEXT("Tile %d will not spawn a coin"), NumTiles));
	}
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

	SpawnCoin();
	
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
