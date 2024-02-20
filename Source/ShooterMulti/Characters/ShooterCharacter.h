#pragma once

#include "HealthCharacter.h"
#include "../Weapons/WeaponComponent.h"
#include "PlayerCameraComponent.h"
#include "ShooterCharacter.generated.h"

UENUM(BlueprintType)
enum class EShooterCharacterState : uint8
{
	IdleRun,
	Aim,
	Sprint,
	Reload,
	Jump,
	Falling,
	Punch,
	Dead,
	PushButton
};

UCLASS()
class SHOOTERMULTI_API AShooterCharacter : public AHealthCharacter
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Character|Shooter")
	UWeaponComponent* Weapon;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character|Shooter")
	EShooterCharacterState State;

	UPROPERTY(Replicated)
	EShooterCharacterState PrevState;

	UPROPERTY(ReplicatedUsing = WeaponReload)
	bool isReloading = false;

	UFUNCTION()
	void WeaponReload();

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(AShooterCharacter, State);
		DOREPLIFETIME(AShooterCharacter, PrevState);
		DOREPLIFETIME(AShooterCharacter, bIsShooting);
		DOREPLIFETIME(AShooterCharacter, isReloading);
	}

	UPROPERTY(BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(BlueprintReadOnly)
	float AimYaw;

	void UpdateAimOffsets(float Pitch, float Yaw);

	UFUNCTION(NetMulticast, Reliable)
	void PlayPushButtonAnim();

	UFUNCTION(NetMulticast, Reliable)
	void PlayPunchAnim();

	void Falling() override;

	void BeginPlay() override;

	void Invincibility(float Duration);

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void InvincibilityFX(float Duration);
	void InvincibilityFX_Implementation(float Duration) {};

public:

	UPROPERTY(Replicated)
	bool bIsShooting = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimArmLength = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float StandardArmLength = 300.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimFOV = 75.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float StandardFOV = 90.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float SprintSpeed = 1000.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float AimWalkSpeed = 180.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter")
	float ReloadWalkSpeed = 200.f;

	UPROPERTY(BlueprintReadOnly)
	float RunSpeed = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character|Shooter", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MinSprintMagnitude = .3f;

	AShooterCharacter();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	EShooterCharacterState GetState() const;
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void SetState(EShooterCharacterState InState);

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UWeaponComponent* GetWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	UCameraComponent* GetCameraComponent();

	void InitPlayer();

	void Tick(float DeltaTime) override;

	UFUNCTION()
	void StartSprint();
	UFUNCTION()
	void EndSprint();

	UFUNCTION()
	void StartJump();
	UFUNCTION()
	void EndJump();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartAim();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndAim();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartShoot();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndShoot();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void StartReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void EndReload();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void AbortReload();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void PushButton();
	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void InflictPushButton();

	UFUNCTION(BlueprintCallable, Category = "Character|Shooter")
	void Punch();

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Shooter")
	void RefreshTeamHUD(ETeam InTeam);
	void RefreshTeamHUD_Implementation(ETeam InTeam) {};

	void StartDisapear() override;
	void FinishDisapear() override;

protected:
	float InvincibilityTimer = 0;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MultiCast_Invincibily(int timer);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_AskForRespawn(APlayerController* playerController);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InitTeamColor(ETeam InTeam);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_InitTeamColor(ETeam InTeam);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_InflictPushButton(AActor* button, ETeam InTeam);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ResetReloading();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartReload();
};