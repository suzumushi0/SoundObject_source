//
// Copyright (c) 2021 suzumushi
//
// 2021-9-18		SOHVLines.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "vstgui4/vstgui/lib/controls/cparamdisplay.h"

namespace VSTGUI {

// Horizontal and vertical lines drawer for XYPad

class SOHVLines: public CParamDisplay
{
public:
	explicit SOHVLines (const CRect& size = CRect (0, 0, 0, 0));
	void setValue (float val) override;
	void draw (CDrawContext* context) override;

protected:
	int zerone {1};
	float xyval [2] {0.0, 0.0};
	CCoord x [2] {0.0, 0.0};
	CCoord y [2] {0.0, 0.0};

	void calculateXY (float value, CCoord& x, CCoord& y);
	void setMin (float val) override {}
	void setMax (float val) override {}
};

} // namespace VSTGUI
