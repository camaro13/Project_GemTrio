// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopSaveGame.h"

void UProject_GemCoopSaveGame::AddGem(FName GemID, EGemGrade Grade)
{
}

bool UProject_GemCoopSaveGame::IsAchievementUnlocked(FName AchievementID) const
{
	return false;
}

void UProject_GemCoopSaveGame::UnlockAchievement(FName AchievementID)
{
}

void UProject_GemCoopSaveGame::UpdateHighScore(float SurvivalTime, int32 Wave)
{
}

int32 UProject_GemCoopSaveGame::GetGemCodexCompletionPercent() const
{
	return int32();
}

//void UProject_GemCoopSaveGame::AccumulateStats(FGameResult Result)
//{
//}
