//
// Copyright (c) 2021-2022 suzumushi
//
// 2022-1-1		version.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "pluginterfaces/base/fplatform.h"

// Here you can define the version of your plug-in: "Major.Sub.Release.Build""
#define MAJOR_VERSION_STR "2"
#define MAJOR_VERSION_INT 2

#define SUB_VERSION_STR "0"
#define SUB_VERSION_INT 0

#define RELEASE_NUMBER_STR "0"
#define RELEASE_NUMBER_INT 0

#define BUILD_NUMBER_STR "1" // Build number to be sure that each result could be identified.
#define BUILD_NUMBER_INT 1

// Version with build number (example "1.0.3.342")
#define FULL_VERSION_STR MAJOR_VERSION_STR "." SUB_VERSION_STR "." RELEASE_NUMBER_STR "." BUILD_NUMBER_STR

// Version without build number (example "1.0.3")
#define VERSION_STR MAJOR_VERSION_STR "." SUB_VERSION_STR "." RELEASE_NUMBER_STR

#define stringOriginalFilename	"SoundObject.vst3"
#if SMTG_PLATFORM_64
#define stringFileDescription	"SoundObject VST3 (64Bit)"
#else
#define stringFileDescription	"SoundObject VST3"
#endif
#define stringCompanyName		"suzumushi\0"
#define stringLegalCopyright	"Copyright(c) 2021-2022 suzumushi."
#define stringLegalTrademarks	"VST is a trademark of Steinberg Media Technologies GmbH"
