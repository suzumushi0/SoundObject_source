//
// Copyright (c) 2021-2026 suzumushi
//
// 2026-1-28		SOparam.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
using Steinberg::Vst::ParameterInfo;
using Steinberg::Vst::ParamValue;
using Steinberg::int32;

#include "SOconfig.h"
#include "SOextparam.h"

namespace suzumushi {

// definition of GUI and host facing parameters tag

// real-time parameters
constexpr ParamID S_X {0};					// position of the acoustic source,  x [m]
constexpr ParamID S_Y {1};					//   y [m]
constexpr ParamID S_Z {2};					//   z [m]
constexpr ParamID R {3};					//   radius [m]
constexpr ParamID THETA {4};				//   elevation [deg (-90..90)] (AES69-2020 based definition)
constexpr ParamID PHI {5};					//   azimuth [deg (0..360)]
constexpr ParamID XYPAD {6};				//   xy Pad
constexpr ParamID YZPAD {7};				//   yz Pad
constexpr ParamID REFLECTANCE {8};			// reflectance [dB]
constexpr ParamID FC {9};					// LPF cut-off frequency [KHz]
// non real-time parameters
constexpr ParamID C {10};					// acoustic speed [m/s]
constexpr ParamID A {11};					// radius of the sphere [mm]
constexpr ParamID R_X {12};					// dimensions of the reverberation room, depth [m]
constexpr ParamID R_Y {13};					//   width [m]
constexpr ParamID R_Z {14};					//   height [m]
constexpr ParamID C_X {15};					// center of the sphere, x [m]
constexpr ParamID C_Y {16};					//   y [m]
constexpr ParamID C_Z {17};					//   z [m]
// control parameters
constexpr ParamID HRIR {18};				// HRIR selector
constexpr ParamID OUTPUT {19};				// output selector
constexpr ParamID BYPASS {20};				// bypass flag
constexpr ParamID FORMAT {21};				// output format
// real-time parameters
constexpr ParamID PHIL {22};				// azimuth angle to left speaker [deg (0..90)]
constexpr ParamID D_ATT {23};				// distance attenuation [dB] (when distance is doubled)


// attributes of GUI and host facing parameter

// s_x: position of the acoustic source [m]
constexpr struct rangeParameter s_x = {
	S_X,								// tag
	{- max_side_len / 2.0},				// min
	{max_side_len / 2.0},				// max
	{1.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};
	
// s_y position of the acoustic source [m]
constexpr struct rangeParameter s_y = {
	S_Y,								// tag
	{- max_side_len / 2.0},				// min
	{max_side_len / 2.0},				// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// s_z position of the acoustic source [m]
constexpr struct rangeParameter s_z = {
	S_Z,								// tag
	{- max_side_len / 2.0},				// min
	{max_side_len / 2.0},				// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// r: radial distance to the acoustic source [m]
constexpr struct rangeParameter r = {
	R,									// tag
	{0.0},								// min
	{max_side_len},						// max
	{1.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// theta: elevation angle to the acoustic source [deg (-90..90)] (AES69-2020 based definition)
constexpr struct rangeParameter theta = {
	THETA,								// tag
	{-90.0},							// min
	{90.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// phi: azimuth angle to the acoustic source [deg (0..360)]
constexpr struct rangeParameter phi = {
	PHI,								// tag
	{0.0},								// min
	{360.0},							// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// xypad: position of the xy Pad
constexpr struct rangeParameter xypad = {
	XYPAD,								// tag
	{0.0},								// min
	{2.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// yzpad: position of the yz Pad
constexpr struct rangeParameter yzpad = {
	YZPAD,								// tag
	{0.0},								// min
	{2.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// reflectance [dB]
constexpr struct rangeParameter reflectance = {
	REFLECTANCE,						// tag
	{-20.0},							// min
	{20.0},								// max
	{0.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// fc: LPF cut-off frequency [KHz]
constexpr struct infLogTaperParameter fc = {
	FC,									// tag
	{0.1},								// min
	{22.0},								// max
	{fc.max},							// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};

// phiL: azimuth angle to left speaker [deg (0..90)]	
constexpr struct rangeParameter phiL = {
	PHIL,								// tag
	{0.0},								// min
	{90.0},								// max
	{30.0},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// d_att: distance attenuation	[dB] (when distance is doubled)
constexpr struct rangeParameter d_att = {
	D_ATT,								// tag
	{-6.0},								// min
	{0.0},								// max
	{-6.0},								// default
	{0},								// continuous
	{ParameterInfo::kCanAutomate}		// flags
};


// c: acoustic speed [m/s]
constexpr struct rangeParameter c = {
	C,									// tag
	{300.0},							// min
	{400.0},							// max
	{331.5 + 0.61 * 22.0},				// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// a: radius of the sphere [mm]
constexpr struct rangeParameter a = {
	A,									// tag
	{10.0},								// min
	{min_dist / 2.0 * 1'000},			// max
	{143.0 / 2.0},						// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// r_x: dimensions of the reverberation room, depth [m]
constexpr struct rangeParameter r_x = {
	R_X,								// tag
	{min_dist * 2.0},					// min
	{max_side_len},						// max
	{2.9},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// r_y: dimensions of the reverberation room, width [m]
constexpr struct rangeParameter r_y = {
	R_Y,								// tag
	{min_dist * 2.0},					// min
	{max_side_len},						// max
	{4.1},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// r_z: dimensions of the reverberation room height [m]
constexpr struct rangeParameter r_z = {
	R_Z,								// tag
	{min_dist * 2.0},					// min
	{max_side_len},						// max
	{2.3},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// c_x: center of the sphere [m]
constexpr struct rangeParameter c_x = {
	C_X,								// tag
	{min_dist},							// min
	{max_side_len / 2.0},				// max
	{1.8},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// c_y: center of the sphere [m]
constexpr struct rangeParameter c_y = {
	C_Y,								// tag
	{min_dist},							// min
	{max_side_len / 2.0},				// max
	{2.2},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// c_z: center of the sphere [m]
constexpr struct rangeParameter c_z = {
	C_Z,								// tag
	{min_dist},							// min
	{max_side_len / 2.0},				// max
	{0.8},								// default
	{0},								// continuous
	{ParameterInfo::kNoFlags}			// flags
};

// hrir: HRIR DB selector
constexpr struct stringListParameter hrir = {
	HRIR,								// tag
	{ParameterInfo::kIsList}			// flags
};
enum class HRIR_L {
	YORK_KU100,
	YORK_KEMAR,
	AACHEN_KEMAR,
	ML_GHRIR,
	LIST_LEN
};

// output: output selector
constexpr struct stringListParameter output = {
	OUTPUT,								// tag
	{ParameterInfo::kIsList}			// flags
};
enum class OUTPUT_L {
	COMBINED_WAVES,
	DIRECT_WAVE,
	REFLECTED_WAVES,
	SCATTERED_WAVE,
	INCIDENT_WAVE,
	LIST_LEN
};

// format: output format selector
constexpr struct stringListParameter format = {
	FORMAT,								// tag
	{ParameterInfo::kIsList}			// flags
};
enum class FORMAT_L {
	BINAURAL,
	TRANSAURAL,
	LIST_LEN
};

// bypass: bypass flag
constexpr struct rangeParameter bypass = {
	BYPASS,								// tag
	{0.0},								// min, false
	{1.0},								// max, true
	{0.0},								// default
	{1},								// toggle
	{ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass}	// flags
};

//  GUI and host facing parameters in processor class

struct GUI_param {
	// real-time parameters
	ParamValue s_x;			// position of the acoustic source,  x [m]
	ParamValue s_y;			//   y [m]
	ParamValue s_z;			//   z [m]
	ParamValue r;			//   radius [m]
	ParamValue theta;		//   elevation [deg (-90..90)] (AES69-2020 based definition)
	ParamValue phi;			//   azimuth [deg (0..360)]
	ParamValue xypad;		//   xy Pad
	ParamValue yzpad;		//   yz Pad
	ParamValue reflectance;	// reflectance [dB]
	ParamValue fc;			// LPF cut-off frequency [KHz]
	ParamValue phiL;		// azimuth angle to left speaker [deg (0..90)]	
	ParamValue d_att;		// distance attenuation
	// non real-time parameters
	ParamValue c;			// acoustic speed [m/s]
	ParamValue a;			// radius of the sphere [mm]
	ParamValue r_x;			// dimensions of the reverberation room, depth [m]
	ParamValue r_y;			//   width [m]
	ParamValue r_z;			//   height [m]
	ParamValue c_x;			// center of the sphere, x [m]
	ParamValue c_y;			//   y [m]
	ParamValue c_z;			//   z [m]
	// control parameters
	int32 hrir;				// HRIR selector
	int32 output;			// output selector
	int32 format;			// output format
	int32 bypass;			// bypass flag
	// flags and misc
	bool reset;				// the first frame after reset()
	bool param_changed;		// GUI and host facing parameters are updated (real-time parameters only)
	bool r_theta_changed;	// r and/or theta are updated
	bool phi_changed;		// phi is updated
	bool xypad_changed;		// xy Pad is updated
	bool yzpad_changed;		// yz Pad is updated

	GUI_param () {
		s_x = suzumushi::s_x.def;
		s_y = suzumushi::s_y.def;
		s_z = suzumushi::s_z.def;
		r = suzumushi::r.def;
		theta = suzumushi::theta.def;
		phi = suzumushi::phi.def;
		xypad = suzumushi::xypad.def;
		yzpad = suzumushi::yzpad.def;
		reflectance = suzumushi::reflectance.def;
		fc = suzumushi::fc.def;
		phiL = suzumushi::phiL.def;
		d_att = suzumushi::d_att.def;
		c = suzumushi::c.def;
		a = suzumushi::a.def;
		r_x = suzumushi::r_x.def;
		r_y = suzumushi::r_y.def;
		r_z = suzumushi::r_z.def;
		c_x = suzumushi::c_x.def;
		c_y = suzumushi::c_y.def;
		c_z = suzumushi::c_z.def;
		hrir = (int32) HRIR_L:: ML_GHRIR;
		output = (int32) OUTPUT_L:: COMBINED_WAVES;
		format = (int32) FORMAT_L:: BINAURAL;
		bypass = suzumushi::bypass.def;
		reset = true;
		param_changed = false;
		r_theta_changed = false;
		phi_changed = false;
		xypad_changed = false;
		yzpad_changed = false;
	}
};

}
