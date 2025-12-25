#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Collection.generated.h"

UCLASS()
class UE5_FRONTEND_UI_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const;
	virtual bool HasAnyChildListData() const;

	void AddChildListData(UListDataObject_Base* InChildListData);
	
	// NEW: Methods to control tab enabled state
	void SetTabEnabled(bool bInEnabled) { bIsTabEnabled = bInEnabled; }
	bool IsTabEnabled() const { return bIsTabEnabled; }

private:
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ChildListDataArray;
	
	// NEW: Track if this tab should be enabled
	bool bIsTabEnabled = true;
};