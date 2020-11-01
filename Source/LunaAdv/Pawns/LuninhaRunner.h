// Lucas Stern Games

#pragma once

#include "CoreMinimal.h"

#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "LuninhaRunner.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class LUNAADV_API ALuninhaRunner final : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALuninhaRunner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* LaneSwapCurve;

	/** Timeline controlling the movement when switching lanes
	 * @see SwapLane()
	 */
	FTimeline LaneSwapTimeLine;

	/** Array that saves the positions of all the lanes of the current tile
	 * @see UpdateLanesPosition()
	 */
	TArray<FVector> LanesPositions;

	/* The lane number where the character is currently on */
	int CurrentLane{ 1 };

	/* The desired lane position on world for lane movement */
	float TargetPosition;

	/* Change the current lane to the one on the right. Clamped to the amount of lanes of LanesPos Vector */
	UFUNCTION()
	void SwapLeft();

	/* Change the current lane to the one on the right. Clamped to the amount of lanes of LanesPos Vector */
	UFUNCTION()
	void SwapRight();

	/**
	 * @brief Move character to another lane based on the current lane and on the player input.
	 * Base the movement speed of the character using the SwapLaneCurve
	 * @see SwapRight(), SwapLeft() 
	 */
	UFUNCTION()
	void SwapLane();

	/* Get and save the lane positions (TArray LanesPositions) of the ground current stepping on */
	void UpdateLanesPosition();
};
