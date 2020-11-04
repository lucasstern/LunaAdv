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

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = TileToAttach;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//Spawn a new coin
	APickups_Base* Coin = GetWorld()->SpawnActor<APickups_Base>(
		CoinType, FVector(SpawnLocation.X, SpawnLocation.Y, 10.f), FRotator(),
		SpawnInfo);
	if(Coin != nullptr)
	{
		Coin->AttachToActor(TileToAttach, FAttachmentTransformRules::KeepWorldTransform);

		//Handle Coin spawn location
		TArray<AActor*> Actorsoverlapping;
		Coin->GetOverlappingActors(Actorsoverlapping);

		//TODO WIP
		Actorsoverlapping.Sort([](const AActor& ActorA, const AActor& ActorB)
        {
            return ActorA.GetActorLocation().Z < ActorB.GetActorLocation().Z;
        });
		for(auto OtherActor : Actorsoverlapping)
		{
			const FVector OtherLocation{ OtherActor->GetActorLocation() };
			const FVector CoinLocation{ Coin->GetActorLocation() };
			Coin->SetActorLocation(FVector(CoinLocation.X, CoinLocation.Y, OtherLocation.Z + 10.f));
			break;
		}
	}
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
		// Randomically choose the lane to spawn the coin
		TArray<FVector> LanesPos;
		LastSpawnedTile->GetLanesPosition(LanesPos);
		FVector SpawnLocation{ LanesPos[UKismetMathLibrary::RandomIntegerInRange(0, LanesPos.Num() - 1)] };

		APickups_Base* LastCoin = SpawnCoin(LastSpawnedTile, SpawnLocation);

		// Spawn more coins based on chance
		while(UKismetMathLibrary::RandomBoolWithWeight(CoinComboChance) && LastCoin)
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
