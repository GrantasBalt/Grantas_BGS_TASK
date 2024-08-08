#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h" 
#include "Components/CapsuleComponent.h"
#include "SkateboardCharacter.generated.h" 

UCLASS()
class GRANTAS_BGS_TASK_API ASkateboardCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ASkateboardCharacter();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsHoldingForward;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void SpeedUp();
	void SlowDown();
	void StartJump();
	void StopJump();
	void ApplyLeaning();

	float SpeedMultiplier;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float DefaultSpeed;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float LeanAmount;

	FVector PreviousVelocity;
	bool bIsAccelerating;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AccelerationThreshold;

	
};
