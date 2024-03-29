#include "ShooterCharacter.h"
#include "../Animations/ShooterCharacterAnim.h"
#include "../GameFramework/PlayerGI.h"
#include "../LD/EnemySpawnerButton.h"
#include "../GameFramework/DeathMatchGS.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/UObjectGlobals.h"
#include "Animation/AnimBlueprint.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "ShooterMulti/Controllers/ShooterController.h"
#include "Engine/World.h"

AShooterCharacter::AShooterCharacter()
{
	DisapearingDelay = 1.5f;

	// Create Weapon
	Weapon = CreateDefaultSubobject<UWeaponComponent>("Rifle");

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshContainer(TEXT("SkeletalMesh'/Game/Weapons/Rifle.Rifle'"));
	if (MeshContainer.Succeeded())
		Weapon->SetSkeletalMesh(MeshContainer.Object);

	Weapon->SetRelativeLocation(FVector(1.0f, 4.0f, -2.0f));
	Weapon->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	Weapon->SetupAttachment(GetMesh(), "hand_r");

	// Create Sprint Arm and Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->ProbeSize = 12.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("PlayerCamera");
	Camera->SetupAttachment(SpringArm);
	Camera->SetRelativeLocation(FVector(30.f, 0.f, 100.f));

	PrimaryActorTick.bCanEverTick = true;

	AimPitch = 0;
	AimYaw = 0;
	State = EShooterCharacterState::IdleRun;
	PrevState = EShooterCharacterState::IdleRun;
}

EShooterCharacterState AShooterCharacter::GetState() const
{
	return State;
}

void AShooterCharacter::SetState(EShooterCharacterState InState)
{
	PrevState = State;
	State = InState;
}

UWeaponComponent* AShooterCharacter::GetWeaponComponent()
{
	return Weapon;
}

UCameraComponent* AShooterCharacter::GetCameraComponent()
{
	return Camera;
}

void AShooterCharacter::UpdateAimOffsets(float Pitch, float Yaw)
{
	AimPitch = Pitch;
	AimYaw = Yaw;
}

void AShooterCharacter::InitPlayer()
{
	const FPlayerInfo& PlayerInfo = static_cast<UPlayerGI*>(GetGameInstance())->GetUserInfo();

	GetPlayerState();

	Server_InitTeamColor(static_cast<ETeam>(PlayerInfo.TeamNum));
}

void AShooterCharacter::Invincibility(float Duration)
{
	Health = 100000;
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, [this]() { Health = MaxHealth; }, Duration, false);

	InvincibilityFX(Duration);
}

void AShooterCharacter::BeginPlay()
{
	OnTeamSwitch.AddLambda([this]() { RefreshTeamHUD(Team); });
	
	Super::BeginPlay();
	
	RunSpeed = GetCharacterMovement()->MaxWalkSpeed;

	if (GetLocalRole() == ROLE_Authority)
		MultiCast_Invincibily(Cast<ADeathMatchGS>(GetWorld()->GetGameState())->GetInvincibilityTime());
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead())
		return;

	if (bIsShooting && !Weapon->Shot())
		Server_StartReload();

	// Anim aim offsets
	if (HasAuthority())
	{
		FRotator LookRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation());
		AimPitch = UKismetMathLibrary::ClampAngle(LookRotation.Pitch, -90.f, 90.f);
		AimYaw = UKismetMathLibrary::ClampAngle(LookRotation.Yaw, -90.f, 90.f);

		UpdateAimOffsets(AimPitch, AimYaw);
	}
}

void AShooterCharacter::StartSprint()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Reload)
		AbortReload();
	else if (State == EShooterCharacterState::Aim)
		EndAim();

	if (State != EShooterCharacterState::IdleRun && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::Sprint;
	else
		SetState(EShooterCharacterState::Sprint);

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AShooterCharacter::EndSprint()
{
	if (State != EShooterCharacterState::Sprint && State != EShooterCharacterState::Jump)
		return;

	if (State == EShooterCharacterState::Jump)
		PrevState = EShooterCharacterState::IdleRun;
	else
		SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::StartJump()
{
	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (CanJump() && (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Sprint))
	{
		SetState(EShooterCharacterState::Jump);
		Jump();
	}
}

void AShooterCharacter::EndJump()
{
	if (State != EShooterCharacterState::Jump && State != EShooterCharacterState::Falling)
		return;

	SetState(EShooterCharacterState::IdleRun);
	StopJumping();
}

void AShooterCharacter::StartAim()
{
	if (State != EShooterCharacterState::IdleRun)
		return;
	
	SetState(EShooterCharacterState::Aim);

	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;

	SpringArm->TargetArmLength = AimArmLength;
	Camera->FieldOfView = AimFOV;
}

void AShooterCharacter::EndAim()
{
	if (State != EShooterCharacterState::Aim)
		return;

	SetState(PrevState);
	
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	
	SpringArm->TargetArmLength = StandardArmLength;
	Camera->FieldOfView = StandardFOV;
}

void AShooterCharacter::StartShoot()
{
	if (State == EShooterCharacterState::IdleRun || State == EShooterCharacterState::Aim)
		bIsShooting = true;
}

void AShooterCharacter::EndShoot()
{
	bIsShooting = false;
}

void AShooterCharacter::StartReload()
{
	if (Weapon && Weapon->AmmoCount > 0 && Weapon->WeaponMagazineSize > Weapon->LoadedAmmo)
	{
		if (State == EShooterCharacterState::Aim)
			EndAim();
		else if (bIsShooting)
			bIsShooting = false;

		if (State != EShooterCharacterState::IdleRun)
			return;

		SetState(EShooterCharacterState::Reload);
		
		GetCharacterMovement()->MaxWalkSpeed = ReloadWalkSpeed;
	}
}

void AShooterCharacter::EndReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	Weapon->Reload();

	isReloading = true;
}

void AShooterCharacter::WeaponReload()
{
	if (isReloading)
	{
		Weapon->Reload();
		Server_ResetReloading();
	}
}

void AShooterCharacter::Server_ResetReloading_Implementation()
{
	isReloading = false;
}

bool AShooterCharacter::Server_ResetReloading_Validate() { return true; }

void AShooterCharacter::Server_StartReload_Implementation()
{
	if (HasAuthority())
		StartReload();
}

bool AShooterCharacter::Server_StartReload_Validate() { return true; }

void AShooterCharacter::AbortReload()
{
	if (State != EShooterCharacterState::Reload)
		return;

	SetState(EShooterCharacterState::IdleRun);

	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AShooterCharacter::Falling()
{
	Super::Falling();

	if (State == EShooterCharacterState::Jump)
		return;

	if (bIsShooting)
		EndShoot();

	if (State == EShooterCharacterState::Aim)
		EndAim();
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	SetState(EShooterCharacterState::Falling);
}

void AShooterCharacter::PushButton()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::PushButton);
	PlayPushButtonAnim();
}

void AShooterCharacter::InflictPushButton()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, TSubclassOf<AEnemySpawnerButton>());

	if (OverlappingActors.Num() > 0)
	{
		Server_InflictPushButton(OverlappingActors[0], Team);
	}
}

void AShooterCharacter::PlayPushButtonAnim_Implementation()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPushButtonMontage();
}

void AShooterCharacter::Punch()
{
	if (bIsShooting)
		bIsShooting = false;
	else if (State == EShooterCharacterState::Reload)
		AbortReload();

	if (State != EShooterCharacterState::IdleRun)
		return;

	SetState(EShooterCharacterState::Punch);
	PlayPunchAnim();
}

void AShooterCharacter::PlayPunchAnim_Implementation()
{
	Cast<UShooterCharacterAnim>(GetMesh()->GetAnimInstance())->PlayPunchMontage();
}

void AShooterCharacter::StartDisapear()
{
	Super::StartDisapear();
	
	FTimerHandle Handle1;
	GetWorld()->GetTimerManager().SetTimer(Handle1, [this]() { Weapon->SetVisibility(false, true); }, 3.5f, false);

	if (Controller)
	{
		APlayerController* PlayerControler = Cast<APlayerController>(Controller);
		PlayerControler->DisableInput(PlayerControler);
		
		FTimerHandle Handle2;
		GetWorld()->GetTimerManager().SetTimer(Handle2, [PlayerControler]() { PlayerControler->EnableInput(PlayerControler); }, 5.0f, false);
	}
}

void AShooterCharacter::FinishDisapear()
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);

	Super::FinishDisapear();
	
	if (HasAuthority())
		Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->Respawn(PlayerController);
	else
		Server_AskForRespawn(PlayerController);
}

void AShooterCharacter::MultiCast_Invincibily_Implementation(int timer)
{
	Invincibility(timer);
}

bool AShooterCharacter::MultiCast_Invincibily_Validate(int timer)
{
	return true;
}

void AShooterCharacter::Server_AskForRespawn_Implementation(APlayerController* playerController)
{
	if (HasAuthority())
		Cast<ADeathMatchGM>(GetWorld()->GetAuthGameMode())->Respawn(playerController);
}

bool AShooterCharacter::Server_AskForRespawn_Validate(APlayerController* playerController)
{
	return true;
}

void AShooterCharacter::Server_InitTeamColor_Implementation(ETeam InTeam)
{
	if (HasAuthority())
		Multicast_InitTeamColor(InTeam);
}

bool AShooterCharacter::Server_InitTeamColor_Validate(ETeam InTeam)
{
	return true;
}

void AShooterCharacter::Multicast_InitTeamColor_Implementation(ETeam InTeam)
{
	SetTeam(InTeam);
	OnTeamSwitch.Broadcast();
}

bool AShooterCharacter::Multicast_InitTeamColor_Validate(ETeam InTeam)
{
	return true;
}

void AShooterCharacter::Server_InflictPushButton_Implementation(AActor* button, ETeam InTeam)
{
	AEnemySpawnerButton* Button = Cast<AEnemySpawnerButton>(button);

	if (Button)
		Button->Activate(InTeam);
}

bool AShooterCharacter::Server_InflictPushButton_Validate(AActor* button, ETeam InTeam)
{
	return true;
}