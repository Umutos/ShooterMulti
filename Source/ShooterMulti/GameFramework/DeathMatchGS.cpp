#include "DeathMatchGS.h"
#include "ShooterPS.h"
#include "TimerManager.h"
#include "DeathMatchGM.h"
#include "../Characters/ShooterCharacter.h"
#include "../LD/Pickup.h"
#include "../Controllers/ShooterController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"

void ADeathMatchGS::BeginPlay()
{
	Super::BeginPlay();

	OnTeamWin.AddLambda([this](ETeam Team) 
		{ 
			ShowTeamWinHUD(Team); 
			RedTeamScore = 0;
			BlueTeamScore = 0;
		});

	OnGameRestart.AddLambda([this]() { Reset(); });

	if (HasAuthority())
		SetUp();
}

void ADeathMatchGS::AdvanceTimer()
{
	--CurrentTime;
	
	if (CurrentTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
		if (RedTeamScore < BlueTeamScore)
			UpdateEndHud(ETeam::Blue);
		else if (RedTeamScore > BlueTeamScore)
			UpdateEndHud(ETeam::Red);
		else
			UpdateEndHud(ETeam::None);
	}
}

void ADeathMatchGS::AddScore(ETeam Team)
{
	Server_AddScore(Team);
}

void ADeathMatchGS::Server_AddScore_Implementation(ETeam Team)
{
	if (Team == ETeam::Red)
		++RedTeamScore;
	else if (Team == ETeam::Blue)
		++BlueTeamScore;
}

void ADeathMatchGS::Rep_RedScore()
{
	if (RedTeamScore == MaxKill)
		UpdateEndHud(ETeam::Red);
}

void ADeathMatchGS::Rep_BlueScore()
{
	if (BlueTeamScore == MaxKill)
		UpdateEndHud(ETeam::Blue);
}

void ADeathMatchGS::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	OnPlayerNum.Broadcast(this);
}

void ADeathMatchGS::RemovePlayerState(APlayerState* PlayerState)
{
	OnPlayerNum.Broadcast(this);

	Super::RemovePlayerState(PlayerState);
}

bool ADeathMatchGS::CanAddAI()
{
	return MaxAIPerPlayer * PlayerArray.Num() > CurrentAICount;

	return false;
}

void ADeathMatchGS::AddAI()
{
	CurrentAICount++;
}

void ADeathMatchGS::RemoveAI()
{
	CurrentAICount--;
}

int ADeathMatchGS::GetNbplayer()
{
	return PlayerArray.Num();
}

void ADeathMatchGS::UpdateEndHud(ETeam Team)
{
	GetWorldTimerManager().ClearTimer(CountdownTimerHandle);
	OnTeamWin.Broadcast(Team);
}

void ADeathMatchGS::Reset()
{
	TArray<AActor*> Resetables;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UResetable::StaticClass(), Resetables);

	for (auto& res : Resetables)
		Cast<IResetable>(res)->Reset();

	Server_RestartGame();
}

void ADeathMatchGS::ResetAfterDelay()
{
	CurrentTime = GameTime;
	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);

	RedTeamScore = 0;
	BlueTeamScore = 0;
	CurrentAICount = 0;

	OnResetAfterDelay.Broadcast();
}

void ADeathMatchGS::EndGameTrigg()
{
	OnGameRestart.Broadcast();
}

void ADeathMatchGS::SetUp()
{
	if (GameMode == nullptr)
		GameMode = Cast<ADeathMatchGM>(AuthorityGameMode);

	MaxKill = GameMode->MaxKill;
	GameTime = GameMode->GameTime;
	MaxAIPerPlayer = GameMode->MaxAIPerPlayer;
	InvincibilityTimer = GameMode->InvincibilityTime;

	CurrentTime = GameTime;

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);
}


void ADeathMatchGS::SetGameTimer()
{
	CurrentTime = GameTime;

	GetWorldTimerManager().SetTimer(CountdownTimerHandle, this, &ADeathMatchGS::AdvanceTimer, 1.0f, true);
}

bool ADeathMatchGS::ArePlayersReady()
{
	if (GetLocalRole() != ROLE_Authority)
		return false;

	UE_LOG(LogTemp, Warning, TEXT("Found %d players"), PlayerArray.Num());

	for (const auto PlayerState : PlayerArray)
	{
		AShooterPS* MyPlayerState = Cast<AShooterPS>(PlayerState);
		if (MyPlayerState == nullptr || !MyPlayerState->bIsPlayerReady)
		{
			UE_LOG(LogTemp, Warning, TEXT("NOT all players are ready"));
			return false;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("All players are ready !!"));

	return true;
}

void ADeathMatchGS::Server_RestartGame_Implementation()
{
	GetWorld()->ServerTravel("Highrise");
}

bool ADeathMatchGS::Server_RestartGame_Validate()
{
	return true;
}