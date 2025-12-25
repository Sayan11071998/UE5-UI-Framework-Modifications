#include "FrontendSettings/FrontendGameUserSettings.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

UFrontendGameUserSettings::UFrontendGameUserSettings()
	: OverallVolume(1.f), MusicVolume(1.f), SoundFXVolume(1.f), bAllowBackgroundAudio(false), bUseHDRAudioMode(false)
{
}

TObjectPtr<UFrontendGameUserSettings> UFrontendGameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());
	}

	return nullptr;
}

void UFrontendGameUserSettings::SetOverallVolume(float InVolume)
{
	UWorld* InAudioWorld = nullptr;
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	
	if (GEngine)
	{
		InAudioWorld = GEngine->GetCurrentPlayWorld();
	}
	
	if (!InAudioWorld || !FrontendDeveloperSettings)
	{
		OverallVolume = InVolume;
		return;
	}
	
	USoundClass* MasterSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->MasterSoundClass.TryLoad())
	{
		MasterSoundClass = CastChecked<USoundClass>(LoadedObject);
	}
	
	USoundMix* DefaultSoundMix = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}
	
	if (!MasterSoundClass || !DefaultSoundMix)
	{
		OverallVolume = InVolume;
		return;
	}
	
	OverallVolume = InVolume;
	
	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		MasterSoundClass,
		OverallVolume,
		1.f,
		0.2f
	);
	
	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetMusicVolume(float InVolume)
{
	UWorld* InAudioWorld = nullptr;
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	
	if (GEngine)
	{
		InAudioWorld = GEngine->GetCurrentPlayWorld();
	}
	
	if (!InAudioWorld || !FrontendDeveloperSettings)
	{
		MusicVolume = InVolume;
		return;
	}
	
	USoundClass* MusicSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->MusicSoundClass.TryLoad())
	{
		MusicSoundClass = CastChecked<USoundClass>(LoadedObject);
	}
	
	USoundMix* DefaultSoundMix = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}
	
	if (!MusicSoundClass || !DefaultSoundMix)
	{
		MusicVolume = InVolume;
		return;
	}
	
	MusicVolume = InVolume;
	
	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		MusicSoundClass,
		MusicVolume,
		1.f,
		0.2f
	);
	
	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetSoundFXVolume(float InVolume)
{
	UWorld* InAudioWorld = nullptr;
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	
	if (GEngine)
	{
		InAudioWorld = GEngine->GetCurrentPlayWorld();
	}
	
	if (!InAudioWorld || !FrontendDeveloperSettings)
	{
		SoundFXVolume = InVolume;
		return;
	}
	
	USoundClass* SoundFXSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->SoundFXSoundClass.TryLoad())
	{
		SoundFXSoundClass = CastChecked<USoundClass>(LoadedObject);
	}
	
	USoundMix* DefaultSoundMix = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}
	
	if (!SoundFXSoundClass || !DefaultSoundMix)
	{
		SoundFXVolume = InVolume;
		return;
	}
	
	SoundFXVolume = InVolume;
	
	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		SoundFXSoundClass,
		SoundFXVolume,
		1.f,
		0.2f
	);
	
	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetAllowBackgroundAudio(bool bIsAllowed)
{
	bAllowBackgroundAudio = bIsAllowed;
	// Implementation depends on your platform requirements
	// For most platforms, this would control whether audio continues when app loses focus
}

void UFrontendGameUserSettings::SetUseHDRAudioMode(bool bIsAllowed)
{
	bUseHDRAudioMode = bIsAllowed;
	// Implementation depends on whether you're using spatial audio or other HDR audio features
}

float UFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}

	return 0.0f;
}

void UFrontendGameUserSettings::SetCurrentDisplayGamma(float InNewGamma)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InNewGamma;
	}
}