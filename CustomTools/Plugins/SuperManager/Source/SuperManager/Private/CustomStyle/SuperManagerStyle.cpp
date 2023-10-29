// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/SuperManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StyleColors.h"

FName FSuperManagerStyle::StyleSetName = FName("SuperManagerStyle");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::SuperManagerSlateStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcons()
{
	if (!SuperManagerSlateStyleSet.IsValid())
	{
		SuperManagerSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*SuperManagerSlateStyleSet);
	}
}

void FSuperManagerStyle::ShutDown()
{
	if (SuperManagerSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*SuperManagerSlateStyleSet);
		SuperManagerSlateStyleSet.Reset();
	}
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName));

	const FString IconsPath = IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() / "Resources";
	const FVector2D Icon16x16(16.f, 16.f);

	CustomStyleSet->SetContentRoot(IconsPath);

	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets",
		new FSlateImageBrush(IconsPath / "DeleteUnusedAsset.png", Icon16x16));

	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
		new FSlateImageBrush(IconsPath / "DeleteEmptyFolders.png", Icon16x16));

	CustomStyleSet->Set("ContentBrowser.AdvanceDeletion",
		new FSlateImageBrush(IconsPath / "AdvanceDeletion.png", Icon16x16));

	CustomStyleSet->Set("LevelEditor.DisableSelection",
		new FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16));

	CustomStyleSet->Set("LevelEditor.EnableSelection",
		new FSlateImageBrush(IconsPath / "SelectionUnlock.png", Icon16x16));

	const FCheckBoxStyle SelectionLockToggleStyle = FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetPadding(FMargin(10.f))

		// Uncheck images
		.SetUncheckedImage(FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16, FStyleColors::White25))
		.SetUncheckedHoveredImage(FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16, FStyleColors::AccentBlue))
		.SetUncheckedPressedImage(FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16, FStyleColors::Foreground))

		//Check images
		.SetCheckedImage(FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16, FStyleColors::Foreground))
		.SetCheckedHoveredImage(FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16, FStyleColors::AccentBlack))
		.SetCheckedPressedImage(FSlateImageBrush(IconsPath / "SelectionLock.png", Icon16x16, FStyleColors::AccentGray));

	CustomStyleSet->Set("SceneOutliner.SelectionLock", SelectionLockToggleStyle);

	return CustomStyleSet;
}
