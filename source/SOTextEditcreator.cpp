//
// Copyright (c) 2023 suzumushi
//
// 2023-11-24		SOTextEditcreator.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/uiviewfactory.h"
#include "vstgui/uidescription/uiattributes.h"
#include "vstgui/uidescription/iviewcreator.h"

#include "SOTextEdit.h"

namespace VSTGUI {
namespace UIViewCreator {

// Creator for SOTextEdit

struct SOTextEditCreator: ViewCreatorAdapter
{
	SOTextEditCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "SOTextEdit";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCTextLabel;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "SO Text Edit";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new SOTextEdit (CRect (0, 0, 100, 20), nullptr, -1);
	}
};

SOTextEditCreator __gHVLinesCreator;


} // namespace UIViewCreator
} // namespace VSTGUI
