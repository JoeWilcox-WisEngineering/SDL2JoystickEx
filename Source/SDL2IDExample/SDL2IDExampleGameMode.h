/***
 *     __      __ _      ___    ___   _        _  _          _    ___             _  _  _            _           _
 *     \ \    / /(_) ___| __|  |   \ (_) __ _ (_)| |_  __ _ | |  | _ \ ___  __ _ | |(_)| |_  _  _   | |    __ _ | |__
 *      \ \/\/ / | |(_-<| _|   | |) || |/ _` || ||  _|/ _` || |  |   // -_)/ _` || || ||  _|| || |  | |__ / _` || '_ \
 *       \_/\_/  |_|/__/|___|  |___/ |_|\__, ||_| \__|\__,_||_|  |_|_\\___|\__,_||_||_| \__| \_, |  |____|\__,_||_.__/
 *                                      |___/                                                |__/
 *     ------------------------------------------------> Copyright (C) 2020 - WisEngineering LLC - All Rights Reserved
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SDL2IDExampleGameMode.generated.h"

UCLASS(MinimalAPI)
class ASDL2IDExampleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASDL2IDExampleGameMode();

	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;
	virtual void StartPlay() override;
	virtual void Destroyed() override;

	void InitSDL();

	/**
	 * It's much easier to create an add UI in blueprints, so we'll add the global press START to join
	 * message there.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = Game)
	void CreateUI();


	/***
	 *      ___ ___  _    ___   ___     _            __
	 *     / __|   \| |  |_  ) |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
	 *     \__ \ |) | |__ / /   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
	 *     |___/___/|____/___| |___|_||_\__\___|_| |_| \__,_\__\___|
	 *
	 */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_PreviewInputKey(int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad);
	void SDL_PreviewInputKey_Implementation(int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad){};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_PreviewInputAxis(int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad);
	void SDL_PreviewInputAxis_Implementation(int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad){};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_DeviceAdded(USDL2InputDeviceInfo* DeviceInfo, int32 ControllerId);
	void SDL_DeviceAdded_Implementation(USDL2InputDeviceInfo* DeviceInfo, int32 ControllerId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_DeviceRemoved(int32 ControllerId);
	void SDL_DeviceRemoved_Implementation(int32 ControllerId);
};



