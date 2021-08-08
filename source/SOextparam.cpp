//
// Copyright (c) 2021 suzumushi
//
// 2021-8-8		SOextparam.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOextparam.h"
#include "pluginterfaces/base/ustring.h"

namespace Steinberg{
namespace Vst {

LogTaperParameter:: LogTaperParameter (const TChar* title, ParamID tag, const TChar* units,
		ParamValue minPlain, ParamValue maxPlain, ParamValue defaultValuePlain, 
		int32 stepCount, int32 flags, UnitID unitID, const TChar* shortTitle):
	Parameter(title, tag, units, 0.0, 0, flags, unitID, shortTitle),
	minPlain {minPlain}, maxPlain {maxPlain}
{
	valueNormalized = toNormalized (defaultValuePlain);
}

void LogTaperParameter:: toString (ParamValue valueNormalized, String128 string) const
{
	UString wrapper (string, str16BufferSize (String128));
	if (! wrapper.printFloat (toPlain (valueNormalized), precision))
		string[0] = 0;
}

bool LogTaperParameter:: fromString (const TChar* string, ParamValue& valueNormalized) const
{
	UString wrapper (const_cast<TChar*> (string), tstrlen (string));
	if (wrapper.scanFloat (valueNormalized)) {
		valueNormalized = toNormalized (valueNormalized);
		return (true);
	}
	return (false);
}

ParamValue LogTaperParameter:: toPlain (ParamValue valueNormalized) const
{
	return (to_plain (valueNormalized, minPlain, maxPlain));
}

ParamValue LogTaperParameter:: toNormalized (ParamValue plainValue) const
{
	return (log ((plainValue - minPlain) / (maxPlain - minPlain) * 80.0 + 1.0) / log (81.0));
}


InfParameter:: InfParameter (const TChar* title, ParamID tag, const TChar* units,
		ParamValue minPlain, ParamValue maxPlain, ParamValue defaultValuePlain, 
		int32 stepCount, int32 flags, bool min_inf, bool max_inf,
		UnitID unitID, const TChar* shortTitle):
	RangeParameter (title, tag, units, minPlain, maxPlain, defaultValuePlain, stepCount, flags, unitID, shortTitle),
	min_Inf {min_inf}, max_Inf {max_inf} {}

void InfParameter:: toString (ParamValue valueNormalized, String128 string) const
{
	UString wrapper (string, str16BufferSize (String128));
	if (min_Inf && valueNormalized == 0.0)
		wrapper.assign (L"-Åá");
	else if (max_Inf && valueNormalized == 1.0)
		wrapper.assign (L"Åá");
	else if (! wrapper.printFloat (toPlain (valueNormalized), precision))
		string[0] = 0;
}


InfLogTaperParameter:: InfLogTaperParameter (const TChar* title, ParamID tag, const TChar* units,
		ParamValue minPlain, ParamValue maxPlain, ParamValue defaultValuePlain, 
		int32 stepCount, int32 flags, UnitID unitID, const TChar* shortTitle):
	LogTaperParameter (title, tag, units, minPlain, maxPlain, defaultValuePlain, stepCount, flags, unitID, shortTitle) {}

void InfLogTaperParameter:: toString (ParamValue valueNormalized, String128 string) const
{
	UString wrapper (string, str16BufferSize (String128));
	if (valueNormalized == 1.0)
		wrapper.assign (L"Åá");
	else if (! wrapper.printFloat (toPlain (valueNormalized), precision))
		string[0] = 0;
}

}
}