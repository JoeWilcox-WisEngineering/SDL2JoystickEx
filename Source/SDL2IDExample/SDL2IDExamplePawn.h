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
#include "GameFramework/Character.h"
#include "SDL2KeyNotificationInterface.h"
#include "SDL2IDExamplePawn.generated.h"

class UPawnMovementComponent;

UCLASS(Blueprintable)
class ASDL2IDExamplePawn : public APawn, public ISDL2KeyNotificationInterface
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;
	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	ASDL2IDExamplePawn();

	virtual void Destroyed() override;

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/* How fast the ship can raise or lower */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float LiftSpeed;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	// Static names for axis bindings
	static const FName ThrottleBinding;
	static const FName LiftBinding;
	static const FName TurnBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	static const FName ShowMenuBinding;

private:

	float Throttle;
	float Lift;

	FVector FireAim;

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	UPawnMovementComponent* MovementComponent;

public:
	/** Returns ShipMeshComponent subobject **/
	FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }

	/** Returns CameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	/***
	 *      ___ ___  _    ___   ___     _            __
	 *     / __|   \| |  |_  ) |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
	 *     \__ \ |) | |__ / /   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
	 *     |___/___/|____/___| |___|_||_\__\___|_| |_| \__,_\__\___|
	 *
	 */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_PreviewInputKey(int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad);
	void SDL_PreviewInputKey_Implementation(int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_PreviewInputAxis(int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad);
	void SDL_PreviewInputAxis_Implementation(int32 ControllerId, FKey Key, float Delta, float DeltaTime, int32 NumSamples, bool bGamepad) {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_DeviceAdded(USDL2InputDeviceInfo* DeviceInfo, int32 ControllerId);
	void SDL_DeviceAdded_Implementation(USDL2InputDeviceInfo* DeviceInfo, int32 ControllerId) {};

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = SDL2)
	void SDL_DeviceRemoved(int32 ControllerId);
	void SDL_DeviceRemoved_Implementation(int32 ControllerId) {};

};

