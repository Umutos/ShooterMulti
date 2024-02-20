#include "DeathMatchGM.h"
#include "Engine/World.h"
#include "ShooterMulti/Controllers/ShooterController.h"
#include "ShooterMulti/Characters/ShooterCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

void ADeathMatchGM::Respawn(APlayerController* PlayerController)
{
	RestartPlayerAtPlayerStart(PlayerController, ChoosePlayerStart(PlayerController));
}

void ADeathMatchGM::Quit()
{
	FGenericPlatformMisc::RequestExit(false);
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void ADeathMatchGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AShooterController* playerController = Cast<AShooterController>(NewPlayer);

	if (!playerController) return;

	//playerController->Client_SetPlayerInfo("Player" + GetNumPlayers(), 1 + GetNumPlayers() % 2);
}

void ADeathMatchGM::CheckPlayersAreReady()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckPlayersAreReady"));

	ADeathMatchGS* MyGameState = GetGameState<ADeathMatchGS>();
	if (MyGameState != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState ok"));

		if (MyGameState->ArePlayersReady())
		{
			UE_LOG(LogTemp, Warning, TEXT("ServerTravel to Game map"));
			bUseSeamlessTravel = true;
			GetWorld()->ServerTravel("Highrise");
		}
	}
}