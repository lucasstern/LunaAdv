// Fill out your copyright notice in the Description page of Project Settings.

#include "Luna.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LunaAdv/Buildings/StepPlatform.h"

// Sets default values
ALuna::ALuna()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/// COMPONENTS INIT
	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	RootComponent = Body;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);

	Body->SetSimulatePhysics(true);


}

// Called to bind functionality to input
void ALuna::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ALuna::StartChargingJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ALuna::Jump);
}

void ALuna::Land(AStepPlatform* Platform)
{
	UE_LOG(LogTemp, Warning, TEXT("Player has landed"));
	bIsOnFloor = true; // Prevents double jumping
	StopBouncing();
	PlatformBelow = Platform;
}

// Called when the game starts or when spawned
void ALuna::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ALuna::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimArrowToMouse();

	if(bChargingJump)
	{
		ChargeJump();
	}

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, FString::Printf(TEXT("Jump Impulse: %f"), JumpImpulse));

		GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red,
		                                 FString::Printf(TEXT("Can Jump: %s"), bIsOnFloor ? TEXT("Yes") : TEXT("No")
		                                 ));

		GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Red,
		                                 FString::Printf(
			                                 TEXT("On A Surface: %s"),
			                                 PlatformBelow != nullptr ? TEXT("Yes") : TEXT("No")));

		if(PlatformBelow)
		{
			GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::Blue,
                                 FString::Printf(
                                     TEXT("Surface: %s"),
                                     *PlatformBelow->GetName()));
			// Keep the actor and the platform moving together
			UE_LOG(LogTemp, Warning, TEXT("%s"), *PlatformBelow->GetVelocity().ToString());
			Body->SetAllPhysicsLinearVelocity(PlatformBelow->GetRootComponent()->GetComponentVelocity());
		}
	}
}

void ALuna::StartChargingJump()
{
	// Only starting to char a jumpwhen is on a floor
	if(bIsOnFloor)
	{
		bChargingJump = true;
	}
}

void ALuna::ChargeJump()
{
	if(JumpImpulse < MaxJumpImpulse)
	{
		JumpImpulse += ImpulseChargeSpeed;
	} else
	{
		if(GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red,
			                                 FString::Printf(TEXT("Max Jump Impulse reached!!")));
		}
	}
}

void ALuna::StopBouncing()
{
	// Stop bouncing when falling
	FVector NewVel{ Body->GetPhysicsLinearVelocity() };
	NewVel.Z = 0;
	Body->SetAllPhysicsLinearVelocity(NewVel);
}

void ALuna::Jump()
{
	//Stop charching
	bChargingJump = false;

	//build the vector used to define where the impulse will target
	const float TargetDegrees{ GetDegreesToMouse(true) };

	// Impulse Target, converting deegres to radians using *PI/180
	const float ZValue{ FMath::Sin(TargetDegrees * PI / 180) };
	const float YValue{ FMath::Cos(TargetDegrees * PI / 180) };

	// Negative in Y otherwise will jump the opposite direction
	Body->AddImpulse(FVector(0, YValue * (-JumpImpulse), ZValue * JumpImpulse));

	bIsOnFloor = false;
	PlatformBelow = nullptr;

	// Reset JumpImpulse
	JumpImpulse = 0.f;
}

void ALuna::ForceJumpImpulse(float NewImpulse)
{
	JumpImpulse = NewImpulse;
}

void ALuna::AimArrowToMouse() const
{
	if(!Arrow) return;

	Arrow->SetWorldRotation(FRotator(GetDegreesToMouse(true), -90, 0));
}

float ALuna::GetDegreesToMouse(bool bOnlyPositive) const
{
	if(!GetWorld()->GetFirstPlayerController()) return 0.f;

	// Get mouse position on screen
	float MouseX;
	float MouseY;
	GetWorld()->GetFirstPlayerController()->GetMousePosition(MouseX, MouseY);

	// Converts the location of this actor to screen, so the mouse and the actor is in the same "plane"
	FVector2D ScreenLocation;
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), this->GetActorLocation(),
	                                       ScreenLocation);

	// Get the angle between the actor and the mouse screen location (in radians)
	// https://upload.wikimedia.org/wikipedia/commons/thumb/a/ad/Atan2definition.svg/1024px-Atan2definition.svg.png
	const float Radians{ FMath::Atan2(ScreenLocation.Y - MouseY, ScreenLocation.X - MouseX) };

	const float Degrees{ FMath::RadiansToDegrees(Radians) };

	if(bOnlyPositive)
	{
		return FMath::ClampAngle(Degrees, 0, 180);
	}

	return Degrees;
}
