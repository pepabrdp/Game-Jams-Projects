// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerDevil.generated.h"


class UInputAction;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAudioComponent;
struct FInputActionValue;


UCLASS()
class DEVILSMAYHEM_API APlayerDevil : public ACharacter
{
	GENERATED_BODY()

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Invisible Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InvisibleAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Pause Game Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseGameAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext* InputMapping;

public:
	// Sets default values for this character's properties
	APlayerDevil();

	UFUNCTION(BlueprintCallable)
	void GotHit(); 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Jump(const FInputActionValue& Value);
	
	/** Called for looking input */
	void Dash(const FInputActionValue& Value);

	/** Called for movement input */
	void TurnInvisible(const FInputActionValue& Value);

	UFUNCTION(BlueprintImplementableEvent)
	void PauseGame(const FInputActionValue& Value);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayDashEffect();

	UFUNCTION(BlueprintCallable)
	bool IsPlayerInvisible() const;

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyTurnInvisibleEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void BecomeVisible();

	//Game Over widget
	UFUNCTION(BlueprintImplementableEvent)
	void ShowGameOverWidget();


	UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* FootstepSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* TurnInvisibleSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* DashingSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* DeathSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, Category = "Audio")
	float SoundMultiplier = 2.0f;

	UFUNCTION(BlueprintCallable)
	bool GetCanDash() const;

	UFUNCTION(BlueprintCallable)
	bool GetCanBecomeInvisible() const;

private :

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 1;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 1;

	UPROPERTY(VisibleAnywhere, Category = "Tank", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Tank", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;




	//Bellow is only Dash related code
	UPROPERTY()
	bool bCanDash = true;

	bool bIsDashing = false;

	UPROPERTY(EditAnywhere,Category = "Dash")
	float DashCooldown = 2.f;

	UPROPERTY(EditAnywhere,Category = "Dash")
	float DashSpeed = 2000.f;

	FTimerHandle DashCooldownTimer;  // Timer handle
	FTimerHandle StopDashTimer;  // Timer handle
	
	void DashCooldownReset();  // Function to reset dash
	void StopDash();

	UPROPERTY(EditAnywhere,Category = "Dash")
	float DashTime = 0.3f;
	UPROPERTY(VisibleAnywhere, Category = "Dash", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector DashStart;
	UPROPERTY(VisibleAnywhere, Category = "Dash", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector DashEnd;
	UPROPERTY(VisibleAnywhere, Category = "Dash", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float DashProgress = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Dash", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int LastDirectionPressed = 1;

	UPROPERTY(EditAnywhere, Category = "Dash", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* DashEffect;

	//Code Related to Invisbility
	bool bIsInvisible = false;
	bool bCanBecomeInvisible = true;

	FTimerHandle InvisibleCooldownTimer;  // Timer handle
	FTimerHandle StopInvisibilityTimer;  // Timer handle
	
	void InvisibleCooldownReset();  
	void StopInvisibility();

	UPROPERTY(EditAnywhere,Category = "Invisible")
	float InvisibilityCooldown = 3.f;
	UPROPERTY(EditAnywhere,Category = "Invisible")
	float InvisibilityDuration = 3.f;

	bool bIsMoving;
};
