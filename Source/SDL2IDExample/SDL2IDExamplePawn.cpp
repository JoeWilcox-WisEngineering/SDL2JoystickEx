/***
 *     __      __ _      ___    ___   _        _  _          _    ___             _  _  _            _           _
 *     \ \    / /(_) ___| __|  |   \ (_) __ _ (_)| |_  __ _ | |  | _ \ ___  __ _ | |(_)| |_  _  _   | |    __ _ | |__
 *      \ \/\/ / | |(_-<| _|   | |) || |/ _` || ||  _|/ _` || |  |   // -_)/ _` || || ||  _|| || |  | |__ / _` || '_ \
 *       \_/\_/  |_|/__/|___|  |___/ |_|\__, ||_| \__|\__,_||_|  |_|_\\___|\__,_||_||_| \__| \_, |  |____|\__,_||_.__/
 *                                      |___/                                                |__/
 *     ------------------------------------------------> Copyright (C) 2020 - WisEngineering LLC - All Rights Reserved
 */

#include "SDL2IDExamplePawn.h"
#include "SDL2IDExampleProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "SDL2InputDevice.h"
#include "DrawDebugHelpers.h"

const FName ASDL2IDExamplePawn::ThrottleBinding("Throttle");
const FName ASDL2IDExamplePawn::LiftBinding("Lift");
const FName ASDL2IDExamplePawn::TurnBinding("Turn");
const FName ASDL2IDExamplePawn::FireForwardBinding("FireForward");
const FName ASDL2IDExamplePawn::FireRightBinding("FireRight");
const FName ASDL2IDExamplePawn::ShowMenuBinding("ShowMenu");



ASDL2IDExamplePawn::ASDL2IDExamplePawn()
{	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	// Create the mesh component
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	
	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	FireSound = FireAudio.Object;
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->SetRelativeRotation(FRotator(-80.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Movement
	MoveSpeed = 1000.0f;
	LiftSpeed = 200.0f;
	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
}

void ASDL2IDExamplePawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	PlayerInputComponent->BindAxis(ThrottleBinding);
	PlayerInputComponent->BindAxis(LiftBinding);
	PlayerInputComponent->BindAxis(TurnBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);


	TSharedPtr<FSDL2InputDevice> SDLInputDevice =  FSDL2InputDevicePlugin::Get().GetInputDevice();
	if (SDLInputDevice.IsValid())
	{
		if (!SDLInputDevice->RegisterPreviewer(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("Could not register (%s) as an SDL2 Preview Object"), *GetNameSafe(this));
		}
	}
}

void ASDL2IDExamplePawn::Destroyed()
{
	Super::Destroyed();
	TSharedPtr<FSDL2InputDevice> SDLInputDevice = FSDL2InputDevicePlugin::Get().GetInputDevice();
	if (SDLInputDevice.IsValid())
	{
		SDLInputDevice->UnregisterPreviewer(this);
	}
}

void ASDL2IDExamplePawn::Tick(float DeltaSeconds)
{

	const float ThrottleValue = GetInputAxisValue(ThrottleBinding);
	const float LiftValue = GetInputAxisValue(LiftBinding);
	const float TurnValue = GetInputAxisValue(TurnBinding);

	// NOTE: This demo was created with a Logitech Extreme 3D Pro flightstick.  It returns values from
	// 40000 to -40000 for it's axis.  I just picked these values.  You should write a real calibration
	// system :)

	const float DeadZone = 20000;
	const float MaxRange = 20000; 


	float DeltaYaw = 0.0f;
	if (FMath::Abs(TurnValue) > DeadZone)
	{
		DeltaYaw = 180.0f * DeltaSeconds * (TurnValue < 0 ? -1.0 : 1.0f);
	}

	const FRotator NewRot = FRotator(0.0f, GetActorRotation().Yaw + DeltaYaw, 0.0f);

	FVector Movement = FVector(0, 0, 0);
	float AbsThrottleValue = FMath::Abs(ThrottleValue);
	if (AbsThrottleValue > DeadZone)
	{
		AbsThrottleValue -= DeadZone;
		AbsThrottleValue = MoveSpeed * (AbsThrottleValue / MaxRange) * DeltaSeconds * (ThrottleValue > 0 ? 1.0f : -1.0f);
		Movement = GetActorForwardVector() * AbsThrottleValue;
	}


	float AbsLift = FMath::Abs(LiftValue);
	if (AbsLift > DeadZone)
	{
		AbsLift -= DeadZone;

		// If we are flying up, set a ceiling
		if (LiftValue > 0)
		{
			FVector Start = GetActorLocation();
			FVector End = Start + FVector(0.0f, 0.0f, -350.0f);	// We can fly up or down 600 units
			FHitResult TraceHit(1.f);

			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			if (!GetWorld()->LineTraceSingleByChannel(TraceHit, Start, End, ECC_Pawn, CollisionParams))
			{
				AbsLift = 0;
			}
		}

		AbsLift = LiftSpeed * (AbsLift / MaxRange) * DeltaSeconds * (LiftValue > 0 ? 1.0f : -1.0f);
		Movement.Z += AbsLift;
	}

	FHitResult Hit(1.f);
	RootComponent->MoveComponent(Movement, NewRot, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
		const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.f - Hit.Time);
		RootComponent->MoveComponent(Deflection, NewRot, true);
	}

	if (!FireAim.IsNearlyZero())
	{
		FireShot(FireAim);
	}

}

void ASDL2IDExamplePawn::FireShot(FVector FireDirection)
{
	// If it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// spawn the projectile
				World->SpawnActor<ASDL2IDExampleProjectile>(SpawnLocation, FireRotation);
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &ASDL2IDExamplePawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			bCanFire = false;
		}
	}
}

void ASDL2IDExamplePawn::ShotTimerExpired()
{
	bCanFire = true;
}

/***
 *      ___ ___  _    ___   ___     _            __
 *     / __|   \| |  |_  ) |_ _|_ _| |_ ___ _ _ / _|__ _ __ ___
 *     \__ \ |) | |__ / /   | || ' \  _/ -_) '_|  _/ _` / _/ -_)
 *     |___/___/|____/___| |___|_||_\__\___|_| |_| \__,_\__\___|
 *
 */

void ASDL2IDExamplePawn::SDL_PreviewInputKey_Implementation(int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad)
{
	FVector NewFireAim;

	UE_LOG(LogTemp, Log, TEXT("Key %s %i"), *Key.ToString(), Event);

	     if (Key == FSDL2InputKeys::RawSDL2Input_HatCentered)	FireAim = FVector();
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatUp)			FireAim = FVector(1.0f, 0.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatUpRight)	FireAim = FVector(1.0f, 1.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatRight)		FireAim = FVector(0.0f, 1.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatDownRight)	FireAim = FVector(-1.0f, 1.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatDown)		FireAim = FVector(-1.0f, 0.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatDownLeft)	FireAim = FVector(-1.0f, -1.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatLeft)		FireAim = FVector(0.0f, -1.0f, 0.0f);
	else if (Key == FSDL2InputKeys::RawSDL2Input_HatUpLeft)		FireAim = FVector(1.0f, -1.0f, 0.0f);
}
