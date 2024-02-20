#include "ShooterController.h"
#include "../Characters/ShooterCharacter.h"
#include "ShooterMulti/GameFramework/PlayerGI.h"
#include "GameFramework/GameModeBase.h"
#include "GameMapsSettings.h"
#include "GameFramework/CharacterMovementComponent.h"

void AShooterController::BeginPlayingState()
{
	Super::BeginPlayingState();

	ShooterCharacter = Cast<AShooterCharacter>(GetPawn());
	ShooterCharacter->InitPlayer();
}

void AShooterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AShooterController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AShooterController::MoveRight);
	InputComponent->BindAxis("LookUp", this, &AShooterController::LookUp);
	InputComponent->BindAxis("Turn", this, &AShooterController::Turn);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &AShooterController::Server_StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AShooterController::Server_EndSprint);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AShooterController::Server_StartAim);
	InputComponent->BindAction("Aim", IE_Released, this, &AShooterController::Server_EndAim);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AShooterController::Server_StartReload);
	InputComponent->BindAction("Punch", IE_Pressed, this, &AShooterController::Server_Punch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AShooterController::Server_StartJump);
	InputComponent->BindAction("Shoot", IE_Pressed, this, &AShooterController::Server_StartShoot);
	InputComponent->BindAction("Shoot", IE_Released, this, &AShooterController::Server_EndShoot);
	InputComponent->BindAction("PushButton", IE_Pressed, this, &AShooterController::Server_PushButton);
}

void AShooterController::EndJump()
{
	Server_EndJump();
}

void AShooterController::EndReload()
{
	Server_EndReload();
}

void AShooterController::DisableInput(APlayerController* PlayerController)
{
	Super::DisableInput(PlayerController);
	
	Server_EndSprint();
	Server_EndAim();
	Server_EndShoot();
}

void AShooterController::SetPlayerInfo(const FString& name, int team)
{
	Cast<UPlayerGI>(GetGameInstance())->SetUserInfo(team, name);
}

void AShooterController::MoveForward(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
		Client_MoveForward(Value);
}

void AShooterController::MoveRight(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::Sprint
		&& ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
		Client_MoveRight(Value);
}

void AShooterController::Client_MoveForward_Implementation(float Value)
{
	if (ShooterCharacter->GetState() == EShooterCharacterState::Sprint && Value <= 0.0f)
		Server_EndSprint();

	FRotator Rotation = GetControlRotation();
	Rotation.Pitch = 0.f;
	Rotation.Roll = 0.f;

	ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().Vector());
}

bool AShooterController::Client_MoveForward_Validate(float Value)
{
	return true;
}

void AShooterController::Client_MoveRight_Implementation(float Value)
{
	FRotator Rotation = GetControlRotation();
	Rotation.Pitch = 0.f;
	Rotation.Roll = 0.f;

	ShooterCharacter->AddMovementInput(Value * Rotation.GetNormalized().RotateVector(FVector::RightVector));
}

bool AShooterController::Client_MoveRight_Validate(float Value)
{
	return true;
}

void AShooterController::LookUp(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		Client_LookUp(Value);
	}
}

void AShooterController::Turn(float Value)
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead() && ShooterCharacter->GetState() != EShooterCharacterState::PushButton)
	{
		Client_Turn(Value);
	}
}

void AShooterController::Client_LookUp_Implementation(float Value)
{
	AddPitchInput(Value);
}

bool AShooterController::Client_LookUp_Validate(float Value)
{
	return true;
}

void AShooterController::Client_Turn_Implementation(float Value)
{
	AddYawInput(Value);
}

bool AShooterController::Client_Turn_Validate(float Value)
{
	return true;
}

void AShooterController::Server_StartSprint_Implementation()
{
	if (HasAuthority())
		Multicast_StartSprint();
}

bool AShooterController::Server_StartSprint_Validate()
{
	return true;
}

void AShooterController::Multicast_StartSprint_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartSprint();
}

bool AShooterController::Multicast_StartSprint_Validate()
{
	return true;
}

void AShooterController::Server_EndSprint_Implementation()
{
	if (HasAuthority())
		Multicast_EndSprint();
}

bool AShooterController::Server_EndSprint_Validate()
{
	return true;
}

void AShooterController::Multicast_EndSprint_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndSprint();
}

bool AShooterController::Multicast_EndSprint_Validate()
{
	return true;
}

void AShooterController::Server_StartJump_Implementation()
{
	if (HasAuthority())
		Multicast_StartJump();
}

bool AShooterController::Server_StartJump_Validate()
{
	return true;
}

void AShooterController::Multicast_StartJump_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartJump();
}

bool AShooterController::Multicast_StartJump_Validate()
{
	return true;
}

void AShooterController::Server_EndJump_Implementation()
{
	if (HasAuthority())
		Multicast_EndJump();
}

bool AShooterController::Server_EndJump_Validate()
{
	return true;
}

void AShooterController::Multicast_EndJump_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndJump();
}

bool AShooterController::Multicast_EndJump_Validate()
{
	return true;
}

void AShooterController::Server_StartAim_Implementation()
{
	if (HasAuthority())
		Multicast_StartAim();
}

bool AShooterController::Server_StartAim_Validate()
{
	return true;
}

void AShooterController::Multicast_StartAim_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartAim();
}

bool AShooterController::Multicast_StartAim_Validate()
{
	return true;
}

void AShooterController::Server_EndAim_Implementation()
{
	if (HasAuthority())
		Multicast_EndAim();
}

bool AShooterController::Server_EndAim_Validate()
{
	return true;
}

void AShooterController::Multicast_EndAim_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndAim();
}

bool AShooterController::Multicast_EndAim_Validate()
{
	return true;
}

void AShooterController::Server_StartShoot_Implementation()
{
	if (HasAuthority())
		Multicast_StartShoot();
}

bool AShooterController::Server_StartShoot_Validate()
{
	return true;
}

void AShooterController::Multicast_StartShoot_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartShoot();
}

bool AShooterController::Multicast_StartShoot_Validate()
{
	return true;
}

void AShooterController::Server_EndShoot_Implementation()
{
	if (HasAuthority())
		Multicast_EndShoot();
}

bool AShooterController::Server_EndShoot_Validate()
{
	return true;
}

void AShooterController::Multicast_EndShoot_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndShoot();
}

bool AShooterController::Multicast_EndShoot_Validate()
{
	return true;
}

void AShooterController::Server_StartReload_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->StartReload();
}

bool AShooterController::Server_StartReload_Validate()
{
	return true;
}

void AShooterController::Server_EndReload_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->EndReload();
}

bool AShooterController::Server_EndReload_Validate()
{
	return true;
}

void AShooterController::Server_PushButton_Implementation()
{
	if (HasAuthority())
		Multicast_PushButton();
}

bool AShooterController::Server_PushButton_Validate()
{
	return true;
}

void AShooterController::Multicast_PushButton_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->PushButton();
}

bool AShooterController::Multicast_PushButton_Validate()
{
	return true;
}

void AShooterController::Server_Punch_Implementation()
{
	if (HasAuthority())
		Multicast_Punch();
}

bool AShooterController::Server_Punch_Validate()
{
	return true;
}

void AShooterController::Multicast_Punch_Implementation()
{
	if (IsValid(ShooterCharacter) && !ShooterCharacter->IsDead())
		ShooterCharacter->Punch();
}

bool AShooterController::Multicast_Punch_Validate()
{
	return true;
}