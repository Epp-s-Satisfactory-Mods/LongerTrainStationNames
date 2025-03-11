#pragma once

#include "CoreMinimal.h"

#include "LTSNDebugSettings.h"
#include "LTSNLogCategory.h"

#if LTSN_DEBUGGING_ENABLED
#define LTSN_LOG(Format, ...)\
    UE_LOG(LogLTSN, Verbose, TEXT(Format), ##__VA_ARGS__)
#else
#define LTSN_LOG(Format, ...)
#endif
