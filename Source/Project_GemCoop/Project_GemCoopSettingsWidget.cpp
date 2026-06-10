// Fill out your copyright notice in the Description page of Project Settings.


#include "Project_GemCoopSettingsWidget.h"
#include "Project_GemCoopGameInstance.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/WidgetSwitcher.h"
#include "Components/InputKeySelector.h"

void UProject_GemCoopSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeOptions();

	if (BTN_TabGame)
	{
		BTN_TabGame->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedTabGame);
	}

	if (BTN_TabGraphics)
	{
		BTN_TabGraphics->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedTabGraphics);
	}

	if (BTN_TabSound)
	{
		BTN_TabSound->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedTabSound);
	}

	if (BTN_TabGameplay)
	{
		BTN_TabGameplay->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedTabGameplay);
	}

	if (BTN_TabKeyBinding)
	{
		BTN_TabKeyBinding->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedTabKeyBinding);
	}

	if (CMB_Language)
	{
		CMB_Language->OnSelectionChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnLanguageChanged);
	}

	if (CMB_Resolution)
	{
		CMB_Resolution->OnSelectionChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnResolutionChanged);
	}

	if (CMB_WindowMode)
	{
		CMB_WindowMode->OnSelectionChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnWindowModeChanged);
	}

	if (CMB_GraphicsQuality)
	{
		CMB_GraphicsQuality->OnSelectionChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnGraphicsQualityChanged);
	}

	if (Slider_HUDScale)
	{
		Slider_HUDScale->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnHUDScaleChanged);
	}

	if (Slider_FrameRateLimit)
	{
		Slider_FrameRateLimit->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnFrameRateLimitChanged);
	}

	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnMasterVolumeChanged);
	}

	if (Slider_BGMVolume)
	{
		Slider_BGMVolume->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnBGMVolumeChanged);
	}

	if (Slider_SFXVolume)
	{
		Slider_SFXVolume->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnSFXVolumeChanged);
	}

	if (Slider_UIVolume)
	{
		Slider_UIVolume->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnUIVolumeChanged);
	}

	if (Slider_MouseSensitivity)
	{
		Slider_MouseSensitivity->OnValueChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnMouseSensitivityChanged);
	}

	if (CB_ShowDamageNumbers)
	{
		CB_ShowDamageNumbers->OnCheckStateChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnShowDamageNumbersChanged);
	}

	if (CB_ColorBlindMode)
	{
		CB_ColorBlindMode->OnCheckStateChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnColorBlindModeChanged);
	}

	if (CB_VSync)
	{
		CB_VSync->OnCheckStateChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnVSyncChanged);
	}

	if (CB_InvertMouse)
	{
		CB_InvertMouse->OnCheckStateChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnInvertMouseChanged);
	}

	if (CB_CameraShake)
	{
		CB_CameraShake->OnCheckStateChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnCameraShakeChanged);
	}

	if (CB_AimAssist)
	{
		CB_AimAssist->OnCheckStateChanged.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnAimAssistChanged);
	}

	if (IKS_MoveForward)
	{
		IKS_MoveForward->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnMoveForwardKeySelected);
	}

	if (IKS_MoveBackward)
	{
		IKS_MoveBackward->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnMoveBackwardKeySelected);
	}

	if (IKS_MoveLeft)
	{
		IKS_MoveLeft->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnMoveLeftKeySelected);
	}

	if (IKS_MoveRight)
	{
		IKS_MoveRight->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnMoveRightKeySelected);
	}

	if (IKS_BasicAttack)
	{
		IKS_BasicAttack->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnBasicAttackKeySelected);
	}

	if (IKS_Dash)
	{
		IKS_Dash->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnDashKeySelected);
	}

	if (IKS_GemSlot1)
	{
		IKS_GemSlot1->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnGemSlot1KeySelected);
	}

	if (IKS_GemSlot2)
	{
		IKS_GemSlot2->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnGemSlot2KeySelected);
	}

	if (IKS_GemSlot3)
	{
		IKS_GemSlot3->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnGemSlot3KeySelected);
	}

	if (IKS_Ultimate)
	{
		IKS_Ultimate->OnKeySelected.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnUltimateKeySelected);
	}

	if (BTN_ApplySettings)
	{
		BTN_ApplySettings->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedApplySettings);
	}

	if (BTN_ResetSettings)
	{
		BTN_ResetSettings->OnClicked.AddDynamic(this, &UProject_GemCoopSettingsWidget::OnClickedResetSettings);
	}
	
	LoadCurrentSettings();
	RefreshAllWidgets();
	SwitchToPanel(Panel_Game);
}

UProject_GemCoopGameInstance* UProject_GemCoopSettingsWidget::GetGemCoopGameInstance() const
{
	return GetWorld() ? Cast<UProject_GemCoopGameInstance>(GetWorld()->GetGameInstance()) : nullptr;
}

void UProject_GemCoopSettingsWidget::InitializeOptions()
{
	if (CMB_Language)
	{
		CMB_Language->ClearOptions();
		CMB_Language->AddOption(TEXT("Korean"));
		CMB_Language->AddOption(TEXT("English"));
	}

	if (CMB_Resolution)
	{
		CMB_Resolution->ClearOptions();
		CMB_Resolution->AddOption(TEXT("1280x720"));
		CMB_Resolution->AddOption(TEXT("1600x900"));
		CMB_Resolution->AddOption(TEXT("1920x1080"));
		CMB_Resolution->AddOption(TEXT("2560x1440"));
	}

	if (CMB_WindowMode)
	{
		CMB_WindowMode->ClearOptions();
		CMB_WindowMode->AddOption(TEXT("Windowed"));
		CMB_WindowMode->AddOption(TEXT("Windowed Fullscreen"));
		CMB_WindowMode->AddOption(TEXT("Fullscreen"));
	}

	if (CMB_GraphicsQuality)
	{
		CMB_GraphicsQuality->ClearOptions();
		CMB_GraphicsQuality->AddOption(TEXT("Low"));
		CMB_GraphicsQuality->AddOption(TEXT("Medium"));
		CMB_GraphicsQuality->AddOption(TEXT("High"));
		CMB_GraphicsQuality->AddOption(TEXT("Epic"));
	}
}

void UProject_GemCoopSettingsWidget::LoadCurrentSettings()
{
	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		PendingSettings = GI->GetUserSettings();
	}
	else
	{
		PendingSettings = FGemCoopUserSettings();
	}
}

void UProject_GemCoopSettingsWidget::RefreshAllWidgets()
{
	if (CMB_Language)
	{
		CMB_Language->SetSelectedOption(PendingSettings.Language);
	}

	if (CMB_Resolution)
	{
		const FString ResolutionString = FString::Printf(TEXT("%dx%d"), PendingSettings.ResolutionX, PendingSettings.ResolutionY);

		CMB_Resolution->SetSelectedOption(ResolutionString);
	}

	if (CMB_WindowMode)
	{
		FString WindowModeString = TEXT("Windowed");

		if (PendingSettings.WindowMode == EGemCoopWindowMode::Fullscreen)
		{
			WindowModeString = TEXT("Fullscreen");
		}

		else if (PendingSettings.WindowMode == EGemCoopWindowMode::WindowedFullscreen)
		{
			WindowModeString = TEXT("Windowed Fullscreen");
		}

		CMB_WindowMode->SetSelectedOption(WindowModeString);
	}

	if (CMB_GraphicsQuality)
	{
		FString QualityString = TEXT("High");

		switch (PendingSettings.GraphicsQuality)
		{
		case EGemCoopGraphicsQuality::Low:
			QualityString = TEXT("Low");
			break;

		case EGemCoopGraphicsQuality::Medium:
			QualityString = TEXT("Medium");
			break;

		case EGemCoopGraphicsQuality::High:
			QualityString = TEXT("High");
			break;

		case EGemCoopGraphicsQuality::Epic:
			QualityString = TEXT("Epic");
			break;

		default:
			break;
		}

		CMB_GraphicsQuality->SetSelectedOption(QualityString);
	}

	if (Slider_HUDScale)
	{
		const float Normalized = (PendingSettings.HUDScale - 0.5f) / 1.5f;
		Slider_HUDScale->SetValue(FMath::Clamp(Normalized, 0.0f, 1.f));
	}

	if (Slider_FrameRateLimit)
	{
		const float Normalized = (PendingSettings.FrameRateLimit - 30.f) / 210.f;
		Slider_FrameRateLimit->SetValue(FMath::Clamp(Normalized, 0.0f, 1.f));
	}

	if (Slider_MasterVolume)
	{
		Slider_MasterVolume->SetValue(PendingSettings.MasterVolume);
	}

	if (Slider_BGMVolume)
	{
		Slider_BGMVolume->SetValue(PendingSettings.BGMVolume);
	}

	if (Slider_SFXVolume)
	{
		Slider_SFXVolume->SetValue(PendingSettings.SFXVolume);
	}

	if (Slider_UIVolume)
	{
		Slider_UIVolume->SetValue(PendingSettings.UIVolume);
	}

	if (Slider_MouseSensitivity)
	{
		const float Normalized = (PendingSettings.MouseSensitivity - 0.1f) / 2.9f;
		Slider_MouseSensitivity->SetValue(FMath::Clamp(Normalized, 0.0f, 1.f));
	}

	if (CB_ShowDamageNumbers)
	{
		CB_ShowDamageNumbers->SetIsChecked(PendingSettings.bShowDamageNumbers);
	}

	if (CB_ColorBlindMode)
	{
		CB_ColorBlindMode->SetIsChecked(PendingSettings.bColorBlindMode);
	}

	if (CB_VSync)
	{
		CB_VSync->SetIsChecked(PendingSettings.bVSync);
	}

	if (CB_InvertMouse)
	{
		CB_InvertMouse->SetIsChecked(PendingSettings.bInvertMouse);
	}

	if (CB_CameraShake)
	{
		CB_CameraShake->SetIsChecked(PendingSettings.bCameraShake);
	}

	if (CB_AimAssist)
	{
		CB_AimAssist->SetIsChecked(PendingSettings.bAimAssist);
	}

	SetKeySelectorValue(IKS_MoveForward, TEXT("MoveForward"));
	SetKeySelectorValue(IKS_MoveBackward, TEXT("MoveBackward"));
	SetKeySelectorValue(IKS_MoveLeft, TEXT("MoveLeft"));
	SetKeySelectorValue(IKS_MoveRight, TEXT("MoveRight"));
	SetKeySelectorValue(IKS_BasicAttack, TEXT("BasicAttack"));
	SetKeySelectorValue(IKS_Dash, TEXT("Dash"));
	SetKeySelectorValue(IKS_GemSlot1, TEXT("GemSlot1"));
	SetKeySelectorValue(IKS_GemSlot2, TEXT("GemSlot2"));
	SetKeySelectorValue(IKS_GemSlot3, TEXT("GemSlot3"));
	SetKeySelectorValue(IKS_Ultimate, TEXT("Ultimate"));

	RefreshText();
}

void UProject_GemCoopSettingsWidget::SetKeySelectorValue(UInputKeySelector* Selector, FName ActionName)
{
	if (!Selector)
	{
		return;
	}

	if (FKey* FoundKey = PendingSettings.KeyBindings.Find(ActionName))
	{
		Selector->SetSelectedKey(FInputChord(*FoundKey));
	}
}

void UProject_GemCoopSettingsWidget::SwitchToPanel(UWidget* TargetPanel)
{
	if (WS_Settings && TargetPanel)
	{
		WS_Settings->SetActiveWidget(TargetPanel);
	}
}

void UProject_GemCoopSettingsWidget::OnClickedTabGame()
{
	SwitchToPanel(Panel_Game);
}

void UProject_GemCoopSettingsWidget::OnClickedTabGraphics()
{
	SwitchToPanel(Panel_Graphics);
}

void UProject_GemCoopSettingsWidget::OnClickedTabSound()
{
	SwitchToPanel(Panel_Sound);
}

void UProject_GemCoopSettingsWidget::OnClickedTabGameplay()
{
	SwitchToPanel(Panel_Gameplay);
}

void UProject_GemCoopSettingsWidget::OnClickedTabKeyBinding()
{
	SwitchToPanel(Panel_KeyBinding);
}

void UProject_GemCoopSettingsWidget::OnLanguageChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	PendingSettings.Language = SelectedItem;
}

void UProject_GemCoopSettingsWidget::OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	FString Left;
	FString Right;

	if (SelectedItem.Split(TEXT("x"), &Left, &Right))
	{
		PendingSettings.ResolutionX = FCString::Atoi(*Left);
		PendingSettings.ResolutionY = FCString::Atoi(*Right);
	}
}

void UProject_GemCoopSettingsWidget::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == TEXT("Fullscreen"))
	{
		PendingSettings.WindowMode = EGemCoopWindowMode::Fullscreen;
	}
	else if (SelectedItem == TEXT("Windowed Fullscreen"))
	{
		PendingSettings.WindowMode = EGemCoopWindowMode::WindowedFullscreen;
	}
	else
	{
		PendingSettings.WindowMode = EGemCoopWindowMode::Windowed;
	}
}

void UProject_GemCoopSettingsWidget::OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (SelectedItem == TEXT("Low"))
	{
		PendingSettings.GraphicsQuality = EGemCoopGraphicsQuality::Low;
	}

	else if (SelectedItem == TEXT("Medium"))
	{
		PendingSettings.GraphicsQuality = EGemCoopGraphicsQuality::Medium;
	}

	else if (SelectedItem == TEXT("Epic"))
	{
		PendingSettings.GraphicsQuality = EGemCoopGraphicsQuality::Epic;
	}

	else
	{
		PendingSettings.GraphicsQuality = EGemCoopGraphicsQuality::High;
	}
}

void UProject_GemCoopSettingsWidget::OnHUDScaleChanged(float Value)
{
	PendingSettings.HUDScale = FMath::Lerp(0.5f, 2.f, FMath::Clamp(Value, 0.0f, 1.f));
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnFrameRateLimitChanged(float Value)
{
	PendingSettings.FrameRateLimit = FMath::Lerp(30.0f, 240.0f, FMath::Clamp(Value, 0.0f, 1.0f));
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnMasterVolumeChanged(float Value)
{
	PendingSettings.MasterVolume = FMath::Clamp(Value, 0.0f, 1.0f);
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnBGMVolumeChanged(float Value)
{
	PendingSettings.BGMVolume = FMath::Clamp(Value, 0.0f, 1.0f);
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnSFXVolumeChanged(float Value)
{
	PendingSettings.SFXVolume = FMath::Clamp(Value, 0.0f, 1.0f);
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnUIVolumeChanged(float Value)
{
	PendingSettings.UIVolume = FMath::Clamp(Value, 0.0f, 1.0f);
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnMouseSensitivityChanged(float Value)
{
	PendingSettings.MouseSensitivity = FMath::Lerp(0.1f, 3.0f, FMath::Clamp(Value, 0.0f, 1.0f));
	RefreshText();
}

void UProject_GemCoopSettingsWidget::OnShowDamageNumbersChanged(bool bIsChecked)
{
	PendingSettings.bShowDamageNumbers = bIsChecked;
}

void UProject_GemCoopSettingsWidget::OnColorBlindModeChanged(bool bIsChecked)
{
	PendingSettings.bColorBlindMode = bIsChecked;
}

void UProject_GemCoopSettingsWidget::OnVSyncChanged(bool bIsChecked)
{
	PendingSettings.bVSync = bIsChecked;
}

void UProject_GemCoopSettingsWidget::OnInvertMouseChanged(bool bIsChecked)
{
	PendingSettings.bInvertMouse = bIsChecked;
}

void UProject_GemCoopSettingsWidget::OnCameraShakeChanged(bool bIsChecked)
{
	PendingSettings.bCameraShake = bIsChecked;
}

void UProject_GemCoopSettingsWidget::OnAimAssistChanged(bool bIsChecked)
{
	PendingSettings.bAimAssist = bIsChecked;
}

void UProject_GemCoopSettingsWidget::OnMoveForwardKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("MoveForward"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnMoveBackwardKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("MoveBackward"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnMoveLeftKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("MoveLeft"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnMoveRightKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("MoveRight"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnBasicAttackKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("BasicAttack"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnDashKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("Dash"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnGemSlot1KeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("GemSlot1"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnGemSlot2KeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("GemSlot2"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnGemSlot3KeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("GemSlot3"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnUltimateKeySelected(FInputChord SelectedKey)
{
	PendingSettings.KeyBindings.Add(TEXT("Ultimate"), SelectedKey.Key);
}

void UProject_GemCoopSettingsWidget::OnClickedApplySettings()
{
	if (UProject_GemCoopGameInstance* GI = GetGemCoopGameInstance())
	{
		GI->SetUserSettings(PendingSettings);
		GI->SaveGameToSlot();
		GI->DebugPrintUserSettings();
	}
}

void UProject_GemCoopSettingsWidget::OnClickedResetSettings()
{
	PendingSettings = FGemCoopUserSettings();

	PendingSettings.KeyBindings.Empty();
	PendingSettings.KeyBindings.Add(TEXT("MoveForward"), EKeys::W);
	PendingSettings.KeyBindings.Add(TEXT("MoveBackward"), EKeys::S);
	PendingSettings.KeyBindings.Add(TEXT("MoveLeft"), EKeys::A);
	PendingSettings.KeyBindings.Add(TEXT("MoveRight"), EKeys::D);
	PendingSettings.KeyBindings.Add(TEXT("BasicAttack"), EKeys::LeftMouseButton);
	PendingSettings.KeyBindings.Add(TEXT("Dash"), EKeys::LeftShift);
	PendingSettings.KeyBindings.Add(TEXT("GemSlot1"), EKeys::One);
	PendingSettings.KeyBindings.Add(TEXT("GemSlot2"), EKeys::Two);
	PendingSettings.KeyBindings.Add(TEXT("GemSlot3"), EKeys::Three);
	PendingSettings.KeyBindings.Add(TEXT("Ultimate"), EKeys::Four);

	RefreshAllWidgets();
}

void UProject_GemCoopSettingsWidget::RefreshText()
{
	if (TXT_HUDScaleValue)
	{
		TXT_HUDScaleValue->SetText(FText::FromString(
			FString::Printf(TEXT("HUD Scale: %.2f"), PendingSettings.HUDScale)
		));
	}

	if (TXT_FrameRateLimitValue)
	{
		TXT_FrameRateLimitValue->SetText(FText::FromString(
			FString::Printf(TEXT("FPS Limit: %.0f"), PendingSettings.FrameRateLimit)
		));
	}

	if (TXT_MasterVolumeValue)
	{
		TXT_MasterVolumeValue->SetText(FText::FromString(
			FString::Printf(TEXT("Master: %.0f%%"), PendingSettings.MasterVolume * 100.0f)
		));
	}

	if (TXT_BGMVolumeValue)
	{
		TXT_BGMVolumeValue->SetText(FText::FromString(
			FString::Printf(TEXT("BGM: %.0f%%"), PendingSettings.BGMVolume * 100.0f)
		));
	}

	if (TXT_SFXVolumeValue)
	{
		TXT_SFXVolumeValue->SetText(FText::FromString(
			FString::Printf(TEXT("SFX: %.0f%%"), PendingSettings.SFXVolume * 100.0f)
		));
	}

	if (TXT_UIVolumeValue)
	{
		TXT_UIVolumeValue->SetText(FText::FromString(
			FString::Printf(TEXT("UI: %.0f%%"), PendingSettings.UIVolume * 100.0f)
		));
	}

	if (TXT_MouseSensitivityValue)
	{
		TXT_MouseSensitivityValue->SetText(FText::FromString(
			FString::Printf(TEXT("Mouse Sensitivity: %.2f"), PendingSettings.MouseSensitivity)
		));
	}
}