// Lucas Stern Games


#include "Pickups_Base.h"
#include "Components/BoxComponent.h"

// Sets default values
APickups_Base::APickups_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupArea = CreateDefaultSubobject<UBoxComponent>(FName("PickupArea"));
	RootComponent = PickupArea;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("Mesh"));
	StaticMesh->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void APickups_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickups_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector APickups_Base::GetPickupAreaBounds() const
{
	return PickupArea->GetScaledBoxExtent();
}

