//
// Copyright (c) 2021 suzumushi
//
// 2021-9-18		SOHVLines.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOHVLines.h"

#include "vstgui4/vstgui/lib/cdrawcontext.h"

namespace VSTGUI {

// Horizontal and vertical lines drawer for XYPad

SOHVLines:: SOHVLines (const CRect& size): CParamDisplay (size)
{
	CParamDisplay:: setMax (2.0);
}

void SOHVLines:: setValue (float val)
{
	if (zerone)
		zerone = 0;		// false
	else
		zerone = 1;		// true
	CParamDisplay:: setValue (val);
	if (xyval [zerone] != getValue ()) {
		calculateXY (xyval [zerone] = getValue (), x [zerone], y [zerone]);
		x [zerone] *= getWidth ();
		y [zerone] *= getHeight ();
	}
}

void SOHVLines:: draw (CDrawContext* context)
{
		drawBack (context);

		CPoint viewPos = getViewSize ().getTopLeft ();
		CDrawContext::Transform t (*context, CGraphicsTransform ().translate (viewPos));

		context -> drawLine (CPoint (x [0], y [0]), CPoint (x [1], y [0]));
		context -> drawLine (CPoint (x [1], y [0]), CPoint (x [1], y [1]));
		context -> drawLine (CPoint (x [0], y [1]), CPoint (x [1], y [1]));
		context -> drawLine (CPoint (x [0], y [0]), CPoint (x [0], y [1]));

		setDirty (false);
}

void SOHVLines:: calculateXY (float value, CCoord& x, CCoord& y)
{
	x = std::floor (value * 1'000.0 + 0.5) * 0.001;
	y = std::floor ((value - x) * 10'000'000.0 + 0.5) * 0.001;
}


} // namespace VSTGUI
