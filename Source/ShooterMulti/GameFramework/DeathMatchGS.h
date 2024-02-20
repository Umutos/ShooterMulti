#pragma once

#include "DeathMatchGM.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"
#include "DeathMatchGS.generated.h"

class AHealthCharacter;

UCLASS()
class SHOOTERMULTI_API ADeathMatchGS : public AGameStateBase
{
	GENERATED_BODY()

protected:

	FTimerHandle CountdownTimerHandle;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Shooter|GameState")
	ADeathMatchGM* GameMode = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState")
	int32 CurrentTime;
	UPROPERTY(BlueprintReadOnly, Category = "Shooter|GameState")
	int32 CurrentAICount = 0;
	UPROPERTY(ReplicatedUsing = Rep_RedScore, BlueprintReadOnly, Category = "Shooter|GameState")
	int32 RedTeamScore = 0;
	UPROPERTY(ReplicatedUsing = Rep_BlueScore, BlueprintReadOnly, Category = "Shooter|GameState")
	int32 BlueTeamScore = 0;

	void AdvanceTimer();

	UFUNCTION()
	void Reset();

public:

	DECLARE_EVENT_OneParam(ADeathMatchGS, FOnPlayerAddAndRemove, ADeathMatchGS*)
	FOnPlayerAddAndRemove OnPlayerNum;
	DECLARE_EVENT_OneParam(ADeathMatchGS, TeamWinEvent, ETeam)
	TeamWinEvent OnTeamWin;
	DECLARE_EVENT(ADeathMatchGS, GameRestartEvent)
	GameRestartEvent OnGameRestart;
	GameRestartEvent OnResetAfterDelay;

	void BeginPlay() override;

	void AddScore(ETeam Team);
	void AddPlayerState(APlayerState* PlayerState) override;
	void RemovePlayerState(APlayerState* PlayerState) override;

	bool CanAddAI();
	void AddAI();
	void RemoveAI();

	int GetNbplayer();

	void UpdateEndHud(ETeam Team);

	UFUNCTION(BlueprintCallable)
	void EndGameTrigg();

	UFUNCTION(BlueprintCallable)
	void ResetAfterDelay();

	UFUNCTION(BlueprintNativeEvent)
	void ShowTeamWinHUD(ETeam Team);
	void ShowTeamWinHUD_Implementation(ETeam Team) {};
	
	int NewFrequency(int Sec);

	UFUNCTION()
	bool ArePlayersReady();

protected:
	UPROPERTY(Replicated)
	int MaxKill = 0;

	UPROPERTY(ReplicatedUsing = SetGameTimer)
	int GameTime = 0;

	UPROPERTY(Replicated)
	int MaxAIPerPlayer = 0;

	UPROPERTY(Replicated)
	int InvincibilityTimer = 0;

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >&
		OutLifetimeProps) const override
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(ADeathMatchGS, MaxKill);
		DOREPLIFETIME(ADeathMatchGS, GameTime);
		DOREPLIFETIME(ADeathMatchGS, MaxAIPerPlayer);
		DOREPLIFETIME(ADeathMatchGS, InvincibilityTimer);
		DOREPLIFETIME(ADeathMatchGS, RedTeamScore);
		DOREPLIFETIME(ADeathMatchGS, BlueTeamScore);
	}

	void SetUp();

	UFUNCTION()
	void SetGameTimer();

	UFUNCTION(Server, Reliable)
	void Server_AddScore(ETeam Team);

	UFUNCTION()
	void Rep_RedScore();

	UFUNCTION()
	void Rep_BlueScore();

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void Server_RestartGame();

public:
	int GetInvincibilityTime() { return InvincibilityTimer; }
};
