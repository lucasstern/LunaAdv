// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "Luna.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UArrowComponent;
class AStepPlatform;

UCLASS()
class LUNAADV_API ALuna : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALuna();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**	Jump, using the value set in JumpImpulse as the force */
	UFUNCTION(BlueprintCallable, Category = "Jumping")
	void Jump();

	/**	Get if the cat is currently charging a jump */
	UFUNCTION(BlueprintGetter, Category = "Jumping")
	bool IsChargingJump() const { return bChargingJump; }

	UFUNCTION(BlueprintCallable, Category="Jumping")
	void ForceJumpImpulse(float NewImpulse);

	UFUNCTION(BlueprintCallable, Category="Movement")
	void SetPlatformBelow(AStepPlatform* Platform)
	{
		PlatformBelow = Platform;
	}

	/** Stop the payer from bouncing and allow to jump again */
	UFUNCTION(BlueprintCallable, Category="Movement")
	void Land(AStepPlatform* Platform);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/**
	* @brief Use atan2() to return the angle between the actor and the mouse location
	* @param bOnlyPositive = Return only positive (0 - 180) value
	* @see https://www.youtube.com/watch?v=CEitcZWFr3Y&list=PLxM99r8wgKfGmpQiggGwUfM-F-GPg5qh9&index=3
	* @return Angle (degrees) between the actor and the mouse location  
	*/
	UFUNCTION(BlueprintPure, Category = "Position")
	float GetDegreesToMouse(bool bOnlyPositive) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Body;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	AStepPlatform* PlatformBelow{};

	UPROPERTY(EditDefaultsOnly, Category = "Jumping", meta = (AllowPrivateAccess = "true"))
	float ImpulseChargeSpeed{ 100.f };

	UPROPERTY(VisibleAnywhere, Category = "Jumping", meta = (AllowPrivateAccess = "true"))
	float JumpImpulse{ 0.f };

	UPROPERTY(EditDefaultsOnly, Category = "Jumping", meta = (AllowPrivateAccess = "true", ClampMin = 1.f))
	float MaxJumpImpulse{ 1000.f };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, BlueprintGetter=IsChargingJump,
		Category = "Jumping", meta = (AllowPrivateAccess = "true"))
	bool bChargingJump{ false };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jumping", meta = (AllowPrivateAccess = "true"))
	bool bIsOnFloor{ true };

	/** Start to charge the jumping before releasing */
	UFUNCTION(Category = "Jumping")
	void StartChargingJump();

	/** @brief If a arrow component is found, change the pitch rotation to point to the mouse */
	void AimArrowToMouse() const;

	/** Get time the jump key is being pressed and convert it to impulse to launch the cat */
	void ChargeJump();
	void StopBouncing();
};
