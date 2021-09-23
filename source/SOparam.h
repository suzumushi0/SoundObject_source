//
// Copyright (c) 2021 suzumushi
//
// 2021-9-23		SOparam.h
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

namespace suzumushi {

// GUI and host facing parameters 
	
constexpr int32		precision1 {1};		// precision of numerical parameters
constexpr int32		precision2 {2};		// precision of numerical parameters

// s_x: position of the acoustic source [m]
constexpr ParamValue s_x_min {- max_side_len / 2.0};
constexpr ParamValue s_x_max {max_side_len / 2.0};
constexpr ParamValue s_x_default {1.0};
constexpr int32		 s_x_step {0};			// continuous 
constexpr int32		 s_x_flags {ParameterInfo::kCanAutomate};

// s_y position of the acoustic source [m]
constexpr ParamValue s_y_min {- max_side_len / 2.0};
constexpr ParamValue s_y_max {max_side_len / 2.0};
constexpr ParamValue s_y_default {0.0};
constexpr int32		 s_y_step {0};			// continuous 
constexpr int32		 s_y_flags {ParameterInfo::kCanAutomate};

// s_z position of the acoustic source [m]
constexpr ParamValue s_z_min {- max_side_len / 2.0};
constexpr ParamValue s_z_max {max_side_len / 2.0};
constexpr ParamValue s_z_default {0.0};
constexpr int32		 s_z_step {0};			// continuous 
constexpr int32		 s_z_flags {ParameterInfo::kCanAutomate};

// r: radial distance to the acoustic source [m]
constexpr ParamValue r_min {0.0};
constexpr ParamValue r_max {max_side_len};
constexpr ParamValue r_default {1.0};
constexpr int32		 r_step {0};			// continuous 
constexpr int32		 r_flags {ParameterInfo::kCanAutomate};

// theta: elevation angle to the acoustic source [deg (-90..90)] (AES69-2020 based definition)
constexpr ParamValue theta_min {-90.0};
constexpr ParamValue theta_max {90.0};
constexpr ParamValue theta_default {0.0};
constexpr int32		 theta_step {0};		// continuous 
constexpr int32		 theta_flags {ParameterInfo::kCanAutomate};

// phi: azimuth angle to the acoustic source [deg (0..360)]
constexpr ParamValue phi_min {0.0};
constexpr ParamValue phi_max {360.0};	
constexpr ParamValue phi_default {0.0};
constexpr int32		 phi_step {0};			// continuous 
constexpr int32		 phi_flags {ParameterInfo::kCanAutomate};

// xyad: position of the xy Pad
constexpr ParamValue xypad_min {0.0};
constexpr ParamValue xypad_max {2.0};	
constexpr ParamValue xypad_default {0.0};
constexpr int32		 xypad_step {0};		// continuous 
constexpr int32		 xypad_flags {ParameterInfo::kNoFlags};

// yzpad: position of the yz Pad
constexpr ParamValue yzpad_min {0.0};
constexpr ParamValue yzpad_max {2.0};	
constexpr ParamValue yzpad_default {0.0};
constexpr int32		 yzpad_step {0};		// continuous 
constexpr int32		 yzpad_flags {ParameterInfo::kNoFlags};

// reflectance [dB]
constexpr ParamValue reflectance_min {-20.0};
constexpr ParamValue reflectance_max {20.0};
constexpr ParamValue reflectance_default {0.0};
constexpr int32		 reflectance_step {0};	// continuous 
constexpr int32		 reflectance_flags {ParameterInfo::kCanAutomate};

// fc: LPF cut-off frequency [KHz]
constexpr ParamValue fc_min {0.1};
constexpr ParamValue fc_max {22.0};
constexpr ParamValue fc_default {fc_max};
constexpr int32		 fc_step {0};	// continuous 
constexpr int32		 fc_flags {ParameterInfo::kCanAutomate};

// c: acoustic speed [m/s]
constexpr ParamValue c_min {300.0};
constexpr ParamValue c_max {400.0};
constexpr ParamValue c_default {331.5 + 0.61 * 22.0};
constexpr int32		 c_step {0};			// continuous 
constexpr int32		 c_flags {ParameterInfo::kNoFlags};

// a: radius of the sphere [mm]
constexpr ParamValue a_min {10.0};
constexpr ParamValue a_max {min_dist / 2.0 * 1'000};
constexpr ParamValue a_default {143.0 / 2.0};
constexpr int32		 a_step {0};			// continuous 
constexpr int32		 a_flags {ParameterInfo::kNoFlags};

// r_x: dimensions of the reverberation room, depth [m]
constexpr ParamValue r_x_min {min_dist * 2.0};
constexpr ParamValue r_x_max {max_side_len};
constexpr ParamValue r_x_default {2.9};
constexpr int32		 r_x_step {0};			// continuous 
constexpr int32		 r_x_flags {ParameterInfo::kNoFlags};

// r_y: dimensions of the reverberation room, width [m]
constexpr ParamValue r_y_min {min_dist * 2.0};
constexpr ParamValue r_y_max {max_side_len};
constexpr ParamValue r_y_default {4.1};
constexpr int32		 r_y_step {0};			// continuous 
constexpr int32		 r_y_flags {ParameterInfo::kNoFlags};

// r_z: dimensions of the reverberation room height [m]
constexpr ParamValue r_z_min {min_dist * 2.0};
constexpr ParamValue r_z_max {max_side_len};
constexpr ParamValue r_z_default {2.3};
constexpr int32		 r_z_step {0};			// continuous 
constexpr int32		 r_z_flags {ParameterInfo::kNoFlags};

// c_x: center of the sphere [m]
constexpr ParamValue c_x_min {min_dist};
constexpr ParamValue c_x_max {max_side_len / 2.0};
constexpr ParamValue c_x_default {1.8};
constexpr int32		 c_x_step {0};			// continuous 
constexpr int32		 c_x_flags {ParameterInfo::kNoFlags};

// c_y: center of the sphere [m]
constexpr ParamValue c_y_min {min_dist};
constexpr ParamValue c_y_max {max_side_len / 2.0};
constexpr ParamValue c_y_default {2.2};
constexpr int32		 c_y_step {0};			// continuous 
constexpr int32		 c_y_flags {ParameterInfo::kNoFlags};

// c_z: center of the sphere [m]
constexpr ParamValue c_z_min {min_dist};
constexpr ParamValue c_z_max {max_side_len / 2.0};
constexpr ParamValue c_z_default {0.8};
constexpr int32		 c_z_step {0};			// continuous 
constexpr int32		 c_z_flags {ParameterInfo::kNoFlags};

// Horizontal and vertical lines for XYPAD and YZPAD
constexpr ParamValue hv_min {0.0};
constexpr ParamValue hv_max {2.0};
constexpr ParamValue hv_default {0.0};
constexpr int32		 hv_step {0};			// continuous 
constexpr int32		 hv_flags {ParameterInfo::kNoFlags};

// hrir: HRIR DB selector
constexpr int32		hrir_flags {ParameterInfo::kIsList};
enum {
	YORK_KU100,
	YORK_KEMAR,
	AACHEN_KEMAR,
	HRIR_LIST_LEN
};

// output: output selector
constexpr int32		output_flags {ParameterInfo::kIsList};
enum {
	COMBINED_WAVES,
	DIRECT_WAVE,
	REFLECTED_WAVES,
	SCATTERED_WAVE,
	INCIDENT_WAVE,
	OUTPUT_LIST_LEN
};

// byass: bypass flag
constexpr ParamValue bypass_min {0.0};		// false
constexpr ParamValue bypass_max {1.0};		// true
constexpr ParamValue bypass_default {0.0};
constexpr int32		 bypass_step {1};		// toggle 
constexpr int32		 bypass_flags {ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass};

// definition of GUI and host facing parameters tag

enum {
	// real-time parameters
	S_X,					// position of the acoustic source,  x [m]
	S_Y,					//   y [m]
	S_Z,					//   z [m]
	R,						//   radius [m]
	THETA,					//   elevation [deg (-90..90)] (AES69-2020 based definition)
	PHI,					//   azimuth [deg (0..360)]
	XYPAD,					//   xy Pad
	YZPAD,					//   yz Pad
	REFLECTANCE,			// reflectance [dB]
	FC,						// LPF cut-off frequency [KHz]
	// non real-time parameters
	C,						// acoustic speed [m/s]
	A,						// radius of the sphere [mm]
	R_X,					// dimensions of the reverberation room, depth [m]
	R_Y,					//   width [m]
	R_Z,					//   height [m]
	C_X,					// center of the sphere, x [m]
	C_Y,					//   y [m]
	C_Z,					//   z [m]
	// control parameters
	HRIR,					// HRIR selector
	OUTPUT,					// output selector
	BYPASS,					// bypass flag
	// Horizontal and vertical lines for XYPAD and YZPAD
	HV_XY,					// xy Pad
	HV_YZ					// yz Pad
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
	int32 bypass;			// byass flag
	// flags and miscs
	bool first_frame;		// the first frame after dsp_reset()
	bool param_changed;		// GUI and host facing parameters are updated (real-time parameters only)
	bool r_theta_changed;	// r and/or theta are updated
	bool phi_changed;		// phi is updated
	bool xypad_changed;		// xy Pad is updated
	bool yzpad_changed;		// yz Pad is updated
	bool room_changed;		// r_XX and/or c_XX are updated
	int32 fb_counter;		// for parameters feedback of HVLines 

	GUI_param () {
		s_x = s_x_default;
		s_y = s_y_default;
		s_z = s_z_default;
		r = r_default;
		theta = theta_default;
		phi = phi_default;
		xypad = xypad_default;
		yzpad = yzpad_default;
		reflectance = reflectance_default;
		fc = fc_default;
		c = c_default;
		a = a_default;
		r_x = r_x_default;
		r_y = r_y_default;
		r_z = r_z_default;
		c_x = c_x_default;
		c_y = c_y_default;
		c_z = c_z_default;
		hrir = 0;
		output = 0;
		bypass = bypass_default;
		first_frame = true;
		param_changed = false;
		r_theta_changed = false;
		phi_changed = false;
		xypad_changed = false;
		yzpad_changed = false;
		room_changed = false;
		fb_counter = 0;
	}

	// for parameters feedback of HVLines 
	const int32 fb_counter_init {3};
	const int32 fb_counter_reaper {30};

	void room_update ()
	{
		room_changed = true;
		if (fb_counter < fb_counter_init)
			fb_counter = fb_counter_init;
	}

	void initial_room_update ()
	{
		room_changed = true;
		fb_counter = fb_counter_reaper;
	}
};

}
