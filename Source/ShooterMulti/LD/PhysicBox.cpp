// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicBox.h"

// Sets default values
APhysicBox::APhysicBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;
	bReplicates = true;
	SetReplicateMovement(true);

	USceneComponent* root = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(root);

	collision = CreateDefaultSubobject<UBoxComponent>("box");
	collision->SetMassOverrideInKg(NAME_None, 50.f);
	collision->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	collision->SetSimulatePhysics(true);
	collision->SetEnableGravity(true);
	collision->SetIsReplicated(true);
	collision->SetupAttachment(root);
}

// Called when the game starts or when spawned
void APhysicBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhysicBox::Reset()
{
	Destroy();
}