#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MyPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class HW07_API AMyPawn : public APawn
{
    GENERATED_BODY()

public:
    AMyPawn();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* Capsule;

    UPROPERTY(VisibleAnywhere)
    USkeletalMeshComponent* SkeletalMesh;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    UPROPERTY(EditAnywhere)
    UInputMappingContext* InputMapping;

    UPROPERTY(EditAnywhere)
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere)
    UInputAction* LookAction;


    FVector2D MoveInput;
    FVector2D LookInput;


    float CurrentYaw;
    float CurrentPitch;


    float MoveSpeed = 300.f;
    float LookSpeed = 100.f;
    float MinPitch = -80.f;
    float MaxPitch = 80.f;


    void Move(const FInputActionValue& Value);
    void StopMove(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StopLook(const FInputActionValue& Value);
};
