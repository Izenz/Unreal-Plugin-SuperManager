// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

class UMaterialInstanceConstant;

UENUM(BlueprintType)
enum class E_ChannelPackingType : uint8
{
	ECPT_NoChannelPacking UMETA (DisplayName = "No channel Packing"),
	ECPT_ORM UMETA (DisplayName = "OcclusionRoughnessMetallic"),
	ECPT_MAX UMETA (DisplayName = "DefaultMAX")
};

UCLASS()
class SUPERMANAGER_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:

#pragma region QuickMatCreationCore

	UFUNCTION(BlueprintCallable, Category = "CreateMaterialFromTextures")
	void CreateMatFromTextures();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromTextures")
	E_ChannelPackingType ChannelPackingType = E_ChannelPackingType::ECPT_NoChannelPacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromTextures")
	bool bCustomMaterialName = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromTextures", meta = (EditCondition = "bCustomMaterialName"))
	FString MaterialName = TEXT("M_");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromTextures")
	bool bCreateMaterialInstance = false;

#pragma endregion

#pragma region SupportedTextureNames

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CreateMaterialFromTextures")
	TArray<FString> BaseColorArray = {
		TEXT("_BaseColor"),
		TEXT("_Albedo"),
		TEXT("_Diffuse"),
		TEXT("_diff")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> MetallicArray = {
		TEXT("_Metallic"),
		TEXT("_metal")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> RoughnessArray = {
		TEXT("_Roughness"),
		TEXT("_RoughnessMap"),
		TEXT("_rough")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> NormalArray = {
		TEXT("_Normal"),
		TEXT("_NormalMap"),
		TEXT("_nor")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> AmbientOcclusionArray = {
		TEXT("_AmbientOcclusion"),
		TEXT("_AmbientOcclusionMap"),
		TEXT("_AO")
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Supported Texture Names")
	TArray<FString> ORMArray = {
		TEXT("_arm"),
		TEXT("OcclusionRoughnessMetallic"),
		TEXT("_ORM")
	};

#pragma endregion

private:

#pragma region QuickMaterialCreation

	bool ProcessSelectedData(const TArray<FAssetData>& SelectedData,
		TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath);
	bool CheckIfNameIsUsed(const FString& FolderPath, const FString& NewName);
	UMaterial* CreateMaterialAsset(const FString& AssetName, const FString& AssetLocation);
	void Default_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& NumOfPinsConnected);
	void ORM_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& NumOfPinsConnected);

#pragma endregion

#pragma region CreateMaterialNodes

	bool TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial);
	bool TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial);

#pragma endregion

	UMaterialInstanceConstant* CreateMaterialInstanceAsset(UMaterial* BaseMaterial, FString& MatInstanceName, const FString& MatInstanceLocation);

};
