#pragma once

#include "Pickup.h"
#include "../GameFramework/Resetable.h"
#include "GameFramework/Actor.h"
#include "PickupDirector.generated.h"

UCLASS()
class SHOOTERMULTI_API APickupDirector : public AActor, public IResetable
{
	GENERATED_BODY()
	
private:

	TArray<bool> IsSpawnFullArray;
	int CurrentPickupIndex = 0;

	FTimerHandle TimerHandle;

	bool bIsFull = false;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(NetMultiCast, Reliable, WithValidation)
	void Multicast_SpawnPickup(int pickupIndex, int spawnPointIndex);
	void Multicast_SpawnPickup_Implementation(int pickupIndex, int spawnPointIndex);
	bool Multicast_SpawnPickup_Validate(int pickupIndex, int spawnPointIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SpawnPickup(int pickupIndex, int spawnPointIndex);
	void Server_SpawnPickup_Implementation(int pickupIndex, int spawnPointIndex);
	bool Server_SpawnPickup_Validate(int pickupIndex, int spawnPointIndex);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_FreePickup(FSpawnKey Key);
	void Server_FreePickup_Implementation(FSpawnKey Key);
	bool Server_FreePickup_Validate(FSpawnKey Key);

public:

	APickupDirector();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Director, meta = (ClampMin = 0.1f))
	float SecondPerSpawn = 15.0f;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditInstanceOnly, BlueprintInternalUseOnly, Category = Director)
	TArray<TSubclassOf<APickup>> PickupBPs;
	
	void SpawnTick();
	void FreePickup(FSpawnKey Key);

	void SpawnPickup(int pickupIndex, int spawnPointIndex);

	void SetFull(bool isFull);

	//void UpdateFrequencies(class ADeathMatchGS* GameState);

	virtual void Reset() override;
};