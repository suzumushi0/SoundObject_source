//
// Copyright (c) 2021 suzumushi
//
// 2021-12-18		SOprocessor.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SOprocessor.h"
#include "SOcids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"


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
		dsp_reset();

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
				Vst::ParamValue update;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
				// suzumushi: get the last change
				if (paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue) {
					switch (paramQueue->getParameterId ()) {
						case S_X:
							update = value * (s_x_max - s_x_min) + s_x_min;
							if (gp.s_x != update) {
								gp.s_x = update;
								gp.param_changed = true;
							}
							break;
						case S_Y:
							update = value * (s_y_max - s_y_min) + s_y_min;
							if (gp.s_y != update) {
								gp.s_y = update;
								gp.param_changed = true;
							}
							break;
						case S_Z:
							update = value * (s_z_max - s_z_min) + s_z_min;
							if (gp.s_z != update) {
								gp.s_z = update;
								gp.param_changed = true;
							}
							break;
						case R:
							update = value * (r_max - r_min) + r_min;
							if (gp.r != update) {
								gp.r = update;
								gp.param_changed = gp.r_theta_changed = true;
							}
							break;
						case THETA:
							update = value * (theta_max - theta_min) + theta_min;
							if (gp.theta != update) {
								gp.theta = update;
								gp.param_changed = gp.r_theta_changed = true;
							}
							break;
						case PHI:
							update = value * (phi_max - phi_min) + phi_min;
							if (gp.phi != update) {
								gp.phi = update;
								gp.param_changed = gp.phi_changed = true;
							}
							break;
						case XYPAD:
							update = value * (xypad_max - xypad_min) + xypad_min;
							if (gp.xypad != update) {
								gp.xypad = update;
								gp.param_changed = gp.xypad_changed = true;
							}
							break;
						case YZPAD:
							update = value * (yzpad_max - yzpad_min) + yzpad_min;
							if (gp.yzpad != update) {
								gp.yzpad = update;
								gp.param_changed = gp.yzpad_changed = true;
							}
							break;
						case REFLECTANCE:
							update = value * (reflectance_max - reflectance_min) + reflectance_min;
							if (gp.reflectance != update) {
								gp.reflectance = update;
								gp.param_changed = true;
							}
							break;
						case FC:
							gp.fc = InfLogTaperParameter:: to_plain (value, fc_min, fc_max); 
							break;

						case C:
							gp.c = value * (c_max - c_min) + c_min;
							break;
						case A:
							gp.a = value * (a_max - a_min) + a_min;
							break;
						case R_X:
							update = value * (r_x_max - r_x_min) + r_x_min;
							if (gp.r_x != update) {
								gp.r_x = update;
								gp.room_update ();
							}
							break;
						case R_Y:
							update = value * (r_y_max - r_y_min) + r_y_min;
							if (gp.r_y != update) {
								gp.r_y = update;
								gp.room_update ();
							}
							break;
						case R_Z:
							update = value * (r_z_max - r_z_min) + r_z_min;
							if (gp.r_z != update) {
								gp.r_z = update;
								gp.room_update ();
							}
							break;
						case C_X:
							update = value * (c_x_max - c_x_min) + c_x_min;
							if (gp.c_x != update) {
								gp.c_x = update;
								gp.room_update ();
							}
							break;
						case C_Y:
							update = value * (c_y_max - c_y_min) + c_y_min;
							if (gp.c_y != update) {
								gp.c_y = update;
								gp.room_update ();
							}
							break;
						case C_Z:
							update = value * (c_z_max - c_z_min) + c_z_min;
							if (gp.c_z != update) {
								gp.c_z = update;
								gp.room_update ();
							}
							break;
						case HRIR:
							gp.hrir = static_cast <int32> (value * (HRIR_LIST_LEN - 1) + 0.5);
							break;
						case OUTPUT:
							gp.output = static_cast <int32> (value * (OUTPUT_LIST_LEN - 1) + 0.5);
							break;
						case BYPASS:
							gp.bypass = static_cast <int32> (value);
							if (! gp.bypass)				// return from bypass
								dsp_reset();
							break;
					}
				}
			}
		}
	}
	
	//--- Here you have to implement your processing

	gp_update ();					// for setState ()

	// numInputs == 0 and data.numOutputs == 0 mean parameters update only
	if (data.numInputs == 0 || data.numOutputs == 0) {
		return kResultOk;
	}
	// Speaker arrangements (at least mono in and stereo out are required) check.
	if (data.inputs[0].numChannels == 0 || data.outputs[0].numChannels <= 1) {
		return kResultOk;
	}

	// The first round of acoustic data processing after dsp_reset()
	if (gp.first_frame) {		
		dp.nrt_param_update (gp, data.outputParameterChanges, processSetup.sampleRate);
		unprocessed_len = 0;
		sphere_scattering_dL.setup (1.0 / dp.inv_cT, dp.a);
		sphere_scattering_dR.setup (1.0 / dp.inv_cT, dp.a);
		int SR = (processSetup.sampleRate + 0.5);
		pinna_scattering_dL.setSR (SR, L_CH);
		pinna_scattering_dR.setSR (SR, R_CH);
		for (int i = 0; i < 6; i++) {
			sphere_scattering_rL [i].setup (1.0 / dp.inv_cT, dp.a);
			sphere_scattering_rR [i].setup (1.0 / dp.inv_cT, dp.a);
			pinna_scattering_rL [i].setSR (SR, L_CH);
			pinna_scattering_rR [i].setSR (SR, R_CH);
		}
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
				case COMBINED_WAVES:
					*out_L++ = d_out_L + r_out_L;
					*out_R++ = d_out_R + r_out_R;
					break;
				case DIRECT_WAVE:
					*out_L++ = d_out_L;
					*out_R++ = d_out_R;
					break;
				case REFLECTED_WAVES:
					*out_L++ = r_out_L;
					*out_R++ = r_out_R;
					break;
				case SCATTERED_WAVE:
					*out_L++ = d_sc_L;
					*out_R++ = d_sc_R;
					break;
				case INCIDENT_WAVE:
					*out_L++ = d_ud_L;
					*out_R++ = d_ud_R;
					break;
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
	if (next_gp.param_changed == true)
		return (kResultFalse);

	if (streamer.readDouble (next_gp.s_x) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.s_y) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.s_z) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.r) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.theta) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.phi) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.xypad) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.yzpad) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.reflectance) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.fc) == false)
		return (kResultFalse);

	if (streamer.readDouble (next_gp.c) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.a) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.r_x) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.r_y) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.r_z) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.c_x) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.c_y) == false)
		return (kResultFalse);
	if (streamer.readDouble (next_gp.c_z) == false)
		return (kResultFalse);

	if (streamer.readInt32 (next_gp.hrir) == false)
		return (kResultFalse);
	if (streamer.readInt32 (next_gp.output) == false)
		return (kResultFalse);
	if (streamer.readInt32 (next_gp.bypass) == false)
		return (kResultFalse);

	next_gp.param_changed = true;

	return (kResultOk);
}

void SoundObjectProcessor:: gp_update ()
{
	if (next_gp.param_changed) {
		gp.s_x = next_gp.s_x;
		gp.s_y = next_gp.s_y;
		gp.s_z = next_gp.s_z;
		gp.r = next_gp.r;
		gp.theta = next_gp.theta;
		gp.phi = next_gp.phi;
		gp.xypad = next_gp.xypad;
		gp.yzpad = next_gp.yzpad;
		gp.reflectance = next_gp.reflectance;
		gp.fc = next_gp.fc;

		gp.c = next_gp.c;
		gp.a = next_gp.a;
		gp.r_x = next_gp.r_x;
		gp.r_y = next_gp.r_y;
		gp.r_z = next_gp.r_z;
		gp.c_x = next_gp.c_x;
		gp.c_y = next_gp.c_y;
		gp.c_z = next_gp.c_z;

		gp.hrir = next_gp.hrir;
		gp.output = next_gp.output;
		gp.bypass = next_gp.bypass;

		next_gp.param_changed = false;
		dsp_reset ();
	}
}

//------------------------------------------------------------------------
tresult PLUGIN_API SoundObjectProcessor:: getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	// suzumushi:
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

	return (kResultOk);
}

//------------------------------------------------------------------------
// suzumushi:
// DSP reset
void SoundObjectProcessor:: dsp_reset ()
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
	gp.first_frame = true;
}

//------------------------------------------------------------------------
} // namespace suzumushi
