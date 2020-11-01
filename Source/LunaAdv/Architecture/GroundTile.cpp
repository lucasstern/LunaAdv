// Lucas Stern Games

#include "GroundTile.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LunaAdv/Buildings/Obstacle.h"
#include "LunaAdv/GameModes/RunnerGameMode.h"
#include "LunaAdv/Pawns/LuninhaRunner.h"

// Sets default values
AGroundTile::AGroundTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GameMode = UGameplayStatics::GetGameMode(AActor::GetWorld()); // get a ref to the current game mode

	SMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = SMesh;

	AttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("AttachPoint"));
	AttachPoint->SetupAttachment(RootComponent);

	SpawnTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnTrigger"));
	SpawnTrigger->SetupAttachment(RootComponent);

	SpawnTrigger->SetNotifyRigidBodyCollision(true); //Enables Hit Events
	SpawnTrigger->OnComponentBeginOverlap.AddDynamic(this, &AGroundTile::OnBeginOverlap);

	// Spawn the tree lanes location identifiers
	LeftLane = CreateDefaultSubobject<UArrowComponent>(TEXT("LeftLane"));
	LeftLane->SetupAttachment(RootComponent);
	MidLane = CreateDefaultSubobject<UArrowComponent>(TEXT("MiddleLane"));
	MidLane->SetupAttachment(RootComponent);
	RightLane = CreateDefaultSubobject<UArrowComponent>(TEXT("RightLane"));
	RightLane->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AGroundTile::BeginPlay()
{
	Super::BeginPlay();

	PopulateLanesPosition();
}


// Called every frame
void AGroundTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGroundTile::SpawnRandomObstacle()
{
	//int32 NumberOfLanesWithObstacles = FMath::RandRange(1, LanesPositions.Num());

	for(auto Lane : LanesPositions)
	{
		if(FMath::RandBool()) //50% chance to spawn or not a obstacle
		{
			//Ensure our obstacle array is populated
			if(ObstaclesModels.Num())
			{
				// Choose a random obstacle type on our obstacles array
				auto ToSpawn = (ObstaclesModels[FMath::RandRange(0, ObstaclesModels.Num() - 1)]);

				// Spawn the obstacle in the world
				AObstacle* NewObstacle = GetWorld()->SpawnActor<AObstacle>(ToSpawn, Lane, FRotator(0, 0, 0));

				NewObstacle->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			} else
			{
				UE_LOG(LogTemp, Error, TEXT("No obstacle type in the Obstacles Array for the groundtile.cpp"));
			}
		}
	}
}

FVector AGroundTile::GetAttachPointLocation() const
{
	return AttachPoint->GetComponentLocation();
}

FVector AGroundTile::GetTileSize() const
{
	return SMesh->GetStaticMesh()->GetBounds().BoxExtent * SMesh->GetComponentScale();
}

void AGroundTile::PopulateLanesPosition()
{
	LanesPositions.Init(FVector(), 3);
	LanesPositions[0] = LeftLane->GetComponentLocation();
	LanesPositions[1] = MidLane->GetComponentLocation();
	LanesPositions[2] = RightLane->GetComponentLocation();
}

void AGroundTile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);

	for(auto Obstacle : AttachedActors)
	{
		Obstacle->Destroy();
	}
}

void AGroundTile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                 const FHitResult& SweepResult)
{
	//TODO Isso aqui deveria ser um delegate sendo consumido pelo RunnerGameMode
	if(Cast<ALuninhaRunner>(OtherActor)) // assert is the player character
	{
		ARunnerGameMode* RunGMode{ Cast<ARunnerGameMode>(GameMode) };
		if(RunGMode)
		{
			// Then Spawn a new title whenever the player hit the end of the current one
			RunGMode->SpawnTitle(true);
			RunGMode->AddScorePoints(ScoreToGive);
			RunGMode->ImproveSpeed();
		}
	}
}
