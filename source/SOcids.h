//
// Copyright (c) 2021 suzumushi
//
// 2021-8-8		SOcids.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace suzumushi {
//------------------------------------------------------------------------
static const Steinberg::FUID kSoundObjectProcessorUID (0x46570030, 0xEE935D34, 0x95AAEFCF, 0x49F7F2CF);
static const Steinberg::FUID kSoundObjectControllerUID (0x4F5A4E6B, 0x006A5AA5, 0xA713AE71, 0xCB32F8B7);

#define SoundObjectVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace suzumushi
