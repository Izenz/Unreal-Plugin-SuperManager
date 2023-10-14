#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

void Print(const FString& message, const FColor& color)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, color, message);
	}
}

void PrintLog(const FString& message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *message);
}

EAppReturnType::Type ShowMessageDialog(EAppMsgType::Type MsgType, const FString& Message,
	bool bShowMessageAsWarning = true)
{
	if (bShowMessageAsWarning)
	{
		FText MessageTitle = FText::FromString(TEXT("Warning"));
		return FMessageDialog::Open(MsgType, FText::FromString(Message), &MessageTitle);
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 7.0f;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}