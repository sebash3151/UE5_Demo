// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayer_Character::APlayer_Character():
	CameraBaseTurnRate(45.f), CameraBaseLookUpRate(45.f), MovementSpeed(1.f), DashMultipier(2.f) //Initial Values
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Creation and attatchment of the camera objects to the player blueprint
	SpringCamera = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringCamera"));
	SpringCamera->SetupAttachment(RootComponent);
	SpringCamera->TargetArmLength = 300;
	SpringCamera->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringCamera, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

//Methos used to know when the player touches ground
void APlayer_Character::Landed(const FHitResult& hit)
{
    Super::Landed(hit);
	bCanMakeJump = true;
	DoubleJumped = false;
}

// Called when the game starts or when spawned
void APlayer_Character::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT(""));
	
}

//Method to add the character movement in the vertical Axis
void APlayer_Character::VerticalMovement(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, (Value * MovementSpeed));
	}
}

//Method to add the character movement in the horizontal Axis
void APlayer_Character::HorizontalMovement(float Value)
{
	if (Controller != nullptr && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, (Value * MovementSpeed));
	}
}

//Methods that allow the rotation of the camera in both axis
void APlayer_Character::CameraTurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * CameraBaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void APlayer_Character::CameraLookAtRate(float Rate)
{
	AddControllerPitchInput(Rate * CameraBaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayer_Character::Attack()
{
	if (IsAttacking) return;
	//Plays the attack sounds
	if (AttackSound) {
		UGameplayStatics::PlaySound2D(this, AttackSound);
	}
	//Gets the animinstance and plays the attack animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackAnimMontage) {
		AnimInstance->Montage_Play(AttackAnimMontage);
		AnimInstance->Montage_JumpToSection(FName("StartAttack"));
		IsAttacking = true;
	}
}

//Gets when the montage of the animation finalize
void APlayer_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->OnMontageEnded.AddDynamic(this, &APlayer_Character::OnAttackMontageEnded);
	
}

//Called when the montage of the attack ends to let a new attack be performed
void APlayer_Character::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (IsAttacking) {
		IsAttacking = false;
		IsDashing = false;
		if (IsDashing) {
			GetCharacterMovement()->GroundFriction = 8.0f;
		}
	}
}

//Method that modifies the speed of the character while is crouched and activates the bool
void APlayer_Character::Crouch()
{
	IsCrouching = true;
	MovementSpeed = 0.5f;
}

//Method that returns the speed of the character to its original speed and deactivates the bool
void APlayer_Character::StopCrouching()
{
	IsCrouching = false;
	MovementSpeed = 1.f;
}

//Method to make the character jump
void APlayer_Character::Jump()
{
	if (bCanMakeJump) 
	{
		//Evaluate if the character is currently in the air to make a doble jump
		const auto bIsInAir{ GetCharacterMovement()->IsFalling() };
		if (bIsInAir) 
		{
			DoubleJumped = true;
			const FVector launchVelocity{ 0.0f, 0.0f, GetCharacterMovement()->JumpZVelocity };
			const auto bCanOverrideVelocityInXAndY{ false };
			const auto bCanOverrideVelocityInZ{ true };

			LaunchCharacter(launchVelocity, bCanOverrideVelocityInXAndY, bCanOverrideVelocityInZ);
			if (HeavyJumpSound) {
				UGameplayStatics::PlaySound2D(this, HeavyJumpSound);
			}
			bCanMakeJump = false;
		}

		ACharacter::Jump();
		if (JumpSound && bCanMakeJump) {
			UGameplayStatics::PlaySound2D(this, JumpSound);
		}
	}
}

void APlayer_Character::Dash()
{
	if (IsDashing || GetLastMovementInputVector().IsZero() ) return;
	if (DashSound) {
		UGameplayStatics::PlaySound2D(this, DashSound);
	}
	//Gets the animinstance and plays the dash animation
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DashAnimMontage) {
		AnimInstance->Montage_Play(DashAnimMontage);
		AnimInstance->Montage_JumpToSection(FName("DashStart"));
		IsDashing = true;
		IsAttacking = true;
	}
	//Get the necesary parameters to lauch the character to the front
	FVector launchVelocity = GetLastMovementInputVector() * DashMultipier;
	const bool bCanOverrideVelocityInXAndY{ true };
	const bool bCanOverrideVelocityInZ{ false };

	//Disables the ground friction to allow the character to dash in the ground without problem
	GetCharacterMovement()->GroundFriction = 0.0f;

	//Lauch the character
	LaunchCharacter(launchVelocity, bCanOverrideVelocityInXAndY, bCanOverrideVelocityInZ);

	/*
	float actualspeed = GetCharacterMovement()->Velocity.Size();
	float actualJump = GetCharacterMovement()->JumpZVelocity;
	if (actualJump <= 1) actualJump = 1;
	if (actualspeed <= 1) actualspeed = 1;
	const FVector launchVelocity{ actualspeed * DashMultipier * -1 , 0.0f , actualJump * -5.f };
	const auto bCanOverrideVelocityInXAndY{ true };
	const auto bCanOverrideVelocityInZ{ true };

	LaunchCharacter(launchVelocity, bCanOverrideVelocityInXAndY, bCanOverrideVelocityInZ);
	*/
}

// Called every frame
void APlayer_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayer_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind the Inputs from the Axis mapping to the player movement methods
	PlayerInputComponent->BindAxis("VerticalMovement", this, &APlayer_Character::VerticalMovement);
	PlayerInputComponent->BindAxis("HorizontalMovement", this, &APlayer_Character::HorizontalMovement);

	//Bind the inputs frome the axis mapping to the camera rotation methods
	PlayerInputComponent->BindAxis("CameraTurnRate", this, &APlayer_Character::CameraTurnAtRate);
	PlayerInputComponent->BindAxis("CameraLookUpRate", this, &APlayer_Character::CameraLookAtRate);
	//Bind the mouse inputs to rotate the camera
	PlayerInputComponent->BindAxis("MouseTurn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("MouseLookUp", this, &APawn::AddControllerPitchInput);

	//Bind the jump inputs to the character action
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayer_Character::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//Bind the attack input to the character action
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayer_Character::Attack);

	//Bind the crouch input to the character action
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayer_Character::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &APlayer_Character::StopCrouching);

	//Bind the dash input to the character action
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayer_Character::Dash);
}
