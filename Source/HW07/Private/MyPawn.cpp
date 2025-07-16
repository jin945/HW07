#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "UObject/ConstructorHelpers.h"

AMyPawn::AMyPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Capsule->InitCapsuleSize(40.f, 150.f);
    Capsule->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
    RootComponent = Capsule;
    Capsule->SetSimulatePhysics(false);


    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMesh->SetupAttachment(RootComponent);
    SkeletalMesh->SetRelativeLocation(FVector(0.f, 0.f, -180.f));
    SkeletalMesh->SetSimulatePhysics(false);

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Fab/Percival_-__Custom_Mecha_Mech_Design/SKM_percival.SKM_percival"));
    if (MeshAsset.Succeeded())
        SkeletalMesh->SetSkeletalMesh(MeshAsset.Object);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
    SpringArm->TargetArmLength = 350.f;
    SpringArm->bUsePawnControlRotation = false;
    SpringArm->bInheritYaw = false;
    SpringArm->bInheritPitch = false;
    SpringArm->bInheritRoll = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;

    static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingAsset(TEXT("/Game/IA/IMC_MyPawn.IMC_MyPawn"));
    if (InputMappingAsset.Succeeded())
        InputMapping = InputMappingAsset.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionAsset(TEXT("/Game/IA/IA_MOVE.IA_MOVE"));
    if (MoveActionAsset.Succeeded())
        MoveAction = MoveActionAsset.Object;

    static ConstructorHelpers::FObjectFinder<UInputAction> LookActionAsset(TEXT("/Game/IA/IA_LOOK.IA_LOOK"));
    if (LookActionAsset.Succeeded())
        LookAction = LookActionAsset.Object;

    MoveInput = FVector2D::ZeroVector;
    LookInput = FVector2D::ZeroVector;
    CurrentYaw = 0.f;
    CurrentPitch = 0.f;
}

void AMyPawn::BeginPlay()
{
    Super::BeginPlay();

    SkeletalMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            if (InputMapping)
                Subsystem->AddMappingContext(InputMapping, 0);
        }
    }
}

void AMyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentYaw += LookInput.X * LookSpeed * DeltaTime;
    CurrentPitch = FMath::Clamp(CurrentPitch + LookInput.Y * LookSpeed * DeltaTime, MinPitch, MaxPitch);
    SpringArm->SetRelativeRotation(FRotator(CurrentPitch, CurrentYaw, 0.f));

    if (!MoveInput.IsNearlyZero())
    {
        FVector CameraForward = Camera->GetForwardVector();
        CameraForward.Z = 0.f;
        CameraForward.Normalize();

        FVector CameraRight = Camera->GetRightVector();
        CameraRight.Z = 0.f;
        CameraRight.Normalize();

        FVector MoveDirection = CameraForward * MoveInput.Y + CameraRight * MoveInput.X;
        MoveDirection.Normalize();

        SetActorRotation(FRotator(0.f, MoveDirection.Rotation().Yaw, 0.f));
        AddActorWorldOffset(MoveDirection * MoveSpeed * DeltaTime, true);
    }
}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
            EIC->BindAction(MoveAction, ETriggerEvent::Completed, this, &AMyPawn::StopMove);
        }
        if (LookAction)
        {
            EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
            EIC->BindAction(LookAction, ETriggerEvent::Completed, this, &AMyPawn::StopLook);
        }
    }
}

void AMyPawn::Move(const FInputActionValue& Value)
{
    MoveInput = Value.Get<FVector2D>();
}

void AMyPawn::StopMove(const FInputActionValue& Value)
{
    MoveInput = FVector2D::ZeroVector;
}

void AMyPawn::Look(const FInputActionValue& Value)
{
    LookInput = Value.Get<FVector2D>();
}

void AMyPawn::StopLook(const FInputActionValue& Value)
{
    LookInput = FVector2D::ZeroVector;
}
