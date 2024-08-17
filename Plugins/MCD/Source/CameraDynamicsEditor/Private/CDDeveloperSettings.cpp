// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "CDDeveloperSettings.h"

UCDDeveloperSettings::UCDDeveloperSettings()
{
}

UCDDeveloperSettings* UCDDeveloperSettings::GetCameraDynamicsDeveloperSettings()
{
	return GetMutableDefault<UCDDeveloperSettings>();
}

void UCDDeveloperSettings::SetActiveDataAsset(const TSoftObjectPtr<UDataAsset> NewActiveAsset)
{
	ActiveDataAsset = NewActiveAsset;
	this->SaveConfig();
}

void UCDDeveloperSettings::SetDebugActive(bool bNewDebugActive)
{
	bIsDebugActive = bNewDebugActive;
	this->SaveConfig();
}
