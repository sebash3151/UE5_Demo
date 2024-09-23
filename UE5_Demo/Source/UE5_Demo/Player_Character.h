// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "Player_Character.generated.h"

UCLASS()
class UE5_DEMO_API APlayer_Character : public ACharacter
{
	GENERATED_BODY()


private:
	//Visibility of the camera objects in the player blueprint
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//Visibility of the attributes for the camera rotation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraBaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float CameraBaseLookUpRate;

	//Editable of the anim montage for the attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackAnimMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Attack, meta = (AllowPrivateAccess = "true"))
	bool IsAttacking = false;
	//Editable of the sound property for the attack
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundCue* AttackSound;

	//Editable for the crouch action
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool IsCrouching = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MovementSpeed;

	//Editable of the sound property of the jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundCue* JumpSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundCue* HeavyJumpSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool DoubleJumped = false;

	//Editable of the anim montage and sound for the dash
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DashAnimMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool IsDashing = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound, meta = (AllowPrivateAccess = "true"))
	USoundCue* DashSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashMultipier;

public:
	// Sets default values for this character's properties
	APlayer_Character();

	//Method to know when the player touches ground
	virtual void Landed(const FHitResult& hit) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Methods to add movement to the player
	void VerticalMovement(float Value);
	void HorizontalMovement(float Value);

	//Methods to add rotation to the player's camera
	void CameraTurnAtRate(float Rate);
	void CameraLookAtRate(float Rate);

	//Method for the attack
	void Attack();
	virtual void PostInitializeComponents() override;
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//Methods for crouching
	void Crouch();
	void StopCrouching();

	//Method and variable for the jump
	void Jump();
	bool bCanMakeJump{ true };

	//Method for the dash
	void Dash();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetCrouch() const { return IsCrouching; }
	bool GetDoubleJump() const { return DoubleJumped; }
};
