// Fill out your copyright notice in the Description page of Project Settings.


#include "ClathrinSim.h"

#include "DataTableUtils.h"
#include "DrawDebugHelpers.h"
#include "PositionDataTable.h"
#include "Components/CapsuleComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values
AClathrinSim::AClathrinSim()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

TArray<FVector> AClathrinSim::GetClathrinOrigins()
{
	static const FString ContextString(TEXT("Player Attack Montage"));
	TArray<FPositionDataTable*> ClathrinOrigins;
	DTClathrinOrigins->GetAllRows(ContextString, ClathrinOrigins);

	TArray<FVector> ToRet;
	for(int i = 0 ; i < ClathrinOrigins.Num(); i++)
	{
		ToRet.Add(ClathrinOrigins[i]->GetPos());
	}
	return ToRet;
}

TArray<FVector> AClathrinSim::GetClathrinCtrlPoints()
{
	static const FString ContextString(TEXT("Player Attack Montage"));
	TArray<FPositionDataTable*> ClathrinOrigins;
	DTClathrinCtrlPoints->GetAllRows(ContextString, ClathrinOrigins);

	TArray<FVector> ToRet;
	for(int i = 0 ; i < ClathrinOrigins.Num(); i++)
	{
		ToRet.Add(ClathrinOrigins[i]->GetPos());
	}
	return ToRet;
}

TArray<FVector> AClathrinSim::GetClathrinEulerAngles()
{
	static const FString ContextString(TEXT("Player Attack Montage"));
	TArray<FPositionDataTable*> ClathrinOrigins;
	DTClathrinEulerAngles->GetAllRows(ContextString, ClathrinOrigins);

	TArray<FVector> ToRet;
	for(int i = 0 ; i < ClathrinOrigins.Num(); i++)
	{
		ToRet.Add(ClathrinOrigins[i]->GetPos());
	}
	return ToRet;
}

// Called when the game starts or when spawned
void AClathrinSim::BeginPlay()
{
	Super::BeginPlay();
	return;
	
			static const FString ContextString(TEXT("Player Attack Montage"));
	FPositionDataTable *pos = DTClathrinOrigins->FindRow<FPositionDataTable>(FName(TEXT("1")), ContextString, true);
	if(pos)
	{
		UE_LOG(LogTemp, Log, TEXT("%f, %f, %f"), pos->XPos, pos->YPos, pos->ZPos);
	}

	TArray<FPositionDataTable*> ClathrinOrigins;
	DTClathrinOrigins->GetAllRows(ContextString, ClathrinOrigins);

	TArray<FPositionDataTable*> CtrlPts;
	DTClathrinCtrlPoints->GetAllRows(ContextString, CtrlPts);

	TArray<FPositionDataTable*> EulerAngles;
	DTClathrinEulerAngles->GetAllRows(ContextString, EulerAngles);
	
	int numClathrins = ClathrinOrigins.Num();
	FColor colors[6] = {FColor::Black, FColor::Blue, FColor::Cyan, FColor::Red, FColor::Green, FColor::Orange} ;
	UE_LOG(LogTemp, Log, TEXT("here"));
	for(int i = 0 ; i < numClathrins; i++)
	{
		int targetInd = 1;
		if(i != targetInd - 1)
			continue;

		UE_LOG(LogTemp, Log, TEXT("CLATHRIN %d ----------"), i + 1);

		FVector org = ClathrinOrigins[i]->GetPos() * FVector(1,-1, 1) * PositionUnitScale;
		UE_LOG(LogTemp, Log, TEXT("Position %s"), *org.ToString());
		
		//UPoseableMeshComponent *pcomp = NewObject<UPoseableMeshComponent>(this);
		USkeletalMeshComponent *pcomp = NewObject<USkeletalMeshComponent>(this);
		pcomp->SkeletalMesh = ClathrinSkeleton;
		//pcomp->SetPhysicsAsset(ClathrinSkeleton->PhysicsAsset);
		pcomp->SetRelativeLocation(org);
		pcomp->SetRelativeScale3D(FVector(MeshUnitScale, MeshUnitScale, MeshUnitScale));
		pcomp->SetSimulatePhysics(true);
		pcomp->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
		pcomp->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		pcomp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECR_Ignore);
		pcomp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		pcomp->SetEnableGravity(false);
		pcomp->RegisterComponent();

		RootComponent = pcomp;
		//UCapsuleComponent *capcomp = NewObject<UCapsuleComponent>(this);
		//capcomp->AttachToComponent(pcomp, FAttachmentTransformRules::SnapToTargetIncludingScale, FName("Bone_024Socket"));
		//capcomp->RegisterComponent();
		
		// 6 control points per origin
		int startInd = i * 6;

		// 0 4 , 2 3 , 1 5
		TArray<FVector> pts;
		for(FPositionDataTable* fpos : CtrlPts)
			pts.Add(fpos->GetPos());

		FVector avg = FVector::ZeroVector;
		for(FVector vec : pts)
			avg += vec;
		avg = avg / pts.Num();

		//FRotator rot = UKismetMathLibrary::FindLookAtRotation(org, avg);
		//pcomp->SetRelativeRotation(rot);


		/*
		*-0.218629333393989	-0.659352165868088	-0.719344101209401
		-0.818564646444268	0.525205148070717	-0.232618726743114
		0.531180886489029	0.537972372715437	-0.654547623953866
		 *
		 */
		FVector euler = EulerAngles[i]->GetPos() * FVector(1, -1, -1);
		UE_LOG(LogTemp, Log, TEXT("Original Euler: %s"), *euler.ToString());
		euler = FMath::RadiansToDegrees(euler);
		UE_LOG(LogTemp, Log, TEXT("Euler (deg): %s"), *euler.ToString());
		FRotator rot = FRotator::MakeFromEuler(euler);
		//UE_LOG(LogTemp, Log, TEXT("roll: %f pitch: %f yaw: %f"), rot.Roll, rot.Pitch, rot.Yaw);
		
		pcomp->SetRelativeRotation(rot);

		FName boneNames[] = {BN_Leg1_1, BN_Leg3_1, BN_Leg2_1, BN_Leg2_2, BN_Leg1_2, BN_Leg3_2};
		//for(int j = 0 ; j < 6; j++)
		//{
			FVector smcPos = CtrlPts[startInd]->GetPos() * PositionUnitScale * FVector(1,-1, 1);
			UStaticMeshComponent *smc = NewObject<UStaticMeshComponent>(this);
			smc->SetStaticMesh(DebugMesh);
			smc->SetRelativeLocation(smcPos);
			smc->SetRelativeScale3D(FVector(DebugMeshUnitScale, DebugMeshUnitScale, DebugMeshUnitScale));
			smc->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			smc->RegisterComponent();
			
			
			phc = NewObject<UPhysicsHandleComponent>(this);
		phc->RegisterComponent();
			phc->SetAutoActivate(true);
		
			
			phc->bInterpolateTarget = true;
			phc->LinearStiffness = LinearStiffness;
			phc->AngularStiffness = AngularStiffness;
		
			FVector boneLoc = pcomp->GetBoneLocation(DebugName);
		
		
			phc->GrabComponentAtLocation(pcomp, DebugName, pcomp->GetBoneLocation(DebugName));
			phc->SetTargetLocation(smcPos);
			//UE_LOG(LogTemp, Log, TEXT(""));
			
			
			//phc->SetTargetLocation(smcPos);
		UE_LOG(LogTemp, Log, TEXT("Bone Loc: %s Comp: %s Bone: %s %f"), *boneLoc.ToString(), *phc->GetGrabbedComponent()->GetName(), *phc->GrabbedBoneName.ToString(), phc->InterpolationSpeed);

		
		//
		//
		//}
		/*
		pcomp->SetBoneLocationByName(BN_Leg1_1, CtrlPts[startInd]->GetPos(), EBoneSpaces::ComponentSpace);
		pcomp->SetBoneLocationByName(BN_Leg1_2, CtrlPts[startInd + 4]->GetPos(), EBoneSpaces::ComponentSpace);
		pcomp->SetBoneLocationByName(BN_Leg2_1, CtrlPts[startInd + 2]->GetPos(), EBoneSpaces::ComponentSpace);
		pcomp->SetBoneLocationByName(BN_Leg2_2, CtrlPts[startInd + 3]->GetPos(), EBoneSpaces::ComponentSpace);
		pcomp->SetBoneLocationByName(BN_Leg3_1, CtrlPts[startInd + 1]->GetPos(), EBoneSpaces::ComponentSpace);
		pcomp->SetBoneLocationByName(BN_Leg3_2, CtrlPts[startInd + 5]->GetPos(), EBoneSpaces::ComponentSpace);
		*/
	}
	
}

void AClathrinSim::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

// Called every frame
void AClathrinSim::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

