//
// Copyright (c) 2023 suzumushi
//
// 2023-11-22		SOvst3editor.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "vstgui4/vstgui/plugin-bindings/vst3editor.h"
#include "SOparam.h"
#include "SOXYPad.h"

namespace VSTGUI {

// VST3Editor for SoundObject

class SOVST3Editor: public VST3Editor
{
public:
	SOVST3Editor (Steinberg::Vst::EditController* controller, UTF8StringPtr templateName, UTF8StringPtr xmlFile):
		VST3Editor (controller, templateName, xmlFile) {}
	SOVST3Editor (UIDescription* desc, Steinberg::Vst::EditController* controller, UTF8StringPtr templateName, UTF8StringPtr xmlFile = nullptr):
		VST3Editor (desc, controller, templateName, xmlFile = nullptr) {}

	void SOvalueChanged (CControl* pControl);

protected:
	void PLUGIN_API close () override;
	virtual void controlTagWillChange (CControl* pControl) override;
	virtual void controlTagDidChange (CControl* pControl) override;
	CView* verifyView (CView* view, const UIAttributes& attributes, const IUIDescription* desc) override;
	void onViewRemoved (CFrame* frame, CView* view) override;

	SOXYPad* pXYpad = nullptr;
	SOXYPad* pYZpad = nullptr;
};

} // namespace VSTGUI
