#include "UndeadAIController.h"
#include "../ShooterMulti.h"
#include "../Characters/UndeadCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AUndeadAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BehaviorTree && BlackboardData)
	{
		RunBehaviorTree(BehaviorTree);

		UseBlackboard(BlackboardData, BlackboardComponent);

		SetIsStun(false);
	}
	else
		UE_LOG(GLogShooterMulti, Warning, TEXT("Please edit the BehaviorTree and BlackboardData values in Blueprint"));
}

bool AUndeadAIController::Punch()
{
	if (AUndeadCharacter* UndeadCharacter = Cast<AUndeadCharacter>(GetCharacter()))
	{
		return UndeadCharacter->Punch();
	}

	return false;
}

void AUndeadAIController::SetIsStun(bool IsStun)
{
	BlackboardComponent->SetValueAsBool(FName("IsStun"), IsStun);
}