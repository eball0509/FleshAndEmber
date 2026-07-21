// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GAS_TestGameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AGAS_TestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AGAS_TestGameMode();
};



