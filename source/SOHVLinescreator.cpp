//
// Copyright (c) 2021 suzumushi
//
// 2021-9-18		SOHVLinescreator.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOHVLines.h"

#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/uiviewfactory.h"
#include "vstgui/uidescription/uiattributes.h"
#include "vstgui/uidescription/iviewcreator.h"


namespace VSTGUI {
namespace UIViewCreator {

// Creator for horizontal and vertical lines drawer

struct HVLinesCreator: ViewCreatorAdapter
{
	HVLinesCreator ()
	{
		UIViewFactory:: registerViewCreator (*this);
	}

	IdStringPtr getViewName () const
	{
		return "SOHVLines";
	}

	IdStringPtr  getBaseViewName () const
	{
		return kCParamDisplay;
	}

	UTF8StringPtr getDisplayName () const
	{
		return "HV Lines";
	}

	CView* create (const UIAttributes& attrib, const IUIDescription* desc) const
	{
		return new SOHVLines (CRect (0, 0, 60, 60));
	}
};

HVLinesCreator __gHVLinesCreator;


} // namespace UIViewCreator
} // namespace VSTGUI
