//
// Copyright (c) 2021-2023 suzumushi
//
// 2023-9-1		SOextparam.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include <cmath>

#include "public.sdk/source/vst/vsteditcontroller.h"
using Steinberg::Vst::ParameterInfo;
using Steinberg::Vst::ParamID;
using Steinberg::Vst::ParamValue;
using Steinberg::int32;
using Steinberg::int16;


namespace suzumushi {

// precision of numerical parameters

constexpr int32 precision0 {0};
constexpr int32 precision1 {1};
constexpr int32	precision2 {2};
constexpr int32	precision3 {3};

// templates for parameter definition

struct rangeParameter {
	ParamID tag;
	ParamValue min;			// min plain
	ParamValue max;			// max plain
	ParamValue def;			// default value plain
	int32 steps;			// step count
	int32 flags;
	// helper functions
	static ParamValue toPlain (const ParamValue normalized, const rangeParameter range)
	{
		return (normalized * (range.max - range.min) + range.min);
	}
	static ParamValue toNormalized (const ParamValue plain, const rangeParameter range)
	{
		return ((plain - range.min) / (range.max - range.min));
	}
	static ParamValue dB_to_ratio (const ParamValue dB)
	{
		return (std::pow (10.0, dB / 20.0));
	}
	static ParamValue ratio_to_dB (const ParamValue ratio)
	{
		return (20.0 * std::log10 (ratio));
	}
};

struct stringListParameter {
	ParamID tag;
	int32 flags;
	// helper functions
	static int32 toPlain (const ParamValue normalized, const int32 ListLength)
	{
		return ((int32)(normalized * (ListLength - 1) + 0.5));
	}
	static ParamValue toNormalized (const int32 plain, const int32 ListLength)
	{
		return ((ParamValue)(plain) / (ListLength - 1));
	}
};

struct logTaperParameter {
	ParamID tag;
	ParamValue min;			// min plain
	ParamValue max;			// max plain
	ParamValue def;			// default value plain
	int32 steps;			// step count
	int32 flags;
};

struct infLogTaperParameter: public logTaperParameter {};

struct infParameter: public rangeParameter {
	bool min_inf;
	bool max_inf;
};

} // namespace suzumushi

//
// Extended parameters 
//

namespace Steinberg {
namespace Vst {

// Parameter for 10% logarithmic taper potentiometer (A curve variable resistor in Japan) 

class LogTaperParameter: public Parameter
{
public:
	LogTaperParameter (const TChar* title, ParamID tag, const TChar* units = nullptr,
		ParamValue minPlain = 0.0, ParamValue maxPlain = 1.0, ParamValue defaultValuePlain = 0.0, 
		int32 stepCount = 0, int32 flags = ParameterInfo::kCanAutomate, UnitID unitID = kRootUnitId,
		const TChar* shortTitle = nullptr);

	/** Converts a normalized value to a string. */
	void toString (ParamValue _valueNormalized, String128 string) const SMTG_OVERRIDE;

	/** Converts a string to a normalized value. */
	bool fromString (const TChar* string, ParamValue& valueNormalized) const SMTG_OVERRIDE;

	/** Converts a normalized value to plain value. */
	ParamValue toPlain (ParamValue valueNormalized) const SMTG_OVERRIDE;
	// helper functions
	static ParamValue toPlain (ParamValue normalized, ParamValue minPlain, ParamValue maxPlain)
	{
		return ((std::pow (81.0, normalized) - 1.0) / 80.0 * (maxPlain - minPlain) + minPlain);
	}
	static ParamValue toPlain (const ParamValue normalized, const suzumushi::logTaperParameter range)
	{
		return (toPlain (normalized, range.min, range.max));
	}

	/** Converts a plain value to a normalized value. */
	ParamValue toNormalized (ParamValue plainValue) const SMTG_OVERRIDE;
	// helper functions
	static ParamValue toNormalized (ParamValue plainValue, ParamValue minPlain, ParamValue maxPlain)
	{
		return (std::log ((plainValue - minPlain) / (maxPlain - minPlain) * 80.0 + 1.0) / std::log (81.0));
	}
	static ParamValue toNormalized (const ParamValue plainValue, const suzumushi::logTaperParameter range)
	{
		return (toNormalized (plainValue, range.min, range.max));
	}

	OBJ_METHODS (LogTaperParameter, Parameter)

protected:
	LogTaperParameter ();

	ParamValue minPlain;
	ParamValue maxPlain;
};


// LogTaperParameter except that "inf" is displayed at the maximum value

class InfLogTaperParameter: public LogTaperParameter
{
public:

	InfLogTaperParameter (const TChar* title, ParamID tag, const TChar* units = nullptr,
		ParamValue minPlain = 0.0, ParamValue maxPlain = 1.0, ParamValue defaultValuePlain = 0.0, 
		int32 stepCount = 0, int32 flags = ParameterInfo::kCanAutomate,
		UnitID unitID = kRootUnitId, const TChar* shortTitle = nullptr);

	/** Converts a normalized value to a string. */
	void toString (ParamValue _valueNormalized, String128 string) const SMTG_OVERRIDE;

	OBJ_METHODS (InfLogTaperParameter, LogTaperParameter)
};


// RangeParameter except that "inf" and/or "-inf" are displayed at the maximum and/or minimum values

class InfParameter: public RangeParameter
{
public:

	InfParameter (const TChar* title, ParamID tag, const TChar* units = nullptr,
		ParamValue minPlain = 0.0, ParamValue maxPlain = 1.0, ParamValue defaultValuePlain = 0.0, 
		int32 stepCount = 0, int32 flags = ParameterInfo::kCanAutomate, bool min_inf = false, bool max_inf = false,
		UnitID unitID = kRootUnitId, const TChar* shortTitle = nullptr);

	/** Converts a normalized value to a string. */
	void toString (ParamValue _valueNormalized, String128 string) const SMTG_OVERRIDE;

	OBJ_METHODS (InfParameter, RangeParameter)

protected:
	bool min_Inf;
	bool max_Inf;
};


}
}
