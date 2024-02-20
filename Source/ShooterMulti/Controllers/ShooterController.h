#pragma once

#include "GameFramework/PlayerController.h"
#include "../GameFramework/DeathMatchGS.h"
#include "ShooterController.generated.h"

UCLASS()
class SHOOTERMULTI_API AShooterController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	class AShooterCharacter* ShooterCharacter = nullptr;

	virtual void BeginPlayingState() override;
	virtual void SetupInputComponent() override;

public:
	void DisableInput(APlayerController* PlayerController) override;

	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndJump();
	UFUNCTION(BlueprintCallable, Category = "Shooter|PlayerController")
	void EndReload();

	void StartReload();

	UFUNCTION(BlueprintCallable)
	void SetPlayerInfo(const FString& name, int team);

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(CLient, Reliable, WithValidation)
	void Client_MoveForward(float Value);

	UFUNCTION(CLient, Reliable, WithValidation)
	void Client_MoveRight(float Value);

	void LookUp(float Value);
	void Turn(float Value);

	UFUNCTION(CLient, Reliable, WithValidation)
	void Client_LookUp(float Value);

	UFUNCTION(CLient, Reliable, WithValidation)
	void Client_Turn(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartSprint();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_StartSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndSprint();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_EndSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartJump();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_StartJump();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndJump();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_EndJump();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartAim();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_StartAim();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndAim();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_EndAim();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartShoot();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_StartShoot();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndShoot();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_EndShoot();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_EndReload();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PushButton();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_PushButton();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Punch();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void Multicast_Punch();
};