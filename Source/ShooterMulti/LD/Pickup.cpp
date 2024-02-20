#include "Pickup.h"
#include "PickupDirector.h"
#include "../Characters/ShooterCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APickup::BeginPlay()
{
	Super::BeginPlay();

	BasePos = GetActorLocation();
}

void APickup::Tick(float DeltaTime)
{
	GlobalTime += DeltaTime;
	float Offset = 10 * sin(GlobalTime);

	SetActorLocation(BasePos + FVector::UpVector * Offset);

	FRotator NewRot = GetActorRotation();
	NewRot.Yaw += 42 * sin(DeltaTime);
	SetActorRotation(NewRot);
}

void APickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);

	if (!IsValid(Player))
		return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, GetActorLocation());

	if (IsValid(Director))
		Director->FreePickup(SpawnKey);

	Server_Destroy();
}

void APickup::Reset()
{
	Destroy();
}

void APickup::Multicast_Destroy_Implementation()
{
	Destroy();
}

bool APickup::Multicast_Destroy_Validate()
{
	return true;
}

void APickup::Server_Destroy_Implementation()
{
	if (HasAuthority())
		Multicast_Destroy();
}

bool APickup::Server_Destroy_Validate()
{
	return true;
}