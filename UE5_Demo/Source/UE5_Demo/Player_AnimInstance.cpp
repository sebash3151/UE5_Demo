// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_AnimInstance.h"
#include "Player_Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayer_AnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayer_Character>(TryGetPawnOwner());
}

void UPlayer_AnimInstance::NativeUpdateAnimation(float Deltatime)
{
	Super::NativeUpdateAnimation(Deltatime);

	if (PlayerCharacter == nullptr) {
		PlayerCharacter = Cast<APlayer_Character>(TryGetPawnOwner());
	}
	else {
		//Get the current speed of the player
		FVector Velocity = PlayerCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		//Bool to know if the player is falling = is in the air
		bIsInAir = PlayerCharacter->GetCharacterMovement()->IsFalling();

		//Bool to know if the player is currently accelerating

		bIsAccelerating = (PlayerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f);

		FRotator AimRotation = PlayerCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(PlayerCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, MovementRotation).Yaw;

		if (PlayerCharacter->GetVelocity().Size() > 0) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		bIsCroughing = PlayerCharacter->GetCrouch();
		bDoubleJumped = PlayerCharacter->GetDoubleJump();
	}
}
