// Copyright 2018 Stuart McDonald.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UE4_BATTLETANK_API IMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void DeterminePlayerInput() = 0;

	virtual void HostOffline() = 0;

	virtual void HostOnline() = 0;

	virtual void JoinOnline() = 0;

	virtual void ReturnToMainMenu() = 0;

	virtual void OuitGame() = 0;
	
};
