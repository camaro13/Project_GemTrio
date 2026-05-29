// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Project_GemCoopTypes.h"
#include "InputCoreTypes.h"
#include "Project_GemCoopSettingsWidget.generated.h"

class UButton;
class USlider;
class UTextBlock;
class UCheckBox;
class UComboBoxString;
class UWidget;
class UWidgetSwitcher;
class UInputKeySelector;
class UProject_GemCoopGameInstance;
/**
 * 
 */
UCLASS()
class PROJECT_GEMCOOP_API UProject_GemCoopSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	UWidgetSwitcher* WS_Settings;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Game;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Graphics;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Sound;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_Gameplay;

	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* Panel_KeyBinding;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_TabGame;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_TabGraphics;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_TabSound;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_TabGameplay;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_TabKeyBinding;

	// Game
	UPROPERTY(meta = (BindWidgetOptional))
	UComboBoxString* CMB_Language;

	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_HUDScale;

	UPROPERTY(meta = (BindWidgetOptional))
	UCheckBox* CB_ShowDamageNumbers;

	UPROPERTY(meta = (BindWidgetOptional))
	UCheckBox* CB_ColorBlindMode;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_HUDScaleValue;

	// Graphics
	UPROPERTY(meta = (BindWidgetOptional))
	UComboBoxString* CMB_Resolution;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBoxString* CMB_WindowMode;

	UPROPERTY(meta = (BindWidgetOptional))
	UComboBoxString* CMB_GraphicsQuality;

	UPROPERTY(meta = (BindWidgetOptional))
	UCheckBox* CB_VSync;

	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_FrameRateLimit;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_FrameRateLimitValue;

	// Sound
	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_MasterVolume;

	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_BGMVolume;

	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_SFXVolume;

	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_UIVolume;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_MasterVolumeValue;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_BGMVolumeValue;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_SFXVolumeValue;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_UIVolumeValue;

	// Gameplay
	UPROPERTY(meta = (BindWidgetOptional))
	USlider* Slider_MouseSensitivity;

	UPROPERTY(meta = (BindWidgetOptional))
	UCheckBox* CB_InvertMouse;

	UPROPERTY(meta = (BindWidgetOptional))
	UCheckBox* CB_CameraShake;

	UPROPERTY(meta = (BindWidgetOptional))
	UCheckBox* CB_AimAssist;

	UPROPERTY(meta = (BindWidgetOptional))
	UTextBlock* TXT_MouseSensitivityValue;

	// Key Binding
	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_MoveForward;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_MoveBackward;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_MoveLeft;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_MoveRight;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_BasicAttack;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_Dash;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_GemSlot1;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_GemSlot2;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_GemSlot3;

	UPROPERTY(meta = (BindWidgetOptional))
	UInputKeySelector* IKS_Ultimate;

	// Common
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_ApplySettings;

	UPROPERTY(meta = (BindWidgetOptional))
	UButton* BTN_ResetSettings;

private:
	UPROPERTY()
	FGemCoopUserSettings PendingSettings;

private:
	UProject_GemCoopGameInstance* GetGemCoopGameInstance() const;

	void InitializeOptions();
	void LoadCurrentSettings();
	void RefreshAllWidgets();
	void RefreshText();
	void SwitchToPanel(UWidget* TargetPanel);

	void SetKeySelectorValue(UInputKeySelector* Selector, FName ActionName);

protected:
	UFUNCTION()
	void OnClickedTabGame();

	UFUNCTION()
	void OnClickedTabGraphics();

	UFUNCTION()
	void OnClickedTabSound();

	UFUNCTION()
	void OnClickedTabGameplay();

	UFUNCTION()
	void OnClickedTabKeyBinding();

	UFUNCTION()
	void OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnHUDScaleChanged(float Value);

	UFUNCTION()
	void OnFrameRateLimitChanged(float Value);

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnBGMVolumeChanged(float Value);

	UFUNCTION()
	void OnSFXVolumeChanged(float Value);

	UFUNCTION()
	void OnUIVolumeChanged(float Value);

	UFUNCTION()
	void OnMouseSensitivityChanged(float Value);

	UFUNCTION()
	void OnShowDamageNumbersChanged(bool bIsChecked);

	UFUNCTION()
	void OnColorBlindModeChanged(bool bIsChecked);

	UFUNCTION()
	void OnVSyncChanged(bool bIsChecked);

	UFUNCTION()
	void OnInvertMouseChanged(bool bIsChecked);

	UFUNCTION()
	void OnCameraShakeChanged(bool bIsChecked);

	UFUNCTION()
	void OnAimAssistChanged(bool bIsChecked);

	UFUNCTION()
	void OnMoveForwardKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnMoveBackwardKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnMoveLeftKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnMoveRightKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnBasicAttackKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnDashKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnGemSlot1KeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnGemSlot2KeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnGemSlot3KeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnUltimateKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void OnClickedApplySettings();

	UFUNCTION()
	void OnClickedResetSettings();
};
