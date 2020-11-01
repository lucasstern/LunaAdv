// Lucas Stern Games

#include "StepPlatform.h"
#include "Kismet/KismetSystemLibrary.h"
#include "LunaAdv/Pawns/Luna.h"

// Sets default values
AStepPlatform::AStepPlatform()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlatformBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformBody"));
	RootComponent = PlatformBody;

	//PlatformBody->SetSimulatePhysics(true);
	PlatformBody->SetNotifyRigidBodyCollision(true); //Enable hit events
	PlatformBody->OnComponentHit.AddDynamic(this, &AStepPlatform::OnHit);
}

// Called when the game starts or when spawned
void AStepPlatform::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AStepPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FHitResult AStepPlatform::CheckCollision() const
{
	const FVector PlatformExtend{ PlatformBody->Bounds.BoxExtent };
	const FVector PlatformLocation{ PlatformBody->GetComponentLocation() };
	const FVector PlatformUpVector{ PlatformBody->GetUpVector() };


	const FVector BoxStart{ FVector(PlatformLocation.X, PlatformLocation.Y, PlatformLocation.Z + PlatformExtend.Z) };
	const FVector BoxEnd{ PlatformUpVector * 20 + BoxStart };
	const FVector BoxHalfSize{ PlatformExtend.X * 0.75f, PlatformExtend.Y * 0.75f, 0.0f };;

	const TArray<AActor*> ActorsToIgnore;

	FHitResult HitResult;

	UKismetSystemLibrary::BoxTraceSingle(GetWorld(), BoxStart, BoxEnd, BoxHalfSize, FRotator(0, 0, 0),
	                                     ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore,
	                                     EDrawDebugTrace::ForOneFrame, HitResult, true);

	return HitResult;
}

void AStepPlatform::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                          UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FHitResult AboveHit{ CheckCollision() };

	if(AboveHit.Actor.Get() != nullptr)
	{
		ALuna* Player = Cast<ALuna>(AboveHit.Actor.Get());
		if(Player)
		{
			Player->Land(this);
		}
	};
}
