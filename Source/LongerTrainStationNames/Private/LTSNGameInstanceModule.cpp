#include "LTSNGameInstanceModule.h"

#include "LTSNLogMacros.h"

#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Toolkit/KismetBytecodeDisassembler.h"

constexpr auto NEW_TRAIN_STATION_NAME_LENGTH_LIMIT = 100;

void ULTSNGameInstanceModule::DispatchLifecycleEvent(ELifecyclePhase phase)
{
    switch (phase)
    {
    case ELifecyclePhase::INITIALIZATION:
        SetupMod();
        break;
    }

    Super::DispatchLifecycleEvent(phase);
}

void ULTSNGameInstanceModule::SetupMod()
{
    if (IsRunningDedicatedServer())
    {
        LTSN_LOG("ULTSNGameInstanceModule::SetupMod: This is a dedicated server, so there's nothing to do!");
        return;
    }

    // Ensure the widget class is loaded
    this->WidgetClass = Widget_TrainStationClass.LoadSynchronous();

    LTSN_LOG("ULTSNGameInstanceModule::SetupMod: Patching function!");

    // The length limit of 22 characters appears to be client-side hardcoded in the UI via 3 different places in ExecuteUbergraph_Widget_TrainStationNew.
    // We just directly edit the bytecode in the identified places to set the length limit to the new length limit.
    this->UberGraphFunction = this->WidgetClass->FindFunctionByName(TEXT("ExecuteUbergraph_Widget_TrainStationNew"));
    //DumpFunction(this->UberGraphFunction, TEXT("BeforeMod"))
    TArray<uint8>& originalCode = this->UberGraphFunction->Script;
    WriteInt(originalCode, 689, NEW_TRAIN_STATION_NAME_LENGTH_LIMIT);
    WriteInt(originalCode, 823, NEW_TRAIN_STATION_NAME_LENGTH_LIMIT);
    WriteInt(originalCode, 1305, NEW_TRAIN_STATION_NAME_LENGTH_LIMIT);
    //DumpFunction(this->UberGraphFunction, TEXT("AfterMod"));
}

void ULTSNGameInstanceModule::WriteInt(TArray<uint8>& code, int32 offset, int32 value)
{
    code[offset] = (uint8)(value & 0xFF);
    code[offset+1] = (uint8)((value >> 8) & 0xFF);
    code[offset+2] = (uint8)((value >> 16) & 0xFF);
    code[offset+3] = (uint8)((value >> 24) & 0xFF);
}

void ULTSNGameInstanceModule::DumpFunction(UFunction* Function, FString Name)
{
    const FString FileLocation = FPaths::RootDir() + FString::Printf(TEXT("BlueprintFunctionDump_%s_%s_%s.json"), *Name, *Function->GetOuter()->GetName(), *Function->GetName());

    FSMLKismetBytecodeDisassembler Disassembler;
    const TArray<TSharedPtr<FJsonValue>> Statements = Disassembler.SerializeFunction(Function);

    FString OutJsonString;
    const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutJsonString);
    FJsonSerializer::Serialize(Statements, Writer);

    FFileHelper::SaveStringToFile(OutJsonString, *FileLocation);
}
