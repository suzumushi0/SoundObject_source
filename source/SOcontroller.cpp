//
// Copyright (c) 2021-2024 suzumushi
//
// 2024-3-10		SOcontroller.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOcontroller.h"
#include "SOparam.h"
#include "SOvst3editor.h"
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
		STR16 ("Acoustic source: x"), s_x.tag, STR16 ("m"),
		s_x.min, s_x.max, s_x.def, s_x.steps, s_x.flags);
	s_x_param -> setPrecision (precision2);
	parameters.addParameter (s_x_param);

	// s_y: position of the acoustic source [m]
	Vst::RangeParameter* s_y_param = new Vst::RangeParameter (
		STR16 ("Acoustic source: y"), s_y.tag, STR16 ("m"),
		s_y.min, s_y.max, s_y.def, s_y.steps, s_y.flags);
	s_y_param -> setPrecision (precision2);
	parameters.addParameter (s_y_param);

	// s_z: position of the acoustic source [m]
	Vst::RangeParameter* s_z_param = new Vst::RangeParameter (
		STR16 ("Acoustic source: z"), s_z.tag, STR16 ("m"),
		s_z.min, s_z.max, s_z.def, s_z.steps, s_z.flags);
	s_z_param -> setPrecision (precision2);
	parameters.addParameter (s_z_param);

	// r: radial distance to the acoustic source [m]
	Vst::RangeParameter* r_param = new Vst::RangeParameter (
		STR16 ("Radial distance"), r.tag, STR16 ("m"),
		r.min, r.max, r.def, r.steps, r.flags);
	r_param -> setPrecision (precision2);
	parameters.addParameter (r_param);

	// theta: elevation to the acoustic source [deg (-90..90)] (AES69-2020 based definition)
	Vst::RangeParameter* theta_param = new Vst::RangeParameter (
		STR16 ("Elevation angle"), theta.tag, STR16 ("deg."),
		theta.min, theta.max, theta.def, theta.steps, theta.flags);
	theta_param -> setPrecision (precision1);
	parameters.addParameter (theta_param);

	// phi: azimuth angle to the acoustic source [deg (0..360)]
	Vst::RangeParameter* phi_param = new Vst::RangeParameter (
		STR16 ("Azimuth angle"), phi.tag, STR16 ("deg."),
		phi.min, phi.max, phi.def, phi.steps, phi.flags);
	phi_param -> setPrecision (precision1);
	parameters.addParameter (phi_param);

	// xypad:: xy Pad
	Vst::RangeParameter* xypad_param = new Vst::RangeParameter (
		STR16 ("xy Pad"), xypad.tag, nullptr,
		xypad.min, xypad.max, xypad.def, xypad.steps, xypad.flags);
	parameters.addParameter (xypad_param);

	// yzpad:: yz Pad
	Vst::RangeParameter* yzpad_param = new Vst::RangeParameter (
		STR16 ("yz Pad"), yzpad.tag, nullptr,
		yzpad.min, yzpad.max, yzpad.def, yzpad.steps, yzpad.flags);
	parameters.addParameter (yzpad_param);

	// reflectance
	Vst::RangeParameter* reflectance_param = new Vst::RangeParameter (
		STR16 ("Reflectance"), reflectance.tag, STR16 ("dB"),
		reflectance.min, reflectance.max, reflectance.def, reflectance.steps, reflectance.flags);
	reflectance_param -> setPrecision (precision1);
	parameters.addParameter (reflectance_param);

	// fc: LPF cut-off frequency [KHz]
	Vst::InfLogTaperParameter* fc_param = new Vst::InfLogTaperParameter (
		STR16 ("Cut-off frequency"), fc.tag, STR16 ("KHz"),
		fc.min, fc.max, fc.def, fc.steps, fc.flags);
	fc_param -> setPrecision (precision1);
	parameters.addParameter (fc_param);

	// phiL: azimuth angle to left speaker [deg (0..90)]	
	Vst::RangeParameter* phiL_param = new Vst::RangeParameter (
		STR16 ("Azimuth to left speaker"), phiL.tag, STR16 ("deg."),
		phiL.min, phiL.max, phiL.def, phiL.steps, phiL.flags);
	phiL_param -> setPrecision (precision1);
	parameters.addParameter (phiL_param);

	// d_att: distance attenuation
	Vst::RangeParameter* d_att_param = new Vst::RangeParameter (
		STR16 ("Distance attenuation"), d_att.tag, STR16 ("dB"),
		d_att.min, d_att.max, d_att.def, d_att.steps, d_att.flags);
	d_att_param -> setPrecision (precision1);
	parameters.addParameter (d_att_param);

	// c: acoustic speed [m/s]
	Vst::RangeParameter* c_param = new Vst::RangeParameter (
		STR16 ("Acoustic speed"), c.tag, STR16 ("m/s"),
		c.min, c.max, c.def, c.steps, c.flags);
	c_param -> setPrecision (precision1);
	parameters.addParameter (c_param);

	// a: radius of the sphere [mm]
	Vst::RangeParameter* a_param = new Vst::RangeParameter (
		STR16 ("Radius of sphere"), a.tag, STR16 ("mm"),
		a.min, a.max, a.def, a.steps, a.flags);
	a_param -> setPrecision (precision1);
	parameters.addParameter (a_param);

	// r_x: dimensions of the reverberation room, depth [m]
	Vst::RangeParameter* r_x_param = new Vst::RangeParameter (
		STR16 ("Room depth"), r_x.tag, STR16 ("m"),
		r_x.min, r_x.max, r_x.def, r_x.steps, r_x.flags);
	r_x_param -> setPrecision (precision2);
	parameters.addParameter (r_x_param);

	// r_y: dimensions of the reverberation room, width [m]
	Vst::RangeParameter* r_y_param = new Vst::RangeParameter (
		STR16 ("Room width"), r_y.tag, STR16 ("m"),
		r_y.min, r_y.max, r_y.def, r_y.steps, r_y.flags);
	r_y_param -> setPrecision (precision2);
	parameters.addParameter (r_y_param);

	// r_z: dimensions of the reverberation room, height [m]
	Vst::RangeParameter* r_z_param = new Vst::RangeParameter (
		STR16 ("Room height"), r_z.tag, STR16 ("m"),
		r_z.min, r_z.max, r_z.def, r_z.steps, r_z.flags);
	r_z_param -> setPrecision (precision2);
	parameters.addParameter (r_z_param);

	// c_x: center of the sphere [m]
	Vst::RangeParameter* c_x_param = new Vst::RangeParameter (
		STR16 ("Center of sphere: x"), c_x.tag, STR16 ("m"),
		c_x.min, c_x.max, c_x.def, c_x.steps, c_x.flags);
	c_x_param -> setPrecision (precision2);
	parameters.addParameter (c_x_param);

	// c_y: center of the sphere [m]
	Vst::RangeParameter* c_y_param = new Vst::RangeParameter (
		STR16 ("Center of sphere: y"), c_y.tag, STR16 ("m"),
		c_y.min, c_y.max, c_y.def, c_y.steps, c_y.flags);
	c_y_param -> setPrecision (precision2);
	parameters.addParameter (c_y_param);

	// c_z: center of the sphere [m]
	Vst::RangeParameter* c_z_param = new Vst::RangeParameter (
		STR16 ("Center of sphere: z"), c_z.tag, STR16 ("m"),
		c_z.min, c_z.max, c_z.def, c_z.steps, c_z.flags);
	c_z_param -> setPrecision (precision2);
	parameters.addParameter (c_z_param);


	// hrir: HRIR selector
	Vst::StringListParameter* hrir_param = new Vst::StringListParameter (
		STR16 ("HRIR"), hrir.tag, nullptr, hrir.flags);
	hrir_param -> appendString (STR16 ("Univ. of York, SADIE II KU 100 HRIR"));
	hrir_param -> appendString (STR16 ("Univ. of York, SADIE II KEMAR HRIR"));
	hrir_param -> appendString (STR16 ("Aachen Univ., High-resolution KEMAR HRIR"));
	parameters.addParameter (hrir_param);

	// output: output selector
	Vst::StringListParameter* output_param = new Vst::StringListParameter (
		STR16 ("Output"), output.tag, nullptr, output.flags);
	output_param -> appendString (STR16 ("Combined waves"));
	output_param -> appendString (STR16 ("Direct wave"));
	output_param -> appendString (STR16 ("Reflected waves"));
	output_param -> appendString (STR16 ("Scattered wave by sphere"));
	output_param -> appendString (STR16 ("Incident wave /w ITD and ILD"));
	parameters.addParameter (output_param);

	// format: output format selector
	Vst::StringListParameter* format_param = new Vst::StringListParameter (
		STR16 ("Format"), format.tag, nullptr, format.flags);
	format_param -> appendString (STR16 ("Binaural (Headphones)"));
	format_param -> appendString (STR16 ("Transaural (Speakers)"));
	parameters.addParameter (format_param);

	// bypass: bypass flag
	Vst::RangeParameter* bypass_param = new Vst::RangeParameter (
		STR16 ("Bypass"), bypass.tag, nullptr,
		bypass.min, bypass.max, bypass.def, bypass.steps, bypass.flags);
	parameters.addParameter (bypass_param);

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

	if (streamer.readDouble (dtmp) == true) {
		if (dtmp > max_side_len / 2.0) {					// new format
			int version;
			if (streamer.readInt32 (version) == false)		// read version
				return (kResultFalse);
			if (streamer.readDouble (dtmp) == false)		// read s_x
				return (kResultFalse);
		}
	} else
		return (kResultFalse);
	setParamNormalized (s_x.tag, plainParamToNormalized (s_x.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (s_y.tag, plainParamToNormalized (s_y.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (s_z.tag, plainParamToNormalized (s_z.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (r.tag, plainParamToNormalized (r.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (theta.tag, plainParamToNormalized (theta.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (phi.tag, plainParamToNormalized (phi.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (xypad.tag, plainParamToNormalized (xypad.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (yzpad.tag, plainParamToNormalized (yzpad.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (reflectance.tag, plainParamToNormalized (reflectance.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (fc.tag, plainParamToNormalized (fc.tag, dtmp));


	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (c.tag, plainParamToNormalized (c.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (a.tag, plainParamToNormalized (a.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (r_x.tag, plainParamToNormalized (r_x.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (r_y.tag, plainParamToNormalized (r_y.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (r_z.tag, plainParamToNormalized (r_z.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (c_x.tag, plainParamToNormalized (c_x.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (c_y.tag, plainParamToNormalized (c_y.tag, dtmp));

	if (streamer.readDouble (dtmp) == false)
		return (kResultFalse);
	setParamNormalized (c_z.tag, plainParamToNormalized (c_z.tag, dtmp));


	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (hrir.tag, plainParamToNormalized (hrir.tag, static_cast <ParamValue> (itmp)));

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (output.tag, plainParamToNormalized (output.tag, static_cast <ParamValue> (itmp)));

	if (streamer.readInt32 (itmp) == false)
		return (kResultFalse);
	setParamNormalized (bypass.tag, plainParamToNormalized (bypass.tag, static_cast <ParamValue> (itmp)));

	// for backward compatibility 
	if (streamer.readInt32 (itmp) == true)
		setParamNormalized (format.tag, plainParamToNormalized (format.tag, static_cast <ParamValue> (itmp)));
	else
		setParamNormalized (format.tag, plainParamToNormalized (format.tag, static_cast <ParamValue> ((int32) FORMAT_L:: BINAURAL)));

	if (streamer.readDouble (dtmp) == true)
		setParamNormalized (phiL.tag, plainParamToNormalized (phiL.tag, dtmp));
	else
		setParamNormalized (phiL.tag, plainParamToNormalized (phiL.tag, phiL.def));

	if (streamer.readDouble (dtmp) == true)
		setParamNormalized (d_att.tag, plainParamToNormalized (d_att.tag, dtmp));
	else
		setParamNormalized (d_att.tag, plainParamToNormalized (d_att.tag, d_att.def));

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
		auto* view = new VSTGUI::SOVST3Editor (this, "view", "SoundObject.uidesc");

		// suzumushi
		std::vector<double> zoom_factors = {0.5, 0.75, 1.0, 1.25, 1.5, 1.75, 2.0};
		view->setAllowedZoomFactors (zoom_factors);

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
