// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxManager.h"

// Sets default values
ABoxManager::ABoxManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABoxManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		Server_SpawnBox();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoxManager::Server_SpawnBox, 30.f, true);
	}
}

// Called every frame
void ABoxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoxManager::Multicast_SpawnBox_Implementation()
{
	GetWorld()->SpawnActor<APhysicBox>(BoxBP, GetActorLocation(), GetActorRotation());
}

bool ABoxManager::Multicast_SpawnBox_Validate()
{
	return true;
}

void ABoxManager::Server_SpawnBox_Implementation()
{
	if (HasAuthority())
		Multicast_SpawnBox();
}

bool ABoxManager::Server_SpawnBox_Validate()
{
	return true;
}
