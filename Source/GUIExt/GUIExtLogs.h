// Copyright (C) 2024 owoDra

#pragma once

#include "Logging/LogMacros.h"

GUIEXT_API DECLARE_LOG_CATEGORY_EXTERN(LogGUIE, Log, All);

#if !UE_BUILD_SHIPPING

#define GUIELOG(FormattedText, ...) UE_LOG(LogGUIE, Log, FormattedText, __VA_ARGS__)

#define GUIEENSURE(InExpression) ensure(InExpression)
#define GUIEENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GUIEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GUIECHECK(InExpression) check(InExpression)
#define GUIECHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GUIELOG(FormattedText, ...)

#define GUIEENSURE(InExpression) InExpression
#define GUIEENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GUIEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GUIECHECK(InExpression) InExpression
#define GUIECHECK_MSG(InExpression, InFormat, ...) InExpression

#endif