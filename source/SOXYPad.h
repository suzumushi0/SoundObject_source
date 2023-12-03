//
// Copyright (c) 2023 suzumushi
//
// 2023-11-25		SOXYPad.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "vstgui4/vstgui/lib/controls/cxypad.h"

#include "SOconfig.h"
#include "SOparam.h"

namespace VSTGUI {

// XYPad for SoundObject

class SOXYPad: public CXYPad
{
public:
	explicit SOXYPad (const CRect& size = CRect (0, 0, 0, 0)): CXYPad (size) {} 
	void draw (CDrawContext* context) override;
	void setX0 (CCoord val);
	void setX1 (CCoord val);
	void setY0 (CCoord val);
	void setY1 (CCoord val);

	// SOXYPad helper functions
	static ParamValue calculateValue (ParamValue x, ParamValue y);
	static void calculateXY (ParamValue value, ParamValue& x, ParamValue& y);

	// 10% logarithmic taper curve (A curve variable resistor in Japan) helper functions for SOXYPad
	static double norm_to_taper (double norm);
	static double taper_to_norm (double taper);

protected:
	CCoord x [2] {0.0, 0.0};
	CCoord y [2] {0.0, 0.0};
};

} // namespace VSTGUI
