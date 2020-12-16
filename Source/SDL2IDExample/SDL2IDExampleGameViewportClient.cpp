/***
 *     __      __ _      ___    ___   _        _  _          _    ___             _  _  _            _           _
 *     \ \    / /(_) ___| __|  |   \ (_) __ _ (_)| |_  __ _ | |  | _ \ ___  __ _ | |(_)| |_  _  _   | |    __ _ | |__
 *      \ \/\/ / | |(_-<| _|   | |) || |/ _` || ||  _|/ _` || |  |   // -_)/ _` || || ||  _|| || |  | |__ / _` || '_ \
 *       \_/\_/  |_|/__/|___|  |___/ |_|\__, ||_| \__|\__,_||_|  |_|_\\___|\__,_||_||_| \__| \_, |  |____|\__,_||_.__/
 *                                      |___/                                                |__/
 *     ------------------------------------------------> Copyright (C) 2020 - WisEngineering LLC - All Rights Reserved
 */

#include "SDL2IDExampleGameViewportClient.h"
#include "SDL2InputTypes.h"

USDL2IDExampleGameViewportClient::USDL2IDExampleGameViewportClient()
{
}

bool USDL2IDExampleGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
/*	UNCOMMENT to Show all input
	
	UE_LOG(LogTemp, Log
		, TEXT("InputKey %s [%i] Controller Id=%i")
		, *EventArgs.Key.ToString()
		, EventArgs.Event
		, EventArgs.ControllerId
	);
*/
	// Start was pressed.  Time to see if we should add a new player....

	if ((EventArgs.Key == EKeys::Gamepad_Special_Right 
		|| EventArgs.Key == FSDL2InputKeys::RawSDL2Input_Button8
		|| EventArgs.Key == FSDL2InputKeys::RawSDL2Input_Button1)
		&& EventArgs.Event == IE_Released)
	{
		// First we make sure this is an SDL2 Controller.  Remember that SDL2InputDevice and Xinput exist
		// at the same time and you can actually get 2 events here from the same controller.  So we make sure
		// this is from the SDL2InputDevice

		if (EventArgs.ControllerId >= 10)	
		{
			// Search all of the Local Players and see if we match one.
			TArray<ULocalPlayer*> ExistingPlayers = GameInstance->GetLocalPlayers();
			for (int32 i = 0; i < ExistingPlayers.Num(); i++)
			{
				if (ExistingPlayers[i] != nullptr && ExistingPlayers[i]->GetControllerId() == EventArgs.ControllerId)
				{
					// This player already exists in the game, don't create him again
					return Super::InputKey(EventArgs);
				}
			}

			// We now need to see if this is the first player.  If it is and that player's controller id is less than
			// 10 we need to take control of him.

			if (ExistingPlayers.Num() == 1 && ExistingPlayers[0] != nullptr && ExistingPlayers[0]->GetControllerId() < 10)
			{
				ExistingPlayers[0]->SetControllerId(EventArgs.ControllerId);
				if (ExistingPlayers[0]->GetPlayerController(GetWorld()) != nullptr)
				{
					ExistingPlayers[0]->GetPlayerController(GetWorld())->ServerRestartPlayer();
				}
			}
			else
			{
				// Add a new player. 
				check(GameInstance);
				FString ErrorString;
				ULocalPlayer* LocalPlayer = GameInstance->CreateLocalPlayer(EventArgs.ControllerId, ErrorString, true);
				check(LocalPlayer);
			}
		}
	}


	return Super::InputKey(EventArgs);
}
