//
// Copyright (c) 2021-2022 suzumushi
//
// 2022-1-17		SOextparam.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

//
// Extended parameters 
//

#include <cmath>
#include "public.sdk/source/vst/vsteditcontroller.h"

namespace Steinberg{
namespace Vst {

// Paramter for 10% logarithmic taper potentiometer (A curve variable registor in Japan) 

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
	// helper function
	static ParamValue to_plain (ParamValue normalized, ParamValue min_param, ParamValue max_param);

	/** Converts a plain value to a normalized value. */
	ParamValue toNormalized (ParamValue plainValue) const SMTG_OVERRIDE;

	OBJ_METHODS (LogTaperParameter, Parameter)

protected:
	LogTaperParameter ();

	ParamValue minPlain;
	ParamValue maxPlain;
};

inline ParamValue LogTaperParameter:: to_plain (ParamValue normalized, ParamValue min_param, ParamValue max_param)
{
	return ((std::pow (81.0, normalized) - 1.0) / 80.0 * (max_param - min_param) + min_param);
}


// RangeParamter except that "inf" and/or "-inf" are indicated at the maximum and/or minimum values

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


// LogTaperParameter except that "inf" is indicated at the maximum value

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


}
}