#include "PickupDirector.h"
#include "Engine/World.h"
#include "../ShooterMulti.h"
#include "../GameFramework/DeathMatchGS.h"

APickupDirector::APickupDirector()
{
	bAlwaysRelevant = true;
	bReplicates = true;
}

void APickupDirector::BeginPlay()
{
	Super::BeginPlay();

	IsSpawnFullArray.SetNum(SpawnPoints.Num());

	if (HasAuthority())
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &APickupDirector::SpawnTick, SecondPerSpawn, true);

	//ADeathMatchGS* GameState = Cast<ADeathMatchGS>(GetWorld()->GetGameState());
	//GameState->OnPlayerNum.AddLambda([this](ADeathMatchGS* GS) { UpdateFrequencies(GS); }); // ??
}

void APickupDirector::SpawnTick()
{
	if (bIsFull)
		return;
	
	int MaxPoints = SpawnPoints.Num() - 1;
	int RandomPoint = FMath::RandRange(0, MaxPoints);
	int PrevPoint = RandomPoint;

	while (IsSpawnFullArray[RandomPoint])
	{
		RandomPoint = (RandomPoint + 1) % MaxPoints;
		if (RandomPoint == PrevPoint)
		{
			bIsFull = true;
			return;
		}
	}

	IsSpawnFullArray[RandomPoint] = true;
	Server_SpawnPickup(CurrentPickupIndex, RandomPoint);
	CurrentPickupIndex = (CurrentPickupIndex + 1) % PickupBPs.Num();
}

void APickupDirector::SpawnPickup(int pickupIndex, int spawnPointIndex)
{
	auto pickupBP = PickupBPs[pickupIndex];
	auto pickupLocation = SpawnPoints[spawnPointIndex]->GetActorLocation();
	auto pickupRotation = SpawnPoints[spawnPointIndex]->GetActorRotation();

	auto Pickup = GetWorld()->SpawnActor<APickup>(pickupBP, pickupLocation, pickupRotation);

	if (Pickup)
	{
		Pickup->SpawnKey.ClassKey = pickupIndex;
		Pickup->SpawnKey.SpawnPointKey = spawnPointIndex;
		Pickup->Director = this;
	}
}

void APickupDirector::FreePickup(FSpawnKey Key)
{
	Server_FreePickup(Key);
}

void APickupDirector::SetFull(bool isFull)
{
	bIsFull = isFull;
}

void APickupDirector::Reset()
{
	bIsFull = false;

	for (int i = 0; i < IsSpawnFullArray.Num(); i++)
		IsSpawnFullArray[i] = false;
}

void APickupDirector::Multicast_SpawnPickup_Implementation(int pickupIndex, int spawnPointIndex)
{
	SpawnPickup(pickupIndex, spawnPointIndex);
}

bool APickupDirector::Multicast_SpawnPickup_Validate(int pickupIndex, int spawnPointIndex)
{
	return true;
}

void APickupDirector::Server_SpawnPickup_Implementation(int pickupIndex, int spawnPointIndex)
{
	if (HasAuthority())
		Multicast_SpawnPickup(pickupIndex, spawnPointIndex);
}

bool APickupDirector::Server_SpawnPickup_Validate(int pickupIndex, int spawnPointIndex)
{
	return true;
}

void APickupDirector::Server_FreePickup_Implementation(FSpawnKey Key)
{
	IsSpawnFullArray[Key.SpawnPointKey] = false;
}

bool APickupDirector::Server_FreePickup_Validate(FSpawnKey Key)
{
	return true;
}