//
// Copyright (c) 2021 suzumushi
//
// 2021-11-1		SOprocessor.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

// suzumushi: 
#include "SOconfig.h"
#include "SOparam.h"
#include "SODSPparam.h"
#include "SOudsampling.h"
#include "SO2ndordIIRfilters.h"
#include "SOpinna.h"
#include "SOLPF.h"
#include "SOextparam.h"


namespace suzumushi {

//------------------------------------------------------------------------
//  SoundObjectProcessor
//------------------------------------------------------------------------
class SoundObjectProcessor: public Steinberg::Vst::AudioEffect
{
public:
	SoundObjectProcessor ();
	~SoundObjectProcessor () SMTG_OVERRIDE;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/) 
	{ 
		return (Steinberg::Vst::IAudioProcessor*)new SoundObjectProcessor; 
	}

	//--- ---------------------------------------------------------------------
	// AudioEffect overrides:
	//--- ---------------------------------------------------------------------
	/** Called at first after constructor */
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	
	/** Called at the end before destructor */
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	
	/** Switch the Plug-in on/off */
	Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;

	/** Will be called before any process call */
	Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
	
	/** Asks if a given sample size is supported see SymbolicSampleSizes. */
	Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

	/** Here we go...the process call */
	Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
		
	/** For persistence */
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

//------------------------------------------------------------------------
private:
	// suzumush: 
	// GUI and host facing parameters
	struct GUI_param gp;
	struct GUI_param next_gp;	// for setState ()

	// DSP facing parameters
	SODSPparam dp; 

	// DSP instances 
	SOudsampling <double> up_down_sampling_dL;
	SOudsampling <double> up_down_sampling_dR;
	SOsphere_scattering <double> sphere_scattering_dL;
	SOsphere_scattering <double> sphere_scattering_dR;
	SOpinna_scattering <double> pinna_scattering_dL;
	SOpinna_scattering <double> pinna_scattering_dR;
	SOudsampling <double> up_down_sampling_rL [6];
	SOudsampling <double> up_down_sampling_rR [6];
	SOsphere_scattering <double> sphere_scattering_rL [6];
	SOsphere_scattering <double> sphere_scattering_rR [6];
	SOpinna_scattering <double> pinna_scattering_rL [6];
	SOpinna_scattering <double> pinna_scattering_rR [6];
	SOLPF <double> LPF_L;
	SOLPF <double> LPF_R;

	// internal functions and status
	void gp_update ();						// for setState ()
	void dsp_reset ();	 	
	int unprocessed_len {0};				// unprocessed frame length
};

//------------------------------------------------------------------------
} // namespace suzumushi
