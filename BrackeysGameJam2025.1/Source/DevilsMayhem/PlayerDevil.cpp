// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDevil.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
APlayerDevil::APlayerDevil()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set Up Sring Arm
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
    SpringArm->SetupAttachment(RootComponent);
	//Set Up Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);


	// Create an Audio Component
    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudio"));
    AudioComponent->SetupAttachment(RootComponent);
    AudioComponent->bAutoActivate = false; // Prevent auto-playing
    AudioComponent->SetVolumeMultiplier(1.0f);
    bIsMoving = false;

}

// Called when the game starts or when spawned
void APlayerDevil::BeginPlay()
{
	Super::BeginPlay();

	bCanBecomeInvisible = true;
	bCanDash = true;
	bIsInvisible = false;
	bIsDashing = false;


}

// Called every frame
void APlayerDevil::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bIsDashing)
	{
		DashProgress += DeltaTime / DashTime;
        SetActorLocation(FMath::Lerp(DashStart, DashEnd, DashProgress),true);
	}


	// Check if moving along X-axis
	bool bCurrentlyMoving = GetVelocity().SizeSquared() > 0;
	bool bIsGrounded = GetCharacterMovement()->IsMovingOnGround(); 
	if (bCurrentlyMoving && bIsGrounded && !bIsMoving) // Started moving
	{
		if(!bIsInvisible)
		{
			if (FootstepSound && !AudioComponent->IsPlaying())
			{
				AudioComponent->SetSound(FootstepSound);
				AudioComponent->Play();
			}
		}
		bIsMoving = true;
	}
	else if ((!bCurrentlyMoving || !bIsGrounded) && bIsMoving) // Stopped moving
	{
		AudioComponent->Stop();
		bIsMoving = false;
	}

}

// Called to bind functionality to input
void APlayerDevil::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
    // Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerDevil::Move);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerDevil::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Invisible
		EnhancedInputComponent->BindAction(InvisibleAction, ETriggerEvent::Completed, this, &APlayerDevil::TurnInvisible);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerDevil::Dash);

		// Dashing
		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Triggered, this, &APlayerDevil::PauseGame);
	}
}

void APlayerDevil::Move(const FInputActionValue& Value)
{	
	float KeyPressed = Value.Get<float>();
	LastDirectionPressed = KeyPressed != 0? KeyPressed : LastDirectionPressed;
	if(Controller != nullptr)
	{
		// find out which way is forward
		//const FRotator Rotation = Controller->GetControlRotation();
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		const FRotator Rotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		const FRotator PitchRotation(Rotation.Pitch, 0, 0);
		const FVector ForwardDirection = FRotationMatrix(PitchRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, KeyPressed);
	}
	//UE_LOG(LogTemp, Warning, TEXT("Move"));
}

void APlayerDevil::Jump(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Jump"));
}

void APlayerDevil::Dash(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Dash"));
	if(bCanDash && !bIsDashing)
	{
		bIsDashing = true;
		bCanDash = false;

		DashStart = GetActorLocation();
		DashEnd = DashStart + GetActorForwardVector() * DashSpeed * LastDirectionPressed;		
		DashProgress = 0.f;
		PlayDashEffect();
        GetWorldTimerManager().SetTimer(StopDashTimer, this, &APlayerDevil::StopDash,DashTime, false);
	}
}

void APlayerDevil::StopDash()
{
	bIsDashing = false;

    // Start cooldown before next dash
	GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &APlayerDevil::DashCooldownReset,DashCooldown, false);
}

void APlayerDevil::DashCooldownReset()
{
	bCanDash = true;
}

void APlayerDevil::PlayDashEffect()
{
	if (DashEffect)
    {
		FRotator DashRotation = GetActorRotation() + FRotator(0, 90, 0) * LastDirectionPressed;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), // World context
			DashEffect, // Niagara system asset
			GetActorLocation(), // Spawn at offset
			DashRotation, // Default rotation
			FVector(1, 1, 1), // Default scale
			true, // Auto destroy when done
			true // Auto activate
		);
	}
	if(DashingSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashingSound, GetActorLocation(),SoundMultiplier);
	}
}


void APlayerDevil::TurnInvisible(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Invisible"));

	if(bCanBecomeInvisible && !bIsInvisible)
	{
		bCanBecomeInvisible = false;
		bIsInvisible = true;
		if(TurnInvisibleSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, TurnInvisibleSound, GetActorLocation(),SoundMultiplier);
		}
		ApplyTurnInvisibleEffect();
		GetWorldTimerManager().SetTimer(StopInvisibilityTimer, this, &APlayerDevil::StopInvisibility,InvisibilityDuration, false);
	}
}

void APlayerDevil::StopInvisibility()
{
	bIsInvisible = false;
	BecomeVisible();
	GetWorldTimerManager().SetTimer(InvisibleCooldownTimer, this, &APlayerDevil::InvisibleCooldownReset,InvisibilityCooldown, false);
}

void APlayerDevil::InvisibleCooldownReset()
{
	bCanBecomeInvisible = true;
} 

bool APlayerDevil::IsPlayerInvisible() const
{
	return bIsInvisible;
}

void APlayerDevil::GotHit()
{
	if(DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}
	ShowGameOverWidget();
	//UE_LOG(LogTemp, Warning, TEXT("Got Hit"));
}

bool APlayerDevil::GetCanDash() const
{
	return bCanDash;
}


bool APlayerDevil::GetCanBecomeInvisible() const
{
	return bCanBecomeInvisible;
}

