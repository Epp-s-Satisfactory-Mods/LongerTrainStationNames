#pragma once

#include "CoreMinimal.h"
#include "FGInteractWidget.h"
#include "Module/GameInstanceModule.h"
#include "LTSNGameInstanceModule.generated.h"

UCLASS()
class LONGERTRAINSTATIONNAMES_API ULTSNGameInstanceModule : public UGameInstanceModule
{
    GENERATED_BODY()
public:
    void DispatchLifecycleEvent(ELifecyclePhase phase) override;
    void SetupMod();
    void WriteInt(TArray<uint8>& code, int32 offset, int32 value);
    void DumpFunction(UFunction* Function, FString Name);

    UPROPERTY(EditAnywhere, Category = "Longer Train Station Names Types")
    TSoftClassPtr<UFGInteractWidget> Widget_TrainStationClass;

    // Hold hard references to the edited class and function to prevent our edits from being undone by garbage collection
    UPROPERTY()
    UClass* WidgetClass;
    UPROPERTY()
    UFunction* UberGraphFunction;
};
