//
// Copyright (c) 2021 suzumushi
//
// 2021-9-25		SOpinna.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "SODDL.h"
#include <numbers>
using std::numbers::pi;

namespace suzumushi {

// pinna scattering effect filter

constexpr bool L_CH {true};
constexpr bool R_CH {false};

constexpr int angular_resolution_3 {3};
constexpr int angular_resolution_15 {15};

constexpr int IR_44_LEN {22};
constexpr int IR_48_LEN {24};
constexpr int IR_96_LEN {48};
constexpr int IDL_LEN {IR_96_LEN};

template <typename TYPE>
class SOpinna_scattering {
public:
	void setSR (const int SR, const bool CH);
	void setup (const TYPE theta_p, const int HRIR);
	TYPE process (const TYPE xn);
	void reset ();
private:
	const static TYPE Y1_IR_TBL_L_44 [360 / angular_resolution_15] [IR_44_LEN];
	const static TYPE Y1_IR_TBL_R_44 [360 / angular_resolution_15] [IR_44_LEN];
	const static TYPE Y1_IR_TBL_L_48 [360 / angular_resolution_15] [IR_48_LEN];
	const static TYPE Y1_IR_TBL_R_48 [360 / angular_resolution_15] [IR_48_LEN];
	const static TYPE Y1_IR_TBL_L_96 [360 / angular_resolution_15] [IR_96_LEN];
	const static TYPE Y1_IR_TBL_R_96 [360 / angular_resolution_15] [IR_96_LEN];
	const static TYPE YK_IR_TBL_L_44 [360 / angular_resolution_15] [IR_44_LEN];
	const static TYPE YK_IR_TBL_R_44 [360 / angular_resolution_15] [IR_44_LEN];
	const static TYPE YK_IR_TBL_L_48 [360 / angular_resolution_15] [IR_48_LEN];
	const static TYPE YK_IR_TBL_R_48 [360 / angular_resolution_15] [IR_48_LEN];
	const static TYPE YK_IR_TBL_L_96 [360 / angular_resolution_15] [IR_96_LEN];
	const static TYPE YK_IR_TBL_R_96 [360 / angular_resolution_15] [IR_96_LEN];
	const static TYPE AK_IR_TBL_L_44 [360 / angular_resolution_3] [IR_44_LEN];
	const static TYPE AK_IR_TBL_R_44 [360 / angular_resolution_3] [IR_44_LEN];
	const static TYPE AK_IR_TBL_L_48 [360 / angular_resolution_3] [IR_48_LEN];
	const static TYPE AK_IR_TBL_R_48 [360 / angular_resolution_3] [IR_48_LEN];
	const static TYPE AK_IR_TBL_L_96 [360 / angular_resolution_3] [IR_96_LEN];
	const static TYPE AK_IR_TBL_R_96 [360 / angular_resolution_3] [IR_96_LEN];

	const TYPE* IR_TBL [HRIR_LIST_LEN];		// pointer to IR_TBL
	const TYPE* IR;							// pointer to IR
	int IR_LEN {0};							// IR length
	SODDL <TYPE, IDL_LEN> IDL;				// input delay line
	int prev_angle {0};						// for limiter
	bool first_frame {true};				// The first frame after reset() or initialization
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: Y1_IR_TBL_L_44 [360 / angular_resolution_15] [IR_44_LEN] {
#include "York_KU100/IR_TBL_L_44.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: Y1_IR_TBL_R_44 [360 / angular_resolution_15] [IR_44_LEN] {
#include "York_KU100/IR_TBL_R_44.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: Y1_IR_TBL_L_48 [360 / angular_resolution_15] [IR_48_LEN] {
#include "York_KU100/IR_TBL_L_48.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: Y1_IR_TBL_R_48 [360 / angular_resolution_15] [IR_48_LEN] {
#include "York_KU100/IR_TBL_R_48.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: Y1_IR_TBL_L_96 [360 / angular_resolution_15] [IR_96_LEN] {
#include "York_KU100/IR_TBL_L_96.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: Y1_IR_TBL_R_96 [360 / angular_resolution_15] [IR_96_LEN] {
#include "York_KU100/IR_TBL_R_96.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: YK_IR_TBL_L_44 [360 / angular_resolution_15] [IR_44_LEN] {
#include "York_KEMAR/IR_TBL_L_44.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: YK_IR_TBL_R_44 [360 / angular_resolution_15] [IR_44_LEN] {
#include "York_KEMAR/IR_TBL_R_44.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: YK_IR_TBL_L_48 [360 / angular_resolution_15] [IR_48_LEN] {
#include "York_KEMAR/IR_TBL_L_48.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: YK_IR_TBL_R_48 [360 / angular_resolution_15] [IR_48_LEN] {
#include "York_KEMAR/IR_TBL_R_48.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: YK_IR_TBL_L_96 [360 / angular_resolution_15] [IR_96_LEN] {
#include "York_KEMAR/IR_TBL_L_96.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: YK_IR_TBL_R_96 [360 / angular_resolution_15] [IR_96_LEN] {
#include "York_KEMAR/IR_TBL_R_96.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: AK_IR_TBL_L_44 [360 / angular_resolution_3] [IR_44_LEN] {
#include "Aachen_KEMAR/IR_TBL_L_44.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: AK_IR_TBL_R_44 [360 / angular_resolution_3] [IR_44_LEN] {
#include "Aachen_KEMAR/IR_TBL_R_44.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: AK_IR_TBL_L_48 [360 / angular_resolution_3] [IR_48_LEN] {
#include "Aachen_KEMAR/IR_TBL_L_48.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: AK_IR_TBL_R_48 [360 / angular_resolution_3] [IR_48_LEN] {
#include "Aachen_KEMAR/IR_TBL_R_48.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: AK_IR_TBL_L_96 [360 / angular_resolution_3] [IR_96_LEN] {
#include "Aachen_KEMAR/IR_TBL_L_96.h"
};

template <typename TYPE>
const TYPE SOpinna_scattering <TYPE>:: AK_IR_TBL_R_96 [360 / angular_resolution_3] [IR_96_LEN] {
#include "Aachen_KEMAR/IR_TBL_R_96.h"
};

template <typename TYPE>
void SOpinna_scattering <TYPE>:: setSR (const int SR, const bool CH)
{
	if (SR <= 44'100) {
		if (CH == L_CH) {
			IR_TBL [YORK_KU100]	  = &Y1_IR_TBL_L_44 [0][0];
			IR_TBL [YORK_KEMAR]	  = &YK_IR_TBL_L_44 [0][0];
			IR_TBL [AACHEN_KEMAR] = &AK_IR_TBL_L_44 [0][0];
		} else {
			IR_TBL [YORK_KU100]	  = &Y1_IR_TBL_R_44 [0][0];
			IR_TBL [YORK_KEMAR]   = &YK_IR_TBL_R_44 [0][0];
			IR_TBL [AACHEN_KEMAR] = &AK_IR_TBL_R_44 [0][0];
		}
		IR_LEN = IR_44_LEN;
	} else if (SR == 48'000) {
		if (CH == L_CH) {
			IR_TBL [YORK_KU100]	  = &Y1_IR_TBL_L_48 [0][0];
			IR_TBL [YORK_KEMAR]	  = &YK_IR_TBL_L_48 [0][0];
			IR_TBL [AACHEN_KEMAR] = &AK_IR_TBL_L_48 [0][0];
		} else {
			IR_TBL [YORK_KU100]	  = &Y1_IR_TBL_R_48 [0][0];
			IR_TBL [YORK_KEMAR]   = &YK_IR_TBL_R_48 [0][0];
			IR_TBL [AACHEN_KEMAR] = &AK_IR_TBL_R_48 [0][0];
		}
		IR_LEN = IR_48_LEN;
	} else {
		if (CH == L_CH) {
			IR_TBL [YORK_KU100]	  = &Y1_IR_TBL_L_96 [0][0];
			IR_TBL [YORK_KEMAR]	  = &YK_IR_TBL_L_96 [0][0];
			IR_TBL [AACHEN_KEMAR] = &AK_IR_TBL_L_96 [0][0];
		} else {
			IR_TBL [YORK_KU100]	  = &Y1_IR_TBL_R_96 [0][0];
			IR_TBL [YORK_KEMAR]   = &YK_IR_TBL_R_96 [0][0];
			IR_TBL [AACHEN_KEMAR] = &AK_IR_TBL_R_96 [0][0];
		}
		IR_LEN = IR_96_LEN;
	}
}

template <typename TYPE>
void SOpinna_scattering <TYPE>:: setup (const TYPE theta_p, const int HRIR)
{
	int angular_resolution;
	if (HRIR == YORK_KU100 || HRIR == YORK_KEMAR)
		angular_resolution = angular_resolution_15;
	else // AACHEN_KEMAR
		angular_resolution = angular_resolution_3;

	int angle = theta_p * 180.0 / pi + 0.5;
	if (angle <= -180)
		angle = 180;

	// smoothing
	if (first_frame) {
		first_frame = false;
	} else {
		int diff = angle - prev_angle;
		if (abs (diff) > 180) {
			if (diff > 0)
				diff -= 360;
			else
				diff += 360;
		}
		if (abs (diff) > angular_resolution_15) {
			if (diff > 0) {
				angle = prev_angle + angular_resolution_15;
				if (angle > 180)
					angle -= 360;
			} else {
				angle = prev_angle - angular_resolution_15;
				if (angle <= -180)
					angle += 360;
			}
		}
	}
	IR = IR_TBL [HRIR] + ((180 - angle) / angular_resolution) * IR_LEN;
	prev_angle = angle;
}

template <typename TYPE>
TYPE SOpinna_scattering <TYPE>:: process (const TYPE xn)
{
	IDL.push (xn);
	TYPE sum = 0.0;
	const TYPE* ir = IR;
	for (int at = IDL_LEN - 1; at >= IDL_LEN - IR_LEN; at--)	// (at = IDL_LEN -1) holds the latest data. 
		sum += *ir++ * IDL.read (at);
	return (sum);
}

template <typename TYPE>
void SOpinna_scattering <TYPE>:: reset ()
{
	IDL.reset ();
	first_frame = true;
}


}	// namespace suzumushi
