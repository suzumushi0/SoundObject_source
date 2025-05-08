//
// Copyright (c) 2023-2025 suzumushi
//
// 2025-3-19		SOXYPadcreator.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/uiviewfactory.h"
#include "vstgui/uidescription/uiattributes.h"
#include "vstgui/uidescription/iviewcreator.h"

#include "SOXYPad.h"

namespace VSTGUI {
namespace UIViewCreator {

// Creator for SOXYPad

struct SOXYPadCreator: ViewCreatorAdapter
{
	SOXYPadCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "SOXYPad";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCParamDisplay;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "SO XY Pad";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new SOXYPad (CRect (0, 0, 60, 60));
	}
};

SOXYPadCreator __gSOXYPadCreator;


} // namespace UIViewCreator
} // namespace VSTGUI
