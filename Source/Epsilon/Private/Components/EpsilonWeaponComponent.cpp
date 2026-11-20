// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/EpsilonWeaponComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UEpsilonWeaponComponent::UEpsilonWeaponComponent()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UEpsilonWeaponComponent::AttachToOwnerMesh(USkeletalMeshComponent* OwnerMesh)
{
	if (!OwnerMesh || !WeaponMesh) return;

	WeaponMesh->AttachToComponent(OwnerMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
	WeaponMesh->SetRelativeLocation(AttachLocationOffset);
	WeaponMesh->SetRelativeRotation(AttachRotationOffset);
}

FVector UEpsilonWeaponComponent::GetWeaponStartLocation() const
{
	if (!WeaponMesh) return FVector::ZeroVector;

	return WeaponMesh->DoesSocketExist(WeaponStartSocketName)
		? WeaponMesh->GetSocketLocation(WeaponStartSocketName)
		: WeaponMesh->GetComponentLocation();
}

FVector UEpsilonWeaponComponent::GetWeaponEndLocation() const
{
	if (!WeaponMesh) return FVector::ZeroVector;
	return WeaponMesh->DoesSocketExist(WeaponEndSocketName)
		? WeaponMesh->GetSocketLocation(WeaponEndSocketName)
		: WeaponMesh->GetComponentLocation();
}

void UEpsilonWeaponComponent::StartHitDetection()
{
	bHitDetectionActive = true;
	bHasPreviousFrameLocations = false; // 시작 시점엔 "이전 프레임"이 없으므로 리셋
	HitActorsThisSwing.Empty();
}

void UEpsilonWeaponComponent::StopHitDetection()
{
	bHitDetectionActive = false;
	bHasPreviousFrameLocations = false;
	HitActorsThisSwing.Empty();
}

void UEpsilonWeaponComponent::UpdateHitDetection()
{
	if (!bHitDetectionActive) return;

	const FVector CurrentStart = GetWeaponStartLocation();
	const FVector CurrentEnd = GetWeaponEndLocation();

	if (bHasPreviousFrameLocations)
	{
		PerformSweepTrace(PreviousStartLocation, CurrentStart);
		PerformSweepTrace(PreviousEndLocation, CurrentEnd);
	}

	PreviousStartLocation = CurrentStart;
	PreviousEndLocation = CurrentEnd;
	bHasPreviousFrameLocations = true;
}

void UEpsilonWeaponComponent::PerformSweepTrace(const FVector& PrevLoc, const FVector& CurrentLoc)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);
	ActorsToIgnore.Append(HitActorsThisSwing); // 이미 맞은 대상은 아예 검사 대상에서 제외

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		OwnerActor,
		PrevLoc,
		CurrentLoc,
		10.f, // 작은 반경의 구로 선분을 스윕 — 칼날 두께 정도로 설정
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None, // 디버깅 중엔 ForDuration 등으로 바꿔서 시각화 가능
		HitResults,
		true
	);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor || HitActor == OwnerActor) continue;
		if (HitActorsThisSwing.Contains(HitActor)) continue;

		HitActorsThisSwing.Add(HitActor);
		OnWeaponHit.Broadcast(HitActor);
	}
}


