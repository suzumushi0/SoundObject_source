//
// Copyright (c) 2021 suzumushi
//
// 2021-9-18		SOcontroller.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOcontroller.h"
#include "SOcids.h"
#include "vstgui/plugin-bindings/vst3editor.h"

// suzumushi:
#include "base/source/fstreamer.h"

using namespace Steinberg;

namespace suzumushi {

//------------------------------------------------------------------------
// SoundObjectController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: initialize (FUnknown* context)
{
	// Here the Plug-in will be instanciated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk) {
		return result;
	}

	// Here you could register some parameters

	// suzumushi: registration of GUI and host facing parameters 

	// s_x: position of the acoustic source [m]
	Vst::RangeParameter* s_x_param = new Vst::RangeParameter (
		STR16 ("Acoustic source: x"), S_X, STR16 ("m"),
		s_x_min, s_x_max, s_x_default, s_x_step, s_x_flags);
	s_x_param -> setPrecision (precision2);
	parameters.addParameter (s_x_param);

	// s_y: position of the acoustic source [m]
	Vst::RangeParameter* s_y_param = new Vst::RangeParameter (
		STR16 ("Acoustic source: y"), S_Y, STR16 ("m"),
		s_y_min, s_y_max, s_y_default, s_y_step, s_y_flags);
	s_y_param -> setPrecision (precision2);
	parameters.addParameter (s_y_param);

	// s_z: position of the acoustic source [m]
	Vst::RangeParameter* s_z_param = new Vst::RangeParameter (
		STR16 ("Acoustic source: z"), S_Z, STR16 ("m"),
		s_z_min, s_z_max, s_z_default, s_z_step, s_z_flags);
	s_z_param -> setPrecision (precision2);
	parameters.addParameter (s_z_param);

	// r: radial distance to the acoustic source [m]
	Vst::RangeParameter* r_param = new Vst::RangeParameter (
		STR16 ("Radial distance"), R, STR16 ("m"),
		r_min, r_max, r_default, r_step, r_flags);
	r_param -> setPrecision (precision2);
	parameters.addParameter (r_param);

	// theta: elevation to the acoustic source [deg (-90..90)] (AES69-2020 based definition)
	Vst::RangeParameter* theta_param = new Vst::RangeParameter (
		STR16 ("Elevation angle"), THETA, STR16 ("deg."),
		theta_min, theta_max, theta_default, theta_step, theta_flags);
	theta_param -> setPrecision (precision2);
	parameters.addParameter (theta_param);

	// phi: azimuth angle to the acoustic source [deg (0..360)]
	Vst::RangeParameter* phi_param = new Vst::RangeParameter (
		STR16 ("Azimuth angle"), PHI, STR16 ("deg."),
		phi_min, phi_max, phi_default, phi_step, phi_flags);
	phi_param -> setPrecision (precision2);
	parameters.addParameter (phi_param);

	// xypad:: xy Pad
	Vst::RangeParameter* xypad_param = new Vst::RangeParameter (
		STR16 ("xy Pad"), XYPAD, nullptr,
		xypad_min, xypad_max, xypad_default, xypad_step, xypad_flags);
	parameters.addParameter (xypad_param);

	// yzpad:: yz Pad
	Vst::RangeParameter* yzpad_param = new Vst::RangeParameter (
		STR16 ("yz Pad"), YZPAD, nullptr,
		yzpad_min, yzpad_max, yzpad_default, yzpad_step, yzpad_flags);
	parameters.addParameter (yzpad_param);

	// reflectance
	Vst::InfParameter* reflectance_param = new Vst::InfParameter (
		STR16 ("Reflectance"), REFLECTANCE, STR16 ("dB"),
		reflectance_min, reflectance_max, reflectance_default, reflectance_step, 
		reflectance_flags, true, false);
	reflectance_param -> setPrecision (precision1);
	parameters.addParameter (reflectance_param);

	// fc: LPF cut-off frequency [KHz]
	Vst::InfLogTaperParameter* fc_param = new Vst::InfLogTaperParameter (
		STR16 ("Cut-off frequency"), FC, STR16 ("KHz"),
		fc_min, fc_max, fc_default, fc_step, fc_flags);
	fc_param -> setPrecision (precision1);
	parameters.addParameter (fc_param);


	// c: acoustic speed [m/s]
	Vst::RangeParameter* c_param = new Vst::RangeParameter (
		STR16 ("Acoustic speed"), C, STR16 ("m/s"),
		c_min, c_max, c_default, c_step, c_flags);
	c_param -> setPrecision (precision1);
	parameters.addParameter (c_param);

	// a: radius of the sphere [mm]
	Vst::RangeParameter* a_param = new Vst::RangeParameter (
		STR16 ("Radius of sphere"), A, STR16 ("mm"),
		a_min, a_max, a_default, a_step, a_flags);
	a_param -> setPrecision (precision1);
	parameters.addParameter (a_param);

	// r_x: dimensions of the reverberation room, depth [m]
	Vst::RangeParameter* r_x_param = new Vst::RangeParameter (
		STR16 ("Room depth"), R_X, STR16 ("m"),
		r_x_min, r_x_max, r_x_default, r_x_step, r_x_flags);
	r_x_param -> setPrecision (precision2);
	parameters.addParameter (r_x_param);

	// r_y: dimensions of the reverberation room, width [m]
	Vst::RangeParameter* r_y_param = new Vst::RangeParameter (
		STR16 ("Room width"), R_Y, STR16 ("m"),
		r_y_min, r_y_max, r_y_default, r_y_step, r_y_flags);
	r_y_param -> setPrecision (precision2);
	parameters.addParameter (r_y_param);

	// r_z: dimensions of the reverberation room, height [m]
	Vst::RangeParameter* r_z_param = new Vst::RangeParameter (
		STR16 ("Room height"), R_Z, STR16 ("m"),
		r_z_min, r_z_max, r_z_default, r_z_step, r_z_flags);
	r_z_param -> setPrecision (precision2);
	parameters.addParameter (r_z_param);

	// c_x: center of the sphere [m]
	Vst::RangeParameter* c_x_param = new Vst::RangeParameter (
		STR16 ("Center of sphere: x"), C_X, STR16 ("m"),
		c_x_min, c_x_max, c_x_default, c_x_step, c_x_flags);
	c_x_param -> setPrecision (precision2);
	parameters.addParameter (c_x_param);

	// c_y: center of the sphere [m]
	Vst::RangeParameter* c_y_param = new Vst::RangeParameter (
		STR16 ("Center of sphere: y"), C_Y, STR16 ("m"),
		c_y_min, c_y_max, c_y_default, c_y_step, c_y_flags);
	c_y_param -> setPrecision (precision2);
	parameters.addParameter (c_y_param);

	// c_z: center of the sphere [m]
	Vst::RangeParameter* c_z_param = new Vst::RangeParameter (
		STR16 ("Center of sphere: z"), C_Z, STR16 ("m"),
		c_z_min, c_z_max, c_z_default, c_z_step, c_z_flags);
	c_z_param -> setPrecision (precision2);
	parameters.addParameter (c_z_param);


	// hrir: HRIR selector
	Vst::StringListParameter* hrir_param = new Vst::StringListParameter (
		STR16 ("HRIR"), HRIR, nullptr, hrir_flags);
	hrir_param -> appendString (STR16 ("Univ. of York, SADIE II KU 100 HRIR"));
	hrir_param -> appendString (STR16 ("Univ. of York, SADIE II KEMAR HRIR"));
	hrir_param -> appendString (STR16 ("Aachen Univ., High-resolution KEMAR HRIR"));
	parameters.addParameter (hrir_param);

	// output: output selector
	Vst::StringListParameter* output_param = new Vst::StringListParameter (
		STR16 ("Output"), OUTPUT, nullptr, output_flags);
	output_param -> appendString (STR16 ("Combined waves"));
	output_param -> appendString (STR16 ("Direct wave"));
	output_param -> appendString (STR16 ("Reflected waves"));
	output_param -> appendString (STR16 ("Scattered wave by sphere"));
	output_param -> appendString (STR16 ("Incident wave /w ITD and ILD"));
	parameters.addParameter (output_param);

	// byass: bypass flag
	Vst::RangeParameter* bypass_param = new Vst::RangeParameter (
		STR16 ("Bypass"), BYPASS, nullptr,
		bypass_min, bypass_max, bypass_default, bypass_step, bypass_flags);
	parameters.addParameter (bypass_param);

	// hv_xy: HVLines for xy Pad
	Vst::RangeParameter* hvxy_param = new Vst::RangeParameter (
		STR16 ("HVLines_xy"), HV_XY, nullptr,
		hv_min, hv_max, hv_default, hv_step, hv_flags);
	parameters.addParameter (hvxy_param);

	// hv_yz: HVLines for yz Pad
	Vst::RangeParameter* hvyz_param = new Vst::RangeParameter (
		STR16 ("HVLines_yz"), HV_YZ, nullptr,
		hv_min, hv_max, hv_default, hv_step, hv_flags);
	parameters.addParameter (hvyz_param);

	return (result);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: terminate ()
{
	// Here the Plug-in will be de-instanciated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return (kResultFalse);

	// suzumushi:
	ParamValue dtmp;
	int32 itmp;
	IBStreamer streamer (state, kLittleEndian);

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (S_X, plainParamToNormalized (S_X, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (S_Y, plainParamToNormalized (S_Y, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (S_Z, plainParamToNormalized (S_Z, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (R, plainParamToNormalized (R, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (THETA, plainParamToNormalized (THETA, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (PHI, plainParamToNormalized (PHI, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (XYPAD, plainParamToNormalized (XYPAD, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (YZPAD, plainParamToNormalized (YZPAD, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (REFLECTANCE, plainParamToNormalized (REFLECTANCE, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (FC, plainParamToNormalized (FC, dtmp));


	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (C, plainParamToNormalized (C, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (A, plainParamToNormalized (A, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (R_X, plainParamToNormalized (R_X, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (R_Y, plainParamToNormalized (R_Y, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (R_Z, plainParamToNormalized (R_Z, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (C_X, plainParamToNormalized (C_X, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (C_Y, plainParamToNormalized (C_Y, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (C_Z, plainParamToNormalized (C_Z, dtmp));


	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (HRIR, plainParamToNormalized (HRIR, static_cast <ParamValue> (itmp)));

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (OUTPUT, plainParamToNormalized (OUTPUT, static_cast <ParamValue> (itmp)));

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (BYPASS, plainParamToNormalized (BYPASS, static_cast <ParamValue> (itmp)));

	return (kResultOk);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API SoundObjectController:: createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor)) {
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "SoundObject.uidesc");
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: setParamNormalized (Vst::ParamID tag, Vst::ParamValue value)
{
	// called by host to update your parameters
	tresult result = EditControllerEx1::setParamNormalized (tag, value);
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
	// called by host to get a string for given normalized value of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectController:: getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
	// called by host to get a normalized value from a string representation of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
}

//------------------------------------------------------------------------
} // namespace suzumushi
