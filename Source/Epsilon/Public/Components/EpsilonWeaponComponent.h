// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EpsilonWeaponComponent.generated.h"

class UStaticMeshComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponHit, AActor*);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EPSILON_API UEpsilonWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEpsilonWeaponComponent();


public:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName AttachSocketName = FName("HandGrip_R");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FVector AttachLocationOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FRotator AttachRotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|HitDetection")
	FName WeaponStartSocketName = FName("WeaponStart");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|HitDetection")
	FName WeaponEndSocketName = FName("WeaponEnd");

public:
	FOnWeaponHit OnWeaponHit;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachToOwnerMesh(USkeletalMeshComponent* OwnerMesh);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|HitDetection")
	FVector GetWeaponStartLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|HitDetection")
	FVector GetWeaponEndLocation() const;

	// 히트 디텍션 시작/종료 (AnimNotifyState가 호출)
	UFUNCTION(BlueprintCallable, Category = "Weapon|HitDetection")
	void StartHitDetection();

	UFUNCTION(BlueprintCallable, Category = "Weapon|HitDetection")
	void StopHitDetection();

	// 매 프레임 호출 (캐릭터의 Tick에서 호출 예정)
	UFUNCTION(BlueprintCallable, Category = "Weapon|HitDetection")
	void UpdateHitDetection();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	bool bHitDetectionActive = false;
	bool bHasPreviousFrameLocations = false;
	FVector PreviousStartLocation;
	FVector PreviousEndLocation;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> HitActorsThisSwing;

protected:
	void PerformSweepTrace(const FVector& PrevLoc, const FVector& CurrentLoc);
		
};
