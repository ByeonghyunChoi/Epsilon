// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EpsilonCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/EpsilonAttributeSet.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystem/Abilities/GA_ComboAttack.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/EpsilonTargetingComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/EpsilonWeaponComponent.h"

AEpsilonCharacter::AEpsilonCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UEpsilonAttributeSet>(TEXT("AttributeSet"));
	TargetingComponent = CreateDefaultSubobject<UEpsilonTargetingComponent>(TEXT("TargetingComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	WeaponComponent = CreateDefaultSubobject<UEpsilonWeaponComponent>(TEXT("WeaponComponent"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true; // 카메라는 컨트롤러 회전을 따라감

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // SpringArm이 이미 회전을 적용하므로 카메라 자체는 추가 회전 불필요
}

UAbilitySystemComponent* AEpsilonCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AEpsilonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		GiveStartupAbilities();
	}
}

void AEpsilonCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	UpdateAttackCameraLock(deltaTime);

	if (WeaponComponent)
	{
		WeaponComponent->UpdateHitDetection();
	}
}

void AEpsilonCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (WeaponComponent && GetMesh())
	{
		WeaponComponent->AttachToOwnerMesh(GetMesh());
	}
}

UEpsilonTargetingComponent* AEpsilonCharacter::GetEpsilonTargetingComponent() const
{
	return TargetingComponent;
}

UEpsilonWeaponComponent* AEpsilonCharacter::GetWeaponComponent() const
{
	return WeaponComponent;
}

void AEpsilonCharacter::OnMoveInput(const FInputActionValue& Value)
{
	if (AbilitySystemComponent &&
		AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Attack.Active"))))
	{
		return; // 공격 중에는 이동 입력 무시
	}

	const FVector2D MoveInput = Value.Get<FVector2D>();
	
	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, ControlRotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MoveInput.Y);
		AddMovementInput(RightDirection, MoveInput.X);
	}
}

void AEpsilonCharacter::OnLookInput(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}

void AEpsilonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (BasicAttackInputAction)
		{
			EIC->BindAction(BasicAttackInputAction, ETriggerEvent::Started, this, &AEpsilonCharacter::OnBasicAttackInput);
		}

		if (MoveInputAction)
		{
			EIC->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AEpsilonCharacter::OnMoveInput);
		}

		if (MouseLookInputAction)
		{
			EIC->BindAction(MouseLookInputAction, ETriggerEvent::Triggered, this, &AEpsilonCharacter::OnLookInput);
		}
	}


}

void AEpsilonCharacter::OnBasicAttackInput(const FInputActionValue& Value)
{
	if (!AbilitySystemComponent) return;

	if (UGA_ComboAttack* ComboInstance = UGA_ComboAttack::FindActiveInstance(this))
	{
		ComboInstance->RequestNextCombo();
		return;
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Attack.Combo")));
	AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}

void AEpsilonCharacter::GiveStartupAbilities()
{
	if (!AbilitySystemComponent) return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(AbilityClass, 1, 0, this)
			);
		}
	}
}

void AEpsilonCharacter::UpdateAttackCameraLock(float DeltaTime)
{
	if (!AbilitySystemComponent) return;

	const bool bIsAttacking = AbilitySystemComponent->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag(FName("State.Attack.Active")));

	if (!bIsAttacking) return;

	AActor* LockedTarget = TargetingComponent ? TargetingComponent->GetOrRefreshLockedTarget() : nullptr;
	if (!IsValid(LockedTarget)) return;

	if (AController* PC = GetController())
	{
		const FVector ToTarget = LockedTarget->GetActorLocation() - GetActorLocation();
		const FRotator DesiredRotation = ToTarget.Rotation();

		const FRotator CurrentRotation = PC->GetControlRotation();
		const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, DesiredRotation, DeltaTime, AttackCameraLockInterpSpeed);

		PC->SetControlRotation(NewRotation);
	}
}


