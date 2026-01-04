#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendUISubsystem.generated.h"

class UFrontendCommonButtonBase;
class UListDataObject_Base;
struct FGameplayTag;
class UWidget_PrimaryLayout;
class UWidget_ActivatableBase;

enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush,
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnButtonDescriptionTextUpdatedDelegate,
	UFrontendCommonButtonBase*, BroadcastingButton,
	FText, DescriptionText	
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnOptionValueChangedDelegate,
	FName, OptionID,
	FText, OptionDisplayName,
	EOptionsListDataModifyReason, ModifyReason
);

UCLASS()
class UE5_FRONTEND_UI_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static TObjectPtr<UFrontendUISubsystem> Get(const TObjectPtr<UObject> WorldContextObject);
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback);
	void PushConfirmScreenToModelStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);
	
	void BroadcastOptionValueChanged(UListDataObject_Base* ChangedOption, EOptionsListDataModifyReason ModifyReason);
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedWidget);

	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Options Events")
	FOnOptionValueChangedDelegate OnOptionValueChanged;

private:
	UPROPERTY(Transient)
	TObjectPtr<UWidget_PrimaryLayout> CreatedPrimaryLayout;
};