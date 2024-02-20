// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicBox.h"
#include "BoxManager.generated.h"

UCLASS()
class SHOOTERMULTI_API ABoxManager : public AActor
{
	GENERATED_BODY()

protected:
	FTimerHandle TimerHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void Multicast_SpawnBox();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnBox();

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director)
	TSubclassOf<APhysicBox> BoxBP;

	// Sets default values for this actor's properties
	ABoxManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
