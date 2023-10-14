#pragma once

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