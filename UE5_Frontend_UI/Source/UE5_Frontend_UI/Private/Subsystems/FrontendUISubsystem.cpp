#include "Subsystems/FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "Widgets/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/Widget_ConfirmScreen.h"
#include "FrontendGameplayTags.h"
#include "FrontendFunctionLibrary.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

TObjectPtr<UFrontendUISubsystem> UFrontendUISubsystem::Get(const TObjectPtr<UObject> WorldContextObject)
{
	if (GEngine && WorldContextObject)
	{
		// Use LogWarning instead of Assert to avoid crash
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        
		if (World && World->GetGameInstance())
		{
			return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
		}
	}
    
	// Fallback: try to find ANY game world
	if (GEngine)
	{
		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE)
			{
				if (UWorld* World = Context.World())
				{
					return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
				}
			}
		}
	}
    
	return nullptr;
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);

		return FoundClasses.IsEmpty();
	}
	return false;
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback)
{
	check(!InSoftWidgetClass.IsNull());

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallback]()
			{
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
				check(LoadedWidgetClass && CreatedPrimaryLayout);

				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass,
					[AsyncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance)
					{
						AsyncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
					}
				);

				AsyncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		)
	);
}

void UFrontendUISubsystem::PushConfirmScreenToModelStackAsync(EConfirmScreenType InScreenType,
	const FText& InScreenTitle, const FText& InScreenMessage,
	TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
	UConfirmScreenInfoObject* CreatedInfoObject = nullptr;
	
	switch (InScreenType)
	{
	case EConfirmScreenType::Ok:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMessage);
		break;
	case EConfirmScreenType::YesNo:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMessage);
		break;
	case EConfirmScreenType::OkCancel:
		CreatedInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMessage);
		break;
	case EConfirmScreenType::Unknown:
		break;
	default:
		break;;
	}

	check(CreatedInfoObject);

	PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_ConfirmScreen),
		[CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget)
		{
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
			}
		}
	);
}

void UFrontendUISubsystem::BroadcastOptionValueChanged(UListDataObject_Base* ChangedOption,
	EOptionsListDataModifyReason ModifyReason)
{
	if (ChangedOption)
	{
		OnOptionValueChanged.Broadcast(
			ChangedOption->GetDataID(),
			ChangedOption->GetDataDisplayName(),
			ModifyReason
		);
	}
}

void UFrontendUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget)
{
	check(InCreatedWidget);
	CreatedPrimaryLayout = InCreatedWidget;
}