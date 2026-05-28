// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopTypes.h"
#include "Project_GemCoopGemInvenRowWidget.generated.h"

class UTextBlock;
class UButton;
class UProject_GemCoopLobbyWidget;
/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopGemInvenRowWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_GemInfo;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_EquipQ;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_EquipW;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_EquipE;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GemInventory")
	FName GemID = NAME_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GemInventory")
	EGemType GemType = EGemType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GemInventory")
	EGemGrade Grade = EGemGrade::Common;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GemInventory")
	int32 Count = 0;

	UPROPERTY()
	UProject_GemCoopLobbyWidget* OwnerLobbyWidget = nullptr;

public:
	UFUNCTION(BlueprintCallable, Category = "GemInventory")
	void SetupRow(UProject_GemCoopLobbyWidget* InOwner, const FName& InGemID, EGemType InGemType, EGemGrade InGrade, int32 InCount);

protected:
	UFUNCTION()
	void OnClickedEquipQ();

	UFUNCTION()
	void OnClickedEquipW();

	UFUNCTION()
	void OnClickedEquipE();

private:
	FString GemTypeToString(EGemType InGemType) const;
	FString GemGradeToString(EGemGrade InGrade) const;
	void RefreshText();
};
