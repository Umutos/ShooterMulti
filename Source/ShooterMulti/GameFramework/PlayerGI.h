#pragma once

#include "Engine/GameInstance.h"
#include "DeathMatchGM.h"
#include "PlayerGI.generated.h"

USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	ETeam TeamNum;
	UPROPERTY(BlueprintReadWrite)
	FString UserName;
};

UCLASS()
class SHOOTERMULTI_API UPlayerGI : public UGameInstance
{
	GENERATED_BODY()
	
	FPlayerInfo UserInfo;

public:
	UPlayerGI(const FObjectInitializer& ObjInit);
	
	FPlayerInfo GetUserInfo();

	UFUNCTION(BlueprintCallable)
	void SetUserInfo(int32 InTeamNum, const FString& InUserName);
	
	UFUNCTION(BlueprintCallable)
	void SetUsername(const FString& InUserName);

	UFUNCTION(BlueprintCallable)
	FString GetUsername();
	
	UFUNCTION(BlueprintCallable)
	void SetTeamNum(int32 InTeamNum);
};