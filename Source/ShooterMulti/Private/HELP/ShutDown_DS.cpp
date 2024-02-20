// Fill out your copyright notice in the Description page of Project Settings.


#include "HELP/ShutDown_DS.h"

void UShutDown_DS::ShutDown()
{
	GIsRequestingExit = true;
}
