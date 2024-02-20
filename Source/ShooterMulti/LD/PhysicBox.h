// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GameFramework/Resetable.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Classes/Components/BoxComponent.h"
#include "PhysicBox.generated.h"

UCLASS()
class SHOOTERMULTI_API APhysicBox : public AActor, public IResetable
{
	GENERATED_BODY()

protected:
	UBoxComponent* collision;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Reset() override;

public:	
	// Sets default values for this actor's properties
	APhysicBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};
