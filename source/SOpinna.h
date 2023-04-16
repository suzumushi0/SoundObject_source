//
// Copyright (c) 2021-2023 suzumushi
//
// 2023-4-14		SOpinna.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "SODDL.h"
#ifdef _MSC_VER			// Visual C++
#include <numbers>
using std::numbers::pi;
#endif


namespace suzumushi {

// pinna scattering effect filter

constexpr bool L_CH {true};
constexpr bool R_CH {false};

constexpr int IR_44_LEN {22};
constexpr int IR_48_LEN {24};
constexpr int IR_96_LEN {48};
constexpr int MAX_IR_LEN {IR_96_LEN};

template <typename TYPE, int AR>
struct IR_DATA {
	TYPE L_44 [360 / AR] [IR_44_LEN];
	TYPE R_44 [360 / AR] [IR_44_LEN];
	TYPE L_48 [360 / AR] [IR_48_LEN];
	TYPE R_48 [360 / AR] [IR_48_LEN];
	TYPE L_96 [360 / AR] [IR_96_LEN];
	TYPE R_96 [360 / AR] [IR_96_LEN];
	int angular_resolution {AR};
};

template <typename TYPE>
class SOpinna_scattering {
public:
	void setup_SR (const int SR, const bool CH);
	void setup (const TYPE theta_p, const int HRIR);
	TYPE process (const TYPE xn);
	void reset ();
private:
	const static IR_DATA <TYPE, 15> Y1_IR;	// IR data set of the KU 100 derived from Univ. of York, SADIE II
	const static IR_DATA <TYPE, 15> YK_IR;	// IR data set of the KEMAR derived from Univ. of York, SADIE II
	const static IR_DATA <TYPE, 3>  AK_IR;	// IR data set of the KEMAR derived from Aachen Univ., High-resolution HRTF
	const static int angular_resolution [(int32)HRIR_L::LIST_LEN];	// angular resolution table

	const TYPE* IR_TBL [(int32)HRIR_L::LIST_LEN];	// pointer to IR_TBL
	TYPE IR [MAX_IR_LEN];							// IR
	TYPE delta_IR [MAX_IR_LEN];						// for smoothing 
	int IR_LEN {0};									// IR length
	SODDL <TYPE, MAX_IR_LEN> IDL;					// input delay line
	TYPE prev_theta_p {0.0};						// for smoothing
	int prev_HRIR {0};								// for smoothing
	bool first_frame {true};						// The first frame after reset() or initialization
	bool smoothing {false};							// smoothing mode
};

template <typename TYPE>
const IR_DATA <TYPE, 15> SOpinna_scattering <TYPE>:: Y1_IR {
	{
	#include "York_KU100/IR_TBL_L_44.h"
	}, {
	#include "York_KU100/IR_TBL_R_44.h"
	}, {
	#include "York_KU100/IR_TBL_L_48.h"
	}, {
	#include "York_KU100/IR_TBL_R_48.h"
	}, {
	#include "York_KU100/IR_TBL_L_96.h"
	}, {
	#include "York_KU100/IR_TBL_R_96.h"
	}
};

template <typename TYPE>
const IR_DATA <TYPE, 15> SOpinna_scattering <TYPE>:: YK_IR {
	{
	#include "York_KEMAR/IR_TBL_L_44.h"
	}, {
	#include "York_KEMAR/IR_TBL_R_44.h"
	}, {
	#include "York_KEMAR/IR_TBL_L_48.h"
	}, {
	#include "York_KEMAR/IR_TBL_R_48.h"
	}, {
	#include "York_KEMAR/IR_TBL_L_96.h"
	}, {
	#include "York_KEMAR/IR_TBL_R_96.h"
	}
};

template <typename TYPE>
const IR_DATA <TYPE, 3> SOpinna_scattering <TYPE>:: AK_IR {
	{
	#include "Aachen_KEMAR/IR_TBL_L_44.h"
	}, {
	#include "Aachen_KEMAR/IR_TBL_R_44.h"
	}, {
	#include "Aachen_KEMAR/IR_TBL_L_48.h"
	}, {
	#include "Aachen_KEMAR/IR_TBL_R_48.h"
	}, {
	#include "Aachen_KEMAR/IR_TBL_L_96.h"
	}, {
	#include "Aachen_KEMAR/IR_TBL_R_96.h"
	}
};

template <typename TYPE>
const int SOpinna_scattering <TYPE>:: angular_resolution [(int32)HRIR_L::LIST_LEN] {
	Y1_IR.angular_resolution,
	YK_IR.angular_resolution,	
	AK_IR.angular_resolution
};

template <typename TYPE>
void SOpinna_scattering <TYPE>:: setup_SR (const int SR, const bool CH)
{
	if (SR <= 44'100) {
		if (CH == L_CH) {
			IR_TBL [(int32)HRIR_L::YORK_KU100]	 = &Y1_IR.L_44 [0][0];
			IR_TBL [(int32)HRIR_L::YORK_KEMAR]	 = &YK_IR.L_44 [0][0];
			IR_TBL [(int32)HRIR_L::AACHEN_KEMAR] = &AK_IR.L_44 [0][0];
		} else {
			IR_TBL [(int32)HRIR_L::YORK_KU100]	 = &Y1_IR.R_44 [0][0];
			IR_TBL [(int32)HRIR_L::YORK_KEMAR]   = &YK_IR.R_44 [0][0];
			IR_TBL [(int32)HRIR_L::AACHEN_KEMAR] = &AK_IR.R_44 [0][0];
		}
		IR_LEN = IR_44_LEN;
	} else if (SR == 48'000) {
		if (CH == L_CH) {
			IR_TBL [(int32)HRIR_L::YORK_KU100]	 = &Y1_IR.L_48 [0][0];
			IR_TBL [(int32)HRIR_L::YORK_KEMAR]	 = &YK_IR.L_48 [0][0];
			IR_TBL [(int32)HRIR_L::AACHEN_KEMAR] = &AK_IR.L_48 [0][0];
		} else {
			IR_TBL [(int32)HRIR_L::YORK_KU100]	 = &Y1_IR.R_48 [0][0];
			IR_TBL [(int32)HRIR_L::YORK_KEMAR]   = &YK_IR.R_48 [0][0];
			IR_TBL [(int32)HRIR_L::AACHEN_KEMAR] = &AK_IR.R_48 [0][0];
		}
		IR_LEN = IR_48_LEN;
	} else {
		if (CH == L_CH) {
			IR_TBL [(int32)HRIR_L::YORK_KU100]	 = &Y1_IR.L_96 [0][0];
			IR_TBL [(int32)HRIR_L::YORK_KEMAR]	 = &YK_IR.L_96 [0][0];
			IR_TBL [(int32)HRIR_L::AACHEN_KEMAR] = &AK_IR.L_96 [0][0];
		} else {
			IR_TBL [(int32)HRIR_L::YORK_KU100]	 = &Y1_IR.R_96 [0][0];
			IR_TBL [(int32)HRIR_L::YORK_KEMAR]   = &YK_IR.R_96 [0][0];
			IR_TBL [(int32)HRIR_L::AACHEN_KEMAR] = &AK_IR.R_96 [0][0];
		}
		IR_LEN = IR_96_LEN;
	}
}

template <typename TYPE>
void SOpinna_scattering <TYPE>:: setup (const TYPE theta_p, const int HRIR)
{
	if (first_frame || (theta_p == prev_theta_p && HRIR == prev_HRIR))
		smoothing = false;
	else
		smoothing = true;

	if (first_frame || smoothing) {
		prev_theta_p = theta_p;
		prev_HRIR = HRIR;
		TYPE rem = 180.0 * (1.0 - theta_p / pi) / angular_resolution [HRIR];
		int index = rem;	// floor
		rem -= index;

		if (index == 360 / angular_resolution [HRIR])
			index = 0;
		const TYPE* ir_tbl_l = IR_TBL [HRIR] + index * IR_LEN;

		if (++index == 360 / angular_resolution [HRIR])
			index = 0;
		const TYPE* ir_tbl_u = IR_TBL [HRIR] + index * IR_LEN;

		TYPE* ir = & IR [0];
		if (first_frame) {
			first_frame = false;
			for (int i = 0; i < IR_LEN; i++)
				*ir++ = *ir_tbl_l + (*ir_tbl_u++ - +*ir_tbl_l++) * rem;
		}
		if (smoothing) {
			TYPE* delta_ir = & delta_IR [0];
			for (int i = 0; i < IR_LEN; i++) {
				TYPE next_ir = *ir_tbl_l + (*ir_tbl_u++ - +*ir_tbl_l++) * rem;
				*delta_ir++ = (next_ir - *ir++) / frame_len;
			}
		}
	}
}

template <typename TYPE>
TYPE SOpinna_scattering <TYPE>:: process (const TYPE xn)
{
	// smoothing
	if (smoothing) {
		TYPE* ir = & IR [0];
		TYPE* delta_ir = & delta_IR [0];
		for (int i = 0; i < IR_LEN; i++)
			*ir++ += *delta_ir++;
	}

	// convolution
	IDL.enqueue (xn);
	TYPE* ir = & IR [0];
	TYPE sum = 0.0;
	for (int at = MAX_IR_LEN - 1; at >= MAX_IR_LEN - IR_LEN; at--)	// (at = MAX_IR_LEN -1) holds the latest data. 
		sum += *ir++ * IDL.read (at);
	return (sum);
}

template <typename TYPE>
void SOpinna_scattering <TYPE>:: reset ()
{
	IDL.reset();
	first_frame = true;
}


}	// namespace suzumushi
