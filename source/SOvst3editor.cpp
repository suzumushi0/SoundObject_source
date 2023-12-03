//
// Copyright (c) 2023 suzumushi
//
// 2023-11-25		SOvst3editor.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOvst3editor.h"

namespace VSTGUI {

// VST3Editor for SoundObject

void SOVST3Editor:: SOvalueChanged (CControl* pControl)
{
	ParamValue r_x, r_y, r_z, c_x, c_y, c_z = 0.0;
	double n0, n1 = 0.0;
	switch (pControl -> getTag ()) {
		case suzumushi::r_x.tag:
		case suzumushi::c_x.tag:
			r_x = suzumushi::rangeParameter:: toPlain (controller -> getParamNormalized (suzumushi::r_x.tag), suzumushi::r_x);
			c_x = suzumushi::rangeParameter:: toPlain (controller -> getParamNormalized (suzumushi::c_x.tag), suzumushi::c_x);
			if (pXYpad) {
				pXYpad -> setY0 (SOXYPad::taper_to_norm (- c_x));
				pXYpad -> setY1 (SOXYPad::taper_to_norm (r_x - c_x));
				pXYpad -> setDirty ();
			}
			break;
		case suzumushi::r_y.tag:
		case suzumushi::c_y.tag:
			r_y = suzumushi::rangeParameter:: toPlain (controller -> getParamNormalized (suzumushi::r_y.tag), suzumushi::r_y);
			c_y = suzumushi::rangeParameter:: toPlain (controller -> getParamNormalized (suzumushi::c_y.tag), suzumushi::c_y);
			n0 = SOXYPad::taper_to_norm (- c_y);
			n1 = SOXYPad::taper_to_norm (r_y - c_y);
			if (pXYpad) {
				pXYpad -> setX0 (n0);
				pXYpad -> setX1 (n1);
				pXYpad -> setDirty ();
			}
			if (pYZpad) {
				pYZpad -> setX0 (n0);
				pYZpad -> setX1 (n1);
				pYZpad -> setDirty ();
			}
			break;
		case suzumushi::r_z.tag:
		case suzumushi::c_z.tag:
			r_z = suzumushi::rangeParameter:: toPlain (controller -> getParamNormalized (suzumushi::r_z.tag), suzumushi::r_z);
			c_z = suzumushi::rangeParameter:: toPlain (controller -> getParamNormalized (suzumushi::c_z.tag), suzumushi::c_z);
			if (pYZpad) {
				pYZpad -> setY0 (SOXYPad::taper_to_norm (- c_z));
				pYZpad -> setY1 (SOXYPad::taper_to_norm (r_z - c_z));
				pYZpad -> setDirty ();
			}
			break;
	}
}

void PLUGIN_API SOVST3Editor:: close ()
{
	pXYpad = pYZpad = nullptr;
	VST3Editor:: close ();
}
 
void SOVST3Editor:: controlTagWillChange (CControl* pControl)
{
	 int32_t tag = pControl -> getTag ();
	 if (tag != -1 && pControl -> getListener () == this) {
		if (tag == suzumushi::xypad.tag)
			pXYpad = nullptr;
		else if (tag == suzumushi::yzpad.tag)
			pYZpad = nullptr;
	 } 

	 VST3Editor:: controlTagWillChange (pControl);
}

 void SOVST3Editor:: controlTagDidChange (CControl* pControl)
{
	 int32_t tag = pControl -> getTag ();
	 if (tag != -1 && pControl -> getListener () == this) {
		if (tag == suzumushi::xypad.tag)
			pXYpad = static_cast <SOXYPad*> (pControl);
		else if (tag == suzumushi::yzpad.tag)
			pYZpad = static_cast <SOXYPad*> (pControl);
	 } 

	 VST3Editor:: controlTagDidChange (pControl);
}

CView* SOVST3Editor:: verifyView (CView* view, const UIAttributes& attributes, const IUIDescription* desc)
{
	auto* control = dynamic_cast <CControl*> (view);

	if (control) {
		int32_t tag = control -> getTag ();
		if (tag != -1 && control -> getListener () == this) {
			if (tag == suzumushi::xypad.tag)
				pXYpad = static_cast <SOXYPad*> (control);
			else if (tag == suzumushi::yzpad.tag)
				pYZpad = static_cast <SOXYPad*> (control);
		} 
	}

	return (VST3Editor:: verifyView (view, attributes, desc));
}

void SOVST3Editor:: onViewRemoved (CFrame* frame, CView* view)
{
	auto* control = dynamic_cast <CControl*> (view);
	
	if (control) {
		int32_t tag = control -> getTag ();
		if (tag != -1) {
			if (tag == suzumushi::xypad.tag)
				pXYpad = nullptr;
			else if (tag == suzumushi::yzpad.tag)
				pYZpad = nullptr;
		}
	}

	VST3Editor:: onViewRemoved (frame, view);
}

} // namespace VSTGUI
