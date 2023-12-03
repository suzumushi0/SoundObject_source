//
// Copyright (c) 2023 suzumushi
//
// 2023-11-25		SOTextEdit.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "vstgui4/vstgui/lib/controls/ctextedit.h"


namespace VSTGUI {

// TextEdit for SoundObject

class SOTextEdit: public CTextEdit
{
public:
	SOTextEdit (const CRect& size, IControlListener* listener, int32_t tag, UTF8StringPtr txt = nullptr, CBitmap* background = nullptr, const int32_t style = 0):
		CTextEdit (size, listener, tag, txt, background, style) {};
	SOTextEdit (const CTextEdit& textEdit): CTextEdit (textEdit) {};

	void setText (const UTF8String& txt) override;
};

} // namespace VSTGUI
