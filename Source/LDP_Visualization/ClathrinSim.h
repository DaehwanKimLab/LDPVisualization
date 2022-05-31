// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

#include "ClathrinSim.generated.h"

UCLASS()
class LDP_VISUALIZATION_API AClathrinSim : public AActor
{
	GENERATED_BODY()
	
public:

	// Bone names
	UPROPERTY(EditAnywhere, Category="Debug")
	FName DebugName;

	UPROPERTY(EditAnywhere, Category="Debug")
	float LinearStiffness;

	UPROPERTY(EditAnywhere, Category="Debug")
	float AngularStiffness;
	
	UPROPERTY(EditAnywhere, Category="Bone Names")
	FName BN_Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Bone Names")
	FName BN_Leg1_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bone Names")
	FName BN_Leg1_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bone Names")
	FName BN_Leg2_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bone Names")
	FName BN_Leg2_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bone Names")
	FName BN_Leg3_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bone Names")
	FName BN_Leg3_2;

	UPROPERTY(EditAnywhere)
	class UDataTable* DTClathrinOrigins;

	UPROPERTY(EditAnywhere)
	class UDataTable* DTClathrinCtrlPoints;

	UPROPERTY(EditAnywhere)
	class UDataTable* DTClathrinEulerAngles;
	
	UPROPERTY(EditAnywhere)
	class UStaticMesh* DebugMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PositionUnitScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeshUnitScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DebugMeshUnitScale;
	
	UPROPERTY(EditAnywhere)
	class USkeletalMesh* ClathrinSkeleton;

	// Sets default values for this actor's properties
	AClathrinSim();

	UPROPERTY()
	UPhysicsHandleComponent* phc;
protected:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetClathrinOrigins();
	
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetClathrinCtrlPoints();

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetClathrinEulerAngles();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
