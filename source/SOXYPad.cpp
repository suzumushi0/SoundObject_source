//
// Copyright (c) 2023-2024 suzumushi
//
// 2024-3-10		SOXYPad.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "vstgui4/vstgui/lib/cdrawcontext.h"

#include "version.h"
#include "SOXYPad.h"
#include "SOconfig.h"


namespace VSTGUI {

// XYPad for SoundObject and SpeakerObjects

void SOXYPad:: draw (CDrawContext* context)
{
	CXYPad:: draw (context);

	CPoint viewPos = getViewSize ().getTopLeft ();
	CDrawContext:: Transform t (*context, CGraphicsTransform ().translate (viewPos));

	context -> setFrameColor (getFrameColor ());
	context -> drawLine (CPoint (x [0], y [0]), CPoint (x [1], y [0]));
	context -> drawLine (CPoint (x [1], y [0]), CPoint (x [1], y [1]));
	context -> drawLine (CPoint (x [0], y [1]), CPoint (x [1], y [1]));
	context -> drawLine (CPoint (x [0], y [0]), CPoint (x [0], y [1]));

	if (stringOriginalFilename	== "SpeakerObjects.vst3") {
		float mx, my;
		CXYPad:: calculateXY (getValue (), mx, my);
		mx = 1.0 - mx;

		CCoord width = getWidth () - getRoundRectRadius ();
		CCoord height = getHeight () - getRoundRectRadius ();
		CRect r (mx * width, my * height, mx * width, my * height);

		r.extend (getRoundRectRadius () / 2.0, getRoundRectRadius () / 2.0);
		r.offset (getViewSize ().left + getRoundRectRadius () / 2.0, getViewSize ().top + getRoundRectRadius () / 2.0);
		context -> setFillColor (getBackColor ());
		context -> setDrawMode (kAntiAliasing);
		context -> drawEllipse (r, kDrawFilled);
	}
}

void SOXYPad:: setX0 (CCoord val)
{
	x [0] = val * getWidth ();
}

void SOXYPad:: setX1 (CCoord val)
{
	x [1] = val * getWidth ();
}

void SOXYPad:: setY0 (CCoord val)
{
	y [0] = val * getHeight ();
}

void SOXYPad:: setY1 (CCoord val)
{
	y [1] = val * getHeight ();
}

ParamValue SOXYPad:: calculateValue (ParamValue x, ParamValue y)
{
	x = std::floor (x * 1'000.0 + 0.5) * 0.001;
	y = std::floor (y * 1'000.0 + 0.5) * 0.000'000'1;
	return (x + y);
}
void SOXYPad:: calculateXY (ParamValue value, ParamValue& x, ParamValue& y)
{
	x = std::floor (value * 1'000.0 + 0.5) * 0.001;
	y = std::floor ((value - x) * 10'000'000.0 + 0.5) * 0.001;
}

double SOXYPad:: norm_to_taper (double norm)
{
	double coord = 1.0 - 2.0 * norm;
	double ret = Steinberg::Vst::LogTaperParameter:: toPlain (abs (coord), 0.0, suzumushi::max_side_len / 2.0);
	return (coord >= 0.0 ? ret : - ret);
}

double SOXYPad:: taper_to_norm (double taper)
{
	double ret = Steinberg::Vst::LogTaperParameter:: toNormalized (abs (taper), 0.0, suzumushi::max_side_len / 2.0);
	if (taper < 0.0)
		ret = - ret;
	return ((1.0 - ret) / 2.0);
}

} // namespace VSTGUI
