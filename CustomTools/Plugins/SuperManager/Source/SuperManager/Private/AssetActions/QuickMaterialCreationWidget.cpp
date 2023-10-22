// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"

#define NODE_HORIZONTAL_OFFSET 600
#define NODE_VERTICAL_OFFSET 240

#pragma region QuickMatCreationCore

void UQuickMaterialCreationWidget::CreateMatFromTextures()
{
	if (bCustomMaterialName)
	{
		if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
		{
			DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("Please enter a valid name."));
			return;
		}
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<UTexture2D*> SelectedTexturesArray;
	FString SelectedTextureFolderPath;
	uint32 NumOfPinsConected = 0;

	if (!ProcessSelectedData(SelectedAssetsData, SelectedTexturesArray, SelectedTextureFolderPath))
	{
		MaterialName = TEXT("M_");
		return;
	}
	if (CheckIfNameIsUsed(SelectedTextureFolderPath, MaterialName))
	{
		MaterialName = TEXT("M_");
		return;
	}

	UMaterial* NewMaterial = CreateMaterialAsset(MaterialName, SelectedTextureFolderPath);
	if (!NewMaterial)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("Failed to create material."));
		return;
	}

	for (UTexture2D* Texture : SelectedTexturesArray)
	{
		switch (ChannelPackingType)
		{
		case E_ChannelPackingType::ECPT_NoChannelPacking:
			Default_CreateMaterialNodes(NewMaterial, Texture, NumOfPinsConected);
			break;
		case E_ChannelPackingType::ECPT_ORM:
			ORM_CreateMaterialNodes(NewMaterial, Texture, NumOfPinsConected);
			break;
		case E_ChannelPackingType::ECPT_MAX:
			break;
		default:
			break;
		}
	}

	if (NumOfPinsConected > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully connected ") + FString::FromInt(NumOfPinsConected) + TEXT(" pins."));
	}

	if (bCreateMaterialInstance)
	{
		CreateMaterialInstanceAsset(NewMaterial, MaterialName, SelectedTextureFolderPath);
	}

	MaterialName = TEXT("M_");

}

#pragma endregion

#pragma region QuickMaterialCreation

bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedData,
	TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath)
{
	if (SelectedData.Num() == 0)
	{
		DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("No texture selected."));
		return false;
	}

	bool bMaterialNameAssigned = false;
	for (const FAssetData& Data : SelectedData)
	{
		UObject* SelectedAsset = Data.GetAsset();
		if (!SelectedAsset)	continue;

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);
		if (!SelectedTexture)
		{
			DebugHeader::ShowMessageDialog(EAppMsgType::Ok, TEXT("Please select only textures.") +
				SelectedAsset->GetName() + TEXT(" is not a texture."));

			return false;
		}

		OutSelectedTexturesArray.Add(SelectedTexture);
		if (OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = Data.PackagePath.ToString();
		}

		if (!bCustomMaterialName && !bMaterialNameAssigned)
		{
			MaterialName = SelectedTexture->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));

			bMaterialNameAssigned = true;
		}
	}

	return true;
}

bool UQuickMaterialCreationWidget::CheckIfNameIsUsed(const FString& FolderPath, const FString& NewName)
{
	TArray<FString> ExistingAssets = UEditorAssetLibrary::ListAssets(FolderPath, false);
	for (const FString& ExistingAsset : ExistingAssets)
	{
		const FString ExistingAssetName = FPaths::GetBaseFilename(ExistingAsset);
		if (ExistingAssetName.Equals(NewName))
		{
			DebugHeader::ShowMessageDialog(EAppMsgType::Ok, NewName +
				TEXT(" already exists."));

			return true;
		}
	}

	return false;
}

UMaterial* UQuickMaterialCreationWidget::CreateMaterialAsset(const FString& AssetName, const FString& AssetLocation)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();

	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(AssetName, AssetLocation, UMaterial::StaticClass(), MaterialFactory);
	return Cast<UMaterial>(NewAsset);
}

void UQuickMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& NumOfPinsConnected)
{
	UMaterialExpressionTextureSample* TextureSampleNode =
		NewObject<UMaterialExpressionTextureSample>(NewMaterial);

	if (!TextureSampleNode)
	{
		return;
	}

	if (!NewMaterial->BaseColor.IsConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	if (!NewMaterial->Metallic.IsConnected())
	{
		if (TryConnectMetallic(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	if (!NewMaterial->Roughness.IsConnected())
	{
		if (TryConnectRoughness(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	if (!NewMaterial->Normal.IsConnected())
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	if (!NewMaterial->AmbientOcclusion.IsConnected())
	{
		if (TryConnectAO(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	DebugHeader::Print(TEXT("Failed to connect the texture: ") + SelectedTexture->GetName(), FColor::Red);
}

void UQuickMaterialCreationWidget::ORM_CreateMaterialNodes(UMaterial* NewMaterial, UTexture2D* SelectedTexture, uint32& NumOfPinsConnected)
{
	UMaterialExpressionTextureSample* TextureSampleNode =
		NewObject<UMaterialExpressionTextureSample>(NewMaterial);

	if (!TextureSampleNode) return;

	if (!NewMaterial->BaseColor.IsConnected())
	{
		if (TryConnectBaseColor(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	if (!NewMaterial->Normal.IsConnected())
	{
		if (TryConnectNormal(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			++NumOfPinsConnected;
			return;
		}
	}

	if (!NewMaterial->Roughness.IsConnected())
	{
		if (TryConnectORM(TextureSampleNode, SelectedTexture, NewMaterial))
		{
			NumOfPinsConnected += 3;
			return;
		}
	}
}

#pragma endregion

#pragma region CreateMaterialNodes

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& BaseColorName : BaseColorArray)
	{
		if (SelectedTexture->GetName().Contains(BaseColorName))
		{
			// Connect pins to base color socket
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->MaterialExpressionEditorX -= NODE_HORIZONTAL_OFFSET;

			NewMaterial->Expressions.Add(TextureSampleNode);
			NewMaterial->BaseColor.Expression = TextureSampleNode;
			NewMaterial->PostEditChange();

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectMetallic(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& MetallicName : MetallicArray)
	{
		if (SelectedTexture->GetName().Contains(MetallicName))
		{
			// Following Unreal defaults
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			NewMaterial->Expressions.Add(TextureSampleNode);
			NewMaterial->Metallic.Expression = TextureSampleNode;
			NewMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= NODE_HORIZONTAL_OFFSET;
			TextureSampleNode->MaterialExpressionEditorY += NODE_VERTICAL_OFFSET;

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& RoughnessName : RoughnessArray)
	{
		if (SelectedTexture->GetName().Contains(RoughnessName))
		{
			// Following Unreal defaults
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			NewMaterial->Expressions.Add(TextureSampleNode);
			NewMaterial->Roughness.Expression = TextureSampleNode;
			NewMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= NODE_HORIZONTAL_OFFSET;
			TextureSampleNode->MaterialExpressionEditorY += NODE_VERTICAL_OFFSET * 2;

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& NormalName : NormalArray)
	{
		if (SelectedTexture->GetName().Contains(NormalName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Normal;

			NewMaterial->Expressions.Add(TextureSampleNode);
			NewMaterial->Normal.Expression = TextureSampleNode;
			NewMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= NODE_HORIZONTAL_OFFSET;
			TextureSampleNode->MaterialExpressionEditorY += NODE_VERTICAL_OFFSET * 3;

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& AOName : AmbientOcclusionArray)
	{
		if (SelectedTexture->GetName().Contains(AOName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			NewMaterial->Expressions.Add(TextureSampleNode);
			NewMaterial->AmbientOcclusion.Expression = TextureSampleNode;
			NewMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= NODE_HORIZONTAL_OFFSET;
			TextureSampleNode->MaterialExpressionEditorY += NODE_VERTICAL_OFFSET * 4;

			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode, UTexture2D* SelectedTexture, UMaterial* NewMaterial)
{
	for (const FString& ORMName : ORMArray)
	{
		if (SelectedTexture->GetName().Contains(ORMName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Masks;

			NewMaterial->Expressions.Add(TextureSampleNode);
			NewMaterial->AmbientOcclusion.Connect(1, TextureSampleNode);
			NewMaterial->Roughness.Connect(2, TextureSampleNode);
			NewMaterial->Metallic.Connect(3, TextureSampleNode);
			NewMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= NODE_HORIZONTAL_OFFSET;
			TextureSampleNode->MaterialExpressionEditorY += NODE_VERTICAL_OFFSET * 4;

			return true;
		}
	}
	return false;
}

#pragma endregion

UMaterialInstanceConstant* UQuickMaterialCreationWidget::CreateMaterialInstanceAsset(UMaterial* BaseMaterial,
	FString& MatInstanceName, const FString& MatInstanceLocation)
{
	MatInstanceName.RemoveFromStart(TEXT("M_"));
	MatInstanceName.InsertAt(0, TEXT("MI_"));

	UMaterialInstanceConstantFactoryNew* NewMaterialInstanceFactory = NewObject<UMaterialInstanceConstantFactoryNew>();

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UObject* NewAsset = AssetToolsModule.Get().CreateAsset(MatInstanceName, MatInstanceLocation,
		UMaterialInstanceConstant::StaticClass(), NewMaterialInstanceFactory);

	if (UMaterialInstanceConstant* NewMaterialInstance = Cast<UMaterialInstanceConstant>(NewAsset))
	{
		NewMaterialInstance->SetParentEditorOnly(BaseMaterial);

		NewMaterialInstance->PostEditChange();
		BaseMaterial->PostEditChange();

		return NewMaterialInstance;
	}


	return nullptr;
}