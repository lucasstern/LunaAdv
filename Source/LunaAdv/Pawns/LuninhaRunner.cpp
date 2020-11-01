// Lucas Stern Games

#include "LuninhaRunner.h"

#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "LunaAdv/Architecture/GroundTile.h"

// Sets default values
ALuninhaRunner::ALuninhaRunner()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	LanesPositions.Init(FVector(), 3);
}

// Called when the game starts or when spawned
void ALuninhaRunner::BeginPlay()
{
	Super::BeginPlay();

	// Below is the configuratyion for our lane swap time line
	if(LaneSwapCurve) // Ensure timeline curve is set in editor
	{
		FOnTimelineFloat TimelineCallback;
		
		// Function to call when the timeline is updating
		TimelineCallback.BindUFunction(this, FName("SwapLane"));

		// Function to call when the timeline is finished
		//FOnTimelineEventStatic TimelineFinishedCallback;
		//TimelineFinishedCallback.BindUFunction(this, FName("FuncName"));
		// Set our function callback to be called when the timeline is finished
		//LaneSwapTimeLine.SetTimelineFinishedFunc(TimelineFinishedCallback);
		
		// Set the timeline curve
		LaneSwapTimeLine.AddInterpFloat(LaneSwapCurve, TimelineCallback);
		
	}
}

// Called to bind functionality to input
void ALuninhaRunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ALuninhaRunner::SwapLeft);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ALuninhaRunner::SwapRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ALuninhaRunner::Jump);
}

// Called every frame
void ALuninhaRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move forward for ever
	AddMovementInput(GetActorForwardVector());

	// Timeline run together with deltatime
	LaneSwapTimeLine.TickTimeline(DeltaTime);

	//Always check the available lanes on the current tile
	UpdateLanesPosition();

	//	UE_LOG(LogTemp, Warning, TEXT("%f"), GetCharacterMovement()->GetMaxSpeed());
	
}

void ALuninhaRunner::SwapLeft()
{
	// Get the desired position based on input, clamped by the total number os lanes
	// So its not possible to fall from the lanes
	int32 DesiredLane = FMath::Clamp(CurrentLane - 1, 0, LanesPositions.Num() - 1);

	// Get the X location of the desiredLane
	TargetPosition = LanesPositions[DesiredLane].X;

	// Play our timeline that will move the player
	LaneSwapTimeLine.PlayFromStart();

	// Save the new Lane
	CurrentLane = DesiredLane;
}

void ALuninhaRunner::SwapRight()
{
	// Get the desired position based on input, clamped by the total number os lanes
	// So its not possible to fall from the lanes
	int32 DesiredLane = FMath::Clamp(CurrentLane + 1, 0, LanesPositions.Num() - 1);

	// Get the X location of the desiredLane
	TargetPosition = LanesPositions[DesiredLane].X;

	// Play our timeline that will move the player
	LaneSwapTimeLine.PlayFromStart();

	// Save the new Lane
	CurrentLane = DesiredLane;
}

void ALuninhaRunner::SwapLane()
{
	const float TimeLineValue = LaneSwapTimeLine.GetPlaybackPosition(); // Get current timeline position
	const float CurveFloatValue = (LaneSwapCurve->GetFloatValue(TimeLineValue)); //Based on position, gives timeline value

	const FVector CurrentLocation = GetActorLocation();

	const float NewX = UKismetMathLibrary::Lerp(CurrentLocation.X, TargetPosition, CurveFloatValue);
	
	SetActorLocation(FVector(NewX, CurrentLocation.Y, CurrentLocation.Z));
}


void ALuninhaRunner::UpdateLanesPosition()
{
	FHitResult Hit;
	const FVector Start = GetActorLocation();
	const FVector DownVector = GetActorUpVector() * -1; //Inverse the vector to be pointing down
	const FVector End = DownVector * 100.f + Start;

	if(GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		if(Hit.GetActor()->GetClass()->IsChildOf(AGroundTile::StaticClass()))
		{
			// Get positions off all the lanes
			Cast<AGroundTile>(Hit.GetActor())->GetLanesPosition(LanesPositions);
		}
	}
}


