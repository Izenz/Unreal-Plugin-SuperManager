// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickActorActionsWidget.generated.h"

UENUM(BlueprintType)
enum class E_DuplicationAxis : uint8
{
		EDA_XAxis UMETA (Displayname = "X Axis"),
		EDA_YAxis UMETA (Displayname = "Y Axis"),
		EDA_ZAxis UMETA (Displayname = "Z Axis"),
		EDA_MAX UMETA (Displayname = "Default Max"),
};

USTRUCT(BlueprintType)

struct FRandomActorRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRandomizeYawRot = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bRandomizeYawRot"))
	float YawRotMin = -45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bRandomizeYawRot"))
	float YawRotMax = 45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bRandomizePitchRot = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (EditCondition = "bRandomizePitchRot"))
	float PitchRotMin = -45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (EditCondition = "bRandomizePitchRot"))
	float PitchRotMax = 45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bRandomizeRollRot = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (EditCondition = "bRandomizeRollRot"))
	float RollRotMin = -45.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (EditCondition = "bRandomizeRollRot"))
	float RollRotMax = 45.f;

};

UCLASS()
class SUPERMANAGER_API UQuickActorActionsWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
#pragma region ActorBatchSelection

	UFUNCTION(BlueprintCallable)
	void SelectAllActorsWithSimilarName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchSelection")
	TEnumAsByte<ESearchCase::Type>	SearchCase = ESearchCase::IgnoreCase;

#pragma endregion

#pragma region ActorBatchDuplication

	UFUNCTION(BlueprintCallable)
	void DuplicateActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	E_DuplicationAxis DuplicationAxis = E_DuplicationAxis::EDA_XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	int32 NumOfWantedDuplicates = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ActorBatchDuplication")
	float OffsetDistance = 300.f;

#pragma endregion

#pragma region RandomizeActorTransform

	UFUNCTION(BlueprintCallable)
	void RandomizeActorTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RandomizeActorTransform")
	FRandomActorRotation RandomActorRotation;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "RandomizeActorTransform")
	bool bRandomizeScale = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "RandomizeActorTransform",meta = (EditCondition = "bRandomizeScale"))
	float ScaleMin = .8f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "RandomizeActorTransform",meta = (EditCondition = "bRandomizeScale"))
	float ScaleMax = 1.2f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "RandomizeActorTransform")
	bool bRandomizeOffset = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "RandomizeActorTransform",meta = (EditCondition = "bRandomizeOffset"))
	float OffsetMin = -50.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "RandomizeActorTransform",meta = (EditCondition = "bRandomizeOffset"))
	float OffsetMax = 50.f;

#pragma endregion

private: 

	UPROPERTY()
	class UEditorActorSubsystem* EditorActorSubsystem;

	bool GetEditorActorSubsystem();
};
