// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShutDown_DS.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTERMULTI_API UShutDown_DS : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Custom", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", Keywords = "Shut Down"))
	static void ShutDown();

	
};
