#include "SkateboardCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"

// Sets default
ASkateboardCharacter::ASkateboardCharacter()
{
    //call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize speed values
    DefaultSpeed = 600.0f;
    MaxSpeedMultiplier = 2.0f;
    SpeedMultiplier = 1.0f;

    // Initialize lean
    LeanAmount = 0.0f;
}
void ASkateboardCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Configure properties for glide effect
    GetCharacterMovement()->bUseSeparateBrakingFriction = true;
    GetCharacterMovement()->BrakingFriction = 0.2f;  
    GetCharacterMovement()->BrakingDecelerationWalking = 1.0f;  
    GetCharacterMovement()->GroundFriction = 0.1f;  
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;  
    GetCharacterMovement()->MaxAcceleration = 300.0f;  
}


void ASkateboardCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector Velocity = GetVelocity();
    FVector HorizontalVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);

    // Prolong the free ride
    if (FMath::IsNearlyZero(HorizontalVelocity.Size(), 0.1f))
    {
        FVector Deceleration = -HorizontalVelocity.GetSafeNormal() * 2.0f * DeltaTime; 
        GetCharacterMovement()->Velocity += Deceleration;
    }

    // Downhill acceleration 
    if (GetCharacterMovement()->IsMovingOnGround())
    {
        FVector GroundNormal = GetCharacterMovement()->CurrentFloor.HitResult.Normal;
        FVector DownhillDirection = FVector::VectorPlaneProject(FVector::DownVector, GroundNormal).GetSafeNormal();

        if (DownhillDirection.Z < -0.1) // Check if on a downhill slope
        {
            GetCharacterMovement()->Velocity += DownhillDirection * 30.0f * DeltaTime;  // Increase downhill boost
        }
    }
}




// Bind functionality to input
void ASkateboardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Bind movement inputs
    PlayerInputComponent->BindAxis("MoveForward", this, &ASkateboardCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ASkateboardCharacter::MoveRight);

    // Bind action inputs
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASkateboardCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASkateboardCharacter::StopJump);
    PlayerInputComponent->BindAction("SpeedUp", IE_Pressed, this, &ASkateboardCharacter::SpeedUp);
    PlayerInputComponent->BindAction("SlowDown", IE_Pressed, this, &ASkateboardCharacter::SlowDown);
}
void ASkateboardCharacter::MoveForward(float Value)
{
    if (Controller)
    {
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);
        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        bIsHoldingForward = (Value > 0.0f);

        if (Value > 0.0f)
        {
            // Move forward and adjust speed
            AddMovementInput(Direction, Value);
            SpeedMultiplier = FMath::Clamp(SpeedMultiplier + 0.05f * Value, 1.0f, MaxSpeedMultiplier);
        }
        else if (Value < 0.0f)
        {
            // Slow down to stop
            if (GetVelocity().Size() > 10.0f)
            {
                SpeedMultiplier = FMath::Clamp(SpeedMultiplier - 0.05f, 1.0f, MaxSpeedMultiplier);
            }
            else
            {
                // Allow backward movement after stopping
                SpeedMultiplier = 1.0f; // Reset to default speed
                AddMovementInput(-Direction, -Value);
            }
        }

        // Update character's max walk speed based on the speed multiplier
        GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * SpeedMultiplier;
    }
}

void ASkateboardCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        // Get the current forward speed
        float ForwardSpeed = FVector::DotProduct(GetVelocity(), GetActorForwardVector());

        // Only allow side movement if there is significant forward speed
        if (ForwardSpeed > 10.0f) // Adjust threshold as needed
        {
            const FRotator Rotation = Controller->GetControlRotation();
            const FVector Direction = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
            AddMovementInput(Direction, Value);

            // Update LeanAmount for leaning visual effect
            LeanAmount = Value;
        }
        else
        {
            // Reset LeanAmount if not moving forward
            LeanAmount = 0.0f;
        }
    }
    else
    {
        // Reset LeanAmount when there is no input
        LeanAmount = 0.0f;
    }

    // Manage leaning
    ApplyLeaning();
}

void ASkateboardCharacter::StartJump()
{
    bPressedJump = true;
}

void ASkateboardCharacter::StopJump()
{
    bPressedJump = false;
}

void ASkateboardCharacter::SpeedUp()
{
    SpeedMultiplier = FMath::Clamp(SpeedMultiplier + 0.1f, 1.0f, MaxSpeedMultiplier);
    GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * SpeedMultiplier;
}

void ASkateboardCharacter::SlowDown()
{
    SpeedMultiplier = FMath::Clamp(SpeedMultiplier - 0.1f, 0.0f, MaxSpeedMultiplier);
    GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed * SpeedMultiplier;
}

void ASkateboardCharacter::ApplyLeaning()
{
    if (GetCapsuleComponent())
    {
        // LeanFactor controls the degree of tilt per unit of input
        float LeanFactor = 10.0f;

        FRotator DesiredRotation = FRotator(0.0f, 0.0f, LeanAmount * LeanFactor);

        GetCapsuleComponent()->SetRelativeRotation(DesiredRotation);
    }
}







