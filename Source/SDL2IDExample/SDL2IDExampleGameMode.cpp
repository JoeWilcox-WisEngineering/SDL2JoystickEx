/***
 *     __      __ _      ___    ___   _        _  _          _    ___             _  _  _            _           _
 *     \ \    / /(_) ___| __|  |   \ (_) __ _ (_)| |_  __ _ | |  | _ \ ___  __ _ | |(_)| |_  _  _   | |    __ _ | |__
 *      \ \/\/ / | |(_-<| _|   | |) || |/ _` || ||  _|/ _` || |  |   // -_)/ _` || || ||  _|| || |  | |__ / _` || '_ \
 *       \_/\_/  |_|/__/|___|  |___/ |_|\__, ||_| \__|\__,_||_|  |_|_\\___|\__,_||_||_| \__| \_, |  |____|\__,_||_.__/
 *                                      |___/                                                |__/
 *     ------------------------------------------------> Copyright (C) 2020 - WisEngineering LLC - All Rights Reserved
 */

#include "SDL2IDExampleGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "SDL2IDExamplePawn.h"

ASDL2IDExampleGameMode::ASDL2IDExampleGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = ASDL2IDExamplePawn::StaticClass();
}

bool ASDL2IDExampleGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	// For this demo, we just don't allow the player to restart if they don't have an SDL2 ControllerId. 
	if (Player != nullptr)
	{
		ULocalPlayer* LocalPlayer = Player->GetLocalPlayer();
		if (LocalPlayer != nullptr && LocalPlayer->GetControllerId() < 10)
		{
			return false;
		}
	}

	return Super::PlayerCanRestart_Implementation(Player);
}

/**
 * In StartPlay() we will register as a previewer with the SDL2InputDevice plugin so we can
 * get events, and then create the UI. There are a lot of places where this could be done but
 * StartPlay() is fine for this example.
 */

void ASDL2IDExampleGameMode::StartPlay()
{
	Super::StartPlay();

	FTimerHandle TmpHandle;
	GetWorldTimerManager().SetTimer(TmpHandle, this, &ASDL2IDExampleGameMode::InitSDL, 0.2, false);
}

void ASDL2IDExampleGameMode::InitSDL()
{
	FSDL2InputDevicePlugin::Get().GetInputDevice()->RegisterPreviewer(this);
	CreateUI();
}


/**
 * If you register as a previewer, you have to make sure you unregister when you are destroyed.  The plugin
 * does it's best to ignore broken references but safety first.
 */

void ASDL2IDExampleGameMode::Destroyed()
{
	FSDL2InputDevicePlugin::Get().GetInputDevice()->UnregisterPreviewer(this);
	Super::Destroyed();
}

/***
 *      ___ ___  _    ___   ___     _            __
 *     / __|   \| |  |_  ) |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
 *     \__ \ |) | |__ / /   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
 *     |___/___/|____/___| |___|_||_\__\___|_| |_| \__,_\__\___|
 *
 */

void ASDL2IDExampleGameMode::SDL_DeviceAdded_Implementation(USDL2InputDeviceInfo* DeviceInfo, int32 ControllerId)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("Input Device Added: (%i) - %s"), ControllerId, *DeviceInfo->ToString());
}
void ASDL2IDExampleGameMode::SDL_DeviceRemoved_Implementation(int32 ControllerId)
{
	UE_LOG(LogTemp, VeryVerbose, TEXT("Input Device Removed: (%i)"), ControllerId);

	// We are going to look to see if this control is attached to a player in the game.  If it is,
	// then we will clean up the player and remove them from the game.

	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		if (GameState->PlayerArray[i] != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(GameState->PlayerArray[i]->GetOwner());
			if (PlayerController != nullptr)
			{
				ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
				if (LocalPlayer != nullptr && LocalPlayer->GetControllerId() == ControllerId)
				{
					// This player has disconnected.   Kill the pawn
					if (PlayerController->GetPawn() != nullptr)
					{
						PlayerController->GetPawn()->Destroy();
					}

					// Now kill the LocalPlayer
					UGameInstance* GameInstance = GetWorld()->GetGameInstance();
					if (GameInstance != nullptr)
					{
						GameInstance->RemoveLocalPlayer(LocalPlayer);
					}
				}
			}
		}
	}

	/*
			 | \| |/ _ \_   _| __|
			 | .` | (_) || | | _|
			 |_|\_|\___/ |_| |___|

			 We don't have to remove the controller here.  We can just pause it and wait for a new
			 controller to reconnect.  That's outside of this scope (needs ui/etc).  But in a nutshell 
			 you would flag the controller as being disconnected and in the GameViewportClient try to 
			 reestablish connection when start is pressed again.

			 Really, I just added this here so you can see how the KeyNotificationInterface works.
	*/

}

