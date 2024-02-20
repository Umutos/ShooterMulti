// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../GameFramework/Resetable.h"
#include "Pickup.generated.h"


USTRUCT()
struct FSpawnKey
{
	GENERATED_BODY()

	int ClassKey = 0;
	int SpawnPointKey = 0;
};

UCLASS()
class SHOOTERMULTI_API APickup : public AActor, public IResetable
{
	GENERATED_BODY()

private:

	FTimerHandle CoolDownTimerHandle;

	float GlobalTime = 0.0f;

	friend class APickupDirector;
	FSpawnKey SpawnKey;
	APickupDirector* Director;

protected:

	APickup();

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int CoolDownDuration = 10;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int RotationSpeed = 42;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	USoundBase* PickupSound;

	FVector BasePos;

	virtual void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void NotifyActorBeginOverlap(AActor * OtherActor) override;

	void Reset() override;

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void Multicast_Destroy();
	void Multicast_Destroy_Implementation();
	bool Multicast_Destroy_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Destroy();
	void Server_Destroy_Implementation();
	bool Server_Destroy_Validate();
};