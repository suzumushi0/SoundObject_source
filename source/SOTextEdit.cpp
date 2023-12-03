//
// Copyright (c) 2023 suzumushi
//
// 2023-11-25		SOTextEdit.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//


#include "SOTextEdit.h"
#include "SOvst3editor.h"

namespace VSTGUI {

// TextEdit for SoundObject

void SOTextEdit:: setText (const UTF8String& txt) 
{
	CTextEdit:: setText (txt);

	static_cast <SOVST3Editor*> (listener) -> SOvalueChanged (this);
}

} // namespace VSTGUI
