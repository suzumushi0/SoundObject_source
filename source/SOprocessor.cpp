//
// Copyright (c) 2021-2024 suzumushi
//
// 2024-3-10		SOprocessor.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOprocessor.h"
#include "SOcids.h"
#include "SOconfig.h"

#include "base/source/fstreamer.h"

using namespace Steinberg;

namespace suzumushi {
//------------------------------------------------------------------------
// SoundObjectProcessor
//------------------------------------------------------------------------
SoundObjectProcessor:: SoundObjectProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kSoundObjectControllerUID);
}

//------------------------------------------------------------------------
SoundObjectProcessor:: ~SoundObjectProcessor ()
{
	//
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: initialize (FUnknown* context)
{
	// Here the Plug-in will be instanciated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk) {
		return result;
	}

	//--- create Audio IO ------
	// suzumushi:
	addAudioInput  (STR16 ("Input"), Vst::SpeakerArr::kMono);
	addAudioOutput (STR16 ("Output"), Vst::SpeakerArr::kStereo);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: terminate ()
{
	// Here the Plug-in will be de-instanciated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: setActive (TBool state)
{
	// suzumushi:
	if (state != 0)				// if (state == true)
		reset ();

	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

    if (data.inputParameterChanges) {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
        for (int32 index = 0; index < numParamsChanged; index++) {
            if (auto* paramQueue = data.inputParameterChanges->getParameterData (index)) {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
				// suzumushi: get the last change
				if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue) {
					gui_param_update (paramQueue->getParameterId (), value);
				}
			}
		}
	}
	
	//--- Here you have to implement your processing

	gui_param_loading ();					// for setState ()

	// The first round of acoustic data processing after reset()
	if (gp.reset) {		
		dp.nrt_param_update (gp, data.outputParameterChanges, processSetup.sampleRate);
		unprocessed_len = 0;
		sphere_scattering_dL.setup (1.0 / dp.inv_cT, dp.a_r);
		sphere_scattering_dR.setup (1.0 / dp.inv_cT, dp.a_r);
		int SR = (processSetup.sampleRate + 0.5);
		pinna_scattering_dL.setup_SR (SR, L_CH);
		pinna_scattering_dR.setup_SR (SR, R_CH);
		for (int i = 0; i < 6; i++) {
			sphere_scattering_rL [i].setup (1.0 / dp.inv_cT, dp.a_r);
			sphere_scattering_rR [i].setup (1.0 / dp.inv_cT, dp.a_r);
			pinna_scattering_rL [i].setup_SR (SR, L_CH);
			pinna_scattering_rR [i].setup_SR (SR, R_CH);
		}
		xtalk_canceller_L.setup (1.0 / dp.inv_cT, dp.a_r);
		xtalk_canceller_R.setup (1.0 / dp.inv_cT, dp.a_r);
	}

	// numInputs == 0 and data.numOutputs == 0 mean parameters update only
	if (data.numInputs == 0 || data.numOutputs == 0) {
		return kResultOk;
	}
	// Speaker arrangements (at least mono in and stereo out are required) check.
	if (data.inputs[0].numChannels == 0 || data.outputs[0].numChannels <= 1) {
		return kResultOk;
	}

	Vst::Sample32* in = data.inputs[0].channelBuffers32[0];
	Vst::Sample32* out_L = data.outputs[0].channelBuffers32[0];
	Vst::Sample32* out_R = data.outputs[0].channelBuffers32[1];

	if (gp.bypass) {
		// bypass mode
		for (int32 i = 0; i < data.numSamples; i++) {
			if (data.inputs[0].silenceFlags == 0) {
				// all silenceFlags are false
				*out_L++ = *out_R++ = *in++;
			} else {
				// some silenceFlags are true
				*out_L++ = *out_R++ = 0.0;
				data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
			}
		}
	} else {
		// DSP mode
		for (int32 i = 0; i < data.numSamples; i++) {				
			if (unprocessed_len == 0) {
				unprocessed_len = frame_len;
				dp.rt_param_update (gp, data.outputParameterChanges);
				up_down_sampling_dL.setup (dp.inv_cT, dp.distance_L);
				up_down_sampling_dR.setup (dp.inv_cT, dp.distance_R);
				pinna_scattering_dL.setup (dp.theta_p, gp.hrir);
				pinna_scattering_dR.setup (dp.theta_p, gp.hrir);
				for (int i = 0; i < 6; i++) {
					up_down_sampling_rL [i].setup (dp.inv_cT, dp.v_distance_L [i]);
					up_down_sampling_rR [i].setup (dp.inv_cT, dp.v_distance_R [i]);
					pinna_scattering_rL [i].setup (dp.v_theta_p [i], gp.hrir);
					pinna_scattering_rR [i].setup (dp.v_theta_p [i], gp.hrir);
				}
				LPF_L.setup (gp.fc, processSetup.sampleRate);
				LPF_R.setup (gp.fc, processSetup.sampleRate);
			}
			dp.param_smoothing ();
			double d_ud_L, d_ud_R;
			if (data.inputs[0].silenceFlags == 0) {
				d_ud_L = up_down_sampling_dL.process (*in, dp.decay_L);
				d_ud_R = up_down_sampling_dR.process (*in, dp.decay_R);
			} else {
				d_ud_L = up_down_sampling_dL.process (0.0, dp.decay_L);
				d_ud_R = up_down_sampling_dR.process (0.0, dp.decay_R);
			}
			double d_sc_L = sphere_scattering_dL.process (d_ud_L, dp.cos_theta_o);
			double d_sc_R = sphere_scattering_dR.process (d_ud_R, - dp.cos_theta_o);
			double d_out_L = pinna_scattering_dL.process (d_sc_L);
			double d_out_R = pinna_scattering_dR.process (d_sc_R);
			double r_out_L = 0.0, r_out_R = 0.0;
			double b_out_L = 0.0, b_out_R = 0.0;
			for (int i = 0; i < 6; i++) {
				double r_ud_L, r_ud_R;
				if (data.inputs[0].silenceFlags == 0) {
					r_ud_L = up_down_sampling_rL [i].process (*in, dp.v_decay [i]);
					r_ud_R = up_down_sampling_rR [i].process (*in, dp.v_decay [i]);
				} else {
					r_ud_L = up_down_sampling_rL [i].process (0.0, dp.v_decay [i]);
					r_ud_R = up_down_sampling_rR [i].process (0.0, dp.v_decay [i]);
				}
				double r_sc_L = sphere_scattering_rL [i].process (r_ud_L, dp.v_cos_theta_o [i]);
				double r_sc_R = sphere_scattering_rR [i].process (r_ud_R, - dp.v_cos_theta_o [i]);
				r_out_L += pinna_scattering_rL [i].process (r_sc_L);
				r_out_R += pinna_scattering_rR [i].process (r_sc_R);
			}
			r_out_L = LPF_L.process (r_out_L);
			r_out_R = LPF_R.process (r_out_R);
			in++;
			switch (gp.output) {
				case (int32) OUTPUT_L:: COMBINED_WAVES:
					b_out_L = d_out_L + r_out_L;
					b_out_R = d_out_R + r_out_R;
					break;
				case (int32) OUTPUT_L:: DIRECT_WAVE:
					b_out_L = d_out_L;
					b_out_R = d_out_R;
					break;
				case (int32) OUTPUT_L:: REFLECTED_WAVES:
					b_out_L = r_out_L;
					b_out_R = r_out_R;
					break;
				case (int32) OUTPUT_L:: SCATTERED_WAVE:
					b_out_L = d_sc_L;
					b_out_R = d_sc_R;
					break;
				case (int32) OUTPUT_L:: INCIDENT_WAVE:
					b_out_L = d_ud_L;
					b_out_R = d_ud_R;
					break;
			}

			double para_L, para_R, cross_L, cross_R;
			xtalk_canceller_L.process (b_out_L, dp.sin_phiL, para_L, cross_L);
			xtalk_canceller_R.process (b_out_R, dp.sin_phiL, para_R, cross_R);
			if (gp.format == (int32) FORMAT_L:: BINAURAL) {
				*out_L++ = b_out_L;
				*out_R++ = b_out_R;
			} else {	// TRANSAURAL
				*out_L++ = para_L + cross_R;
				*out_R++ = para_R + cross_L;
			}
			unprocessed_len--;
		}
	}
	return (kResultOk);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return (kResultFalse);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
	
	// suzumushi:
	if (gp_load.param_changed == true)
		return (kResultFalse);

	if (streamer.readDouble (gp_load.s_x) == true) {
		if (gp_load.s_x > max_side_len / 2.0) {				// new format
			int version;
			if (streamer.readInt32 (version) == false)		// read version
				return (kResultFalse);
			if (streamer.readDouble (gp_load.s_x) == false)	// read s_x
				return (kResultFalse);
		}
	} else
		return (kResultFalse);
	if (streamer.readDouble (gp_load.s_y) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.s_z) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.r) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.theta) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.phi) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.xypad) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.yzpad) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.reflectance) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.fc) == false)
		return (kResultFalse);

	if (streamer.readDouble (gp_load.c) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.a) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.r_x) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.r_y) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.r_z) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.c_x) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.c_y) == false)
		return (kResultFalse);
	if (streamer.readDouble (gp_load.c_z) == false)
		return (kResultFalse);

	if (streamer.readInt32 (gp_load.hrir) == false)
		return (kResultFalse);
	if (streamer.readInt32 (gp_load.output) == false)
		return (kResultFalse);
	if (streamer.readInt32 (gp_load.bypass) == false)
		return (kResultFalse);

	// for backward compatibility 
	if (streamer.readInt32 (gp_load.format) == false)
		gp_load.format = (int32) FORMAT_L:: BINAURAL;
	if (streamer.readDouble (gp_load.phiL) == false)
		gp_load.phiL = phiL.def;
	if (streamer.readDouble (gp_load.d_att) == false)
		gp_load.d_att = d_att.def;

	gp_load.param_changed = true;

	return (kResultOk);
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	// suzumushi:
	if (streamer.writeDouble (max_side_len) == false)		// new format
		return (kResultFalse);
	int version = 0;										// version 3.0.0
	if (streamer.writeInt32 (version) == false)				
		return (kResultFalse);

	if (streamer.writeDouble (gp.s_x) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.s_y) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.s_z) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.r) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.theta) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.phi) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.xypad) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.yzpad) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.reflectance) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.fc) == false)
		return (kResultFalse);

	if (streamer.writeDouble (gp.c) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.a) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.r_x) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.r_y) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.r_z) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.c_x) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.c_y) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.c_z) == false)
		return (kResultFalse);

	if (streamer.writeInt32 (gp.hrir) == false)
		return (kResultFalse);
	if (streamer.writeInt32 (gp.output) == false)
		return (kResultFalse);
	if (streamer.writeInt32 (gp.bypass) == false)
		return (kResultFalse);
	if (streamer.writeInt32 (gp.format) == false)
		return (kResultFalse);

	if (streamer.writeDouble (gp.phiL) == false)
		return (kResultFalse);
	if (streamer.writeDouble (gp.d_att) == false)
		return (kResultFalse);

	return (kResultOk);
}

//------------------------------------------------------------------------
// suzumushi:

void SoundObjectProcessor:: gui_param_loading ()
{
	if (gp_load.param_changed) {
		gp.s_x = gp_load.s_x;
		gp.s_y = gp_load.s_y;
		gp.s_z = gp_load.s_z;
		gp.r = gp_load.r;
		gp.theta = gp_load.theta;
		gp.phi = gp_load.phi;
		gp.xypad = gp_load.xypad;
		gp.yzpad = gp_load.yzpad;
		gp.reflectance = gp_load.reflectance;
		gp.fc = gp_load.fc;
		gp.phiL = gp_load.phiL;
		gp.d_att = gp_load.d_att;

		gp.c = gp_load.c;
		gp.a = gp_load.a;
		gp.r_x = gp_load.r_x;
		gp.r_y = gp_load.r_y;
		gp.r_z = gp_load.r_z;
		gp.c_x = gp_load.c_x;
		gp.c_y = gp_load.c_y;
		gp.c_z = gp_load.c_z;

		gp.hrir = gp_load.hrir;
		gp.output = gp_load.output;
		gp.format = gp_load.format;
		gp.bypass = gp_load.bypass;

		gp_load.param_changed = false;
		reset ();
	}
}

void SoundObjectProcessor:: gui_param_update (const ParamID paramID, const ParamValue paramValue) 
{
	Vst::ParamValue update;

	switch (paramID) {
		case s_x.tag:
			update = rangeParameter:: toPlain (paramValue, s_x);
			if (gp.s_x != update) {
				gp.s_x = update;
				gp.param_changed = true;
			}
			break;
		case s_y.tag:
			update = rangeParameter:: toPlain (paramValue, s_y);
			if (gp.s_y != update) {
				gp.s_y = update;
				gp.param_changed = true;
			}
			break;
		case s_z.tag:
			update = rangeParameter:: toPlain (paramValue, s_z);
			if (gp.s_z != update) {
				gp.s_z = update;
				gp.param_changed = true;
			}
			break;
		case r.tag:
			update = rangeParameter:: toPlain (paramValue, r);
			if (gp.r != update) {
				gp.r = update;
				gp.param_changed = gp.r_theta_changed = true;
			}
			break;
		case theta.tag:
			update = rangeParameter:: toPlain (paramValue, theta);
			if (gp.theta != update) {
				gp.theta = update;
				gp.param_changed = gp.r_theta_changed = true;
			}
			break;
		case phi.tag:
			update = rangeParameter:: toPlain (paramValue, phi);
			if (gp.phi != update) {
				gp.phi = update;
				gp.param_changed = gp.phi_changed = true;
			}
			break;
		case xypad.tag:
			update = rangeParameter:: toPlain (paramValue, xypad);
			if (gp.xypad != update) {
				gp.xypad = update;
				gp.param_changed = gp.xypad_changed = true;
			}
			break;
		case yzpad.tag:
			update = rangeParameter:: toPlain (paramValue, yzpad);
			if (gp.yzpad != update) {
				gp.yzpad = update;
				gp.param_changed = gp.yzpad_changed = true;
			}
			break;
		case reflectance.tag:
			update = rangeParameter:: toPlain (paramValue, reflectance);
			if (gp.reflectance != update) {
				gp.reflectance = update;
				gp.param_changed = true;
			}
			break;
		case fc.tag:
			gp.fc = InfLogTaperParameter:: toPlain (paramValue, fc); 
			break;

		case c.tag:
			gp.c = rangeParameter:: toPlain (paramValue, c);
			break;
		case a.tag:
			gp.a = rangeParameter:: toPlain (paramValue, a);
			break;
		case r_x.tag:
			update = rangeParameter:: toPlain (paramValue, r_x);
			if (gp.r_x != update)
				gp.r_x = update;
			break;
		case r_y.tag:
			update = rangeParameter:: toPlain (paramValue, r_y);
			if (gp.r_y != update)
				gp.r_y = update;
			break;
		case r_z.tag:
			update = rangeParameter:: toPlain (paramValue, r_z);
			if (gp.r_z != update)
				gp.r_z = update;
			break;
		case c_x.tag:
			update = rangeParameter:: toPlain (paramValue, c_x);
			if (gp.c_x != update)
				gp.c_x = update;
			break;
		case c_y.tag:
			update = rangeParameter:: toPlain (paramValue, c_y);
			if (gp.c_y != update)
				gp.c_y = update;
			break;
		case c_z.tag:
			update = rangeParameter:: toPlain (paramValue, c_z);
			if (gp.c_z != update)
				gp.c_z = update;
			break;
		case hrir.tag:
			gp.hrir = stringListParameter:: toPlain (paramValue, (int32) HRIR_L:: LIST_LEN);
			break;
		case output.tag:
			gp.output = stringListParameter:: toPlain (paramValue, (int32) OUTPUT_L:: LIST_LEN);
			break;
		case bypass.tag:
			gp.bypass = paramValue;
			if (! gp.bypass)				// return from bypass
				reset ();
			break;
		case format.tag:
			gp.format = stringListParameter:: toPlain (paramValue, (int32 )FORMAT_L:: LIST_LEN);
			break;
		case phiL.tag:
			update = rangeParameter:: toPlain (paramValue, phiL);
			if (gp.phiL != update) {
				gp.phiL = update;
				gp.param_changed = true;
			}
			break;
		case d_att.tag:
			update = rangeParameter:: toPlain (paramValue, d_att);
			if (gp.d_att != update) {
				gp.d_att = update;
				gp.param_changed = true;
			}
			break;
	}
}

void SoundObjectProcessor:: reset ()
{
	up_down_sampling_dL.reset ();
	up_down_sampling_dR.reset ();
	sphere_scattering_dL.reset ();
	sphere_scattering_dR.reset ();
	pinna_scattering_dL.reset ();
	pinna_scattering_dR.reset ();
	for (int i = 0; i < 6; i++) {
		up_down_sampling_rL [i].reset ();
		up_down_sampling_rR [i].reset ();
		sphere_scattering_rL [i].reset ();
		sphere_scattering_rR [i].reset ();
		pinna_scattering_rL [i].reset ();
		pinna_scattering_rR [i].reset ();
	}
	LPF_L.reset ();
	LPF_R.reset ();
	xtalk_canceller_L.reset ();
	xtalk_canceller_R.reset ();

	gp.reset = true;
}

//------------------------------------------------------------------------
} // namespace suzumushi
