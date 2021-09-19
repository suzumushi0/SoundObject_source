//
// Copyright (c) 2021 suzumushi
//
// 2021-9-18		SODSPparam.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SODSPparam.h"


namespace suzumushi {

ParamValue SODSPparam:: calculateValue (ParamValue x, ParamValue y)
{
	x = std::floor (x * 1'000.0 + 0.5) * 0.001;
	y = std::floor (y * 1'000.0 + 0.5) * 0.000'000'1;
	return (x + y);
}
void SODSPparam:: calculateXY (ParamValue value, ParamValue& x, ParamValue& y)
{
	x = std::floor (value * 1'000.0 + 0.5) * 0.001;
	y = std::floor ((value - x) * 10'000'000.0 + 0.5) * 0.001;
}

double SODSPparam:: norm_to_taper (double norm)
{
	double coord = 1.0 - 2.0 * norm;
	double ret = (pow (81.0, abs (coord)) - 1.0) * max_side_len / 2.0 / 80.0;
	return (coord >= 0.0 ? ret : - ret);
}

double SODSPparam:: taper_to_norm (double taper)
{
	float ret = log (abs (taper) * 80.0 * 2.0 / max_side_len + 1.0) / log (81.0);
	if (taper < 0.0)
		ret = - ret;
	return ((1.0 - ret) / 2.0);
}

// real-time parameters update
void SODSPparam:: rt_param_update (struct GUI_param &gp, IParameterChanges* outParam)
{
	// smoothing mode
	if (! gp.first_frame && gp.param_changed)
		smoothing = true;
	else
		smoothing = false;

	// HVLines parameters feedback
	ParamValue hv_xy, hv_yz;
	if (gp.zerone) {
		gp.zerone = false;
		hv_xy = calculateValue (taper_to_norm (gp.r_y - gp.c_y), taper_to_norm (gp.r_x - gp.c_x));
		hv_yz = calculateValue (taper_to_norm (gp.r_y - gp.c_y), taper_to_norm (gp.r_z - gp.c_z));
	} else {
		gp.zerone = true;
		hv_xy = calculateValue (taper_to_norm (- gp.c_y), taper_to_norm (- gp.c_x));
		hv_yz = calculateValue (taper_to_norm (- gp.c_y), taper_to_norm (- gp.c_z));
	}
	int32 q_index = 0;			// paramQueue index
	if (outParam) {
		int32 p_index = 0;		// parameter index
		int32 p_offset = 0;		// parameter offset
		IParamValueQueue* paramQueue = outParam->addParameterData (HV_XY, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (hv_xy - hv_min) / (hv_max - hv_min), p_index);
		q_index++;
		paramQueue = outParam->addParameterData (HV_YZ, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (hv_yz - hv_min) / (hv_max - hv_min), p_index);
		q_index++;
	}

	// parameters update
	if (gp.first_frame || gp.param_changed) {
		gp.param_changed = false;
		if (gp.reflectance == reflectance_min)
			reflection = false;
		else
			reflection = true;

		// update of the acoustic source position
		if (gp.r_theta_changed || gp.phi_changed) {
			double theta_rad = gp.theta * pi / 180.0;
			double phi_rad = gp.phi * pi / 180.0;
			double r_cos_theta = gp.r * cos (theta_rad);
			gp.s_x = std::min (std::max (r_cos_theta * cos (phi_rad), s_x_min), s_x_max);
			gp.s_y = std::min (std::max (r_cos_theta * sin (phi_rad), s_y_min), s_y_max);
			if (gp.r_theta_changed)
				gp.s_z = std::min (std::max (gp.r * sin (theta_rad), s_z_min), s_z_max);
			gp.r_theta_changed = gp.phi_changed = false;
		}
		if (gp.yzpad_changed) {
			gp.yzpad_changed = false;
			double y, z;
			calculateXY (gp.yzpad, y, z);
			gp.s_y = norm_to_taper (y);
			gp.s_z = norm_to_taper (z);
		} 
		if (gp.xypad_changed) {
			gp.xypad_changed = false;
			double x, y;
			calculateXY (gp.xypad, y, x);
			gp.s_x = norm_to_taper (x);
			gp.s_y = norm_to_taper (y);
		}

		// position limiter
		if (reflection) {
			gp.s_x = std::min (std::max (gp.s_x, - v_z [4]), v_z [2]);	// z [4]: c_x, z [2]: r_x - c_x
			gp.s_y = std::min (std::max (gp.s_y, - v_z [1]), v_z [3]);	// z [1]: c_y, z [3]: r_y - c_y
			gp.s_z = std::min (std::max (gp.s_z, - v_z [0]), v_z [5]);	// z [0]: c_z, z [5]: r_z - c_z
		}

		// re-calculate positions
		double gp_s_x_2 = pow (gp.s_x, 2.0);
		double gp_s_y_2 = pow (gp.s_y, 2.0);
		gp.r = sqrt (gp_s_x_2 + gp_s_y_2 + pow (gp.s_z, 2.0));
		if (gp.r != 0.0) {
			gp.theta = asin (gp.s_z / gp.r) * 180.0 / pi;
			gp.phi= acos (gp.s_x / sqrt (gp_s_x_2 + gp_s_y_2)) * 180.0 / pi;
			if (gp.s_y < 0.0)
				gp.phi = 360.0 - gp.phi;
		}
		gp.xypad = calculateValue (taper_to_norm (gp.s_y), taper_to_norm (gp.s_x));
		gp.yzpad = calculateValue (taper_to_norm (gp.s_y), taper_to_norm (gp.s_z));

		// parameters feedback
		if (outParam) {
			int32 p_index = 0;		// parameter index
			int32 p_offset = 0;		// parameter offset
			IParamValueQueue* paramQueue = outParam->addParameterData (S_X, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.s_x - s_x_min) / (s_x_max - s_x_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (S_Y, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.s_y - s_y_min) / (s_y_max - s_y_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (S_Z, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.s_z - s_z_min) / (s_z_max - s_z_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (R, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.r - r_min) / (r_max - r_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (THETA, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.theta - theta_min) / (theta_max - theta_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (PHI, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.phi - phi_min) / (phi_max - phi_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (XYPAD, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.xypad - xypad_min) / (xypad_max - xypad_min), p_index);
			q_index++;
			paramQueue = outParam->addParameterData (YZPAD, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, (gp.yzpad - yzpad_min) / (yzpad_max - yzpad_min), p_index);
		}

		// velocity limiter
		double s_x = gp.s_x;
		double s_y = gp.s_y;
		double s_z = gp.s_z;
		if (! gp.first_frame) {
			double v = sqrt (pow (gp.s_x - prev_s_x, 2.0) + pow (gp.s_y - prev_s_y, 2.0) + pow (gp.s_z - prev_s_z, 2.0));
			if (v > v_limit) {
				s_x = (gp.s_x - prev_s_x) * v_limit / v + prev_s_x;
				s_y = (gp.s_y - prev_s_y) * v_limit / v + prev_s_y;
				s_z = (gp.s_z - prev_s_z) * v_limit / v + prev_s_z;
				gp.param_changed = true;
			} 
		}
		prev_s_x = s_x;
		prev_s_y = s_y;
		prev_s_z = s_z;

		// update of direct wave parameters
		double s_x_2 = pow (s_x, 2.0);
		double s_z_2 = pow (s_z, 2.0);
		double med_r = sqrt (s_x_2 + s_z_2);
		if (med_r != 0.0) {
			theta_p = acos (s_x / med_r);
			if (s_z < 0.0)
				theta_p = - theta_p;			
		}

		double r = sqrt (s_x_2 + pow (s_y, 2.0) + s_z_2);
		double r_2 = pow (r, 2.0);
		double r_cos_theta_o = s_y;											// bcause cos_theta_o = s_y / r
		double next_cos_theta_o = (r == 0.0 ? cos_theta_o: (r_cos_theta_o / r));
		double theta_o {0.0};
		double theta_d {0.0};
		if (r > a) {
			theta_o = acos (next_cos_theta_o);								// theta_o [0 .. pi]
			theta_d = acos (a / r);
		}
		if (a <= r_cos_theta_o || a >= r)
			distance_L = sqrt (a_2 + r_2 - 2.0 * a * r_cos_theta_o);
		else
			distance_L = sqrt (r_2 - a_2) + a * (theta_o - theta_d);
		if (a <= - r_cos_theta_o || a >= r)
			distance_R = sqrt (a_2 + r_2 + 2.0 * a * r_cos_theta_o);
		else
			distance_R = sqrt (r_2 - a_2) + a * (pi - theta_o - theta_d);

		// level limiter 
		double next_decay_L = 1.0 / std::max (distance_L, min_dist);
		double next_decay_R = 1.0 / std::max (distance_R, min_dist);

		// update of reflected waves parameters
		valarray <double> v_next_cos_theta_o (6);
		valarray <double> v_next_decay (6);
		double ref = pow (10.0, gp.reflectance / 20.0);				// from dB to ratio

		if (reflection) {
			v_X [0] = v_X [1] = v_X [3] = v_X [5] = s_x;
			v_Y [0] = v_Y [2] = v_Y [4] = v_Y [5] = s_y;
			v_X [2] = v_X [4] = v_Y [1] = v_Y [3] = s_z;
			v_Z [0] = v_z [0] + s_z;
			v_Z [1] = v_z [1] + s_y;
			v_Z [2] = v_z [2] - s_x;
			v_Z [3] = v_z [3] - s_y;
			v_Z [4] = v_z [4] + s_x;
			v_Z [5] = v_z [5] - s_z;

			// update of specular reflections parameters
			v_zdivZz = v_z / (v_z + v_Z);
			v_x0 = v_X * v_zdivZz;
			v_x0_2 = pow (v_x0, 2.0);
			v_y0 = v_Y * v_zdivZz;
			v_y0_2 = pow (v_y0, 2.0);
			v_in_dist = sqrt (pow (v_x0 - v_X, 2.0) + pow (v_y0 - v_Y, 2.0) + pow (v_Z, 2.0));
			v_ref_dist = sqrt (v_x0_2 + v_y0_2 + v_z_2);
			v_dist = v_in_dist + v_ref_dist;
			v_ref_ydir [0] = v_y0 [0];
			v_ref_ydir [2] = v_y0 [2];
			v_ref_ydir [4] = v_y0 [4];
			v_ref_ydir [5] = v_y0 [5];
			v_next_cos_theta_o = v_ref_ydir / v_ref_dist;

			v_theta_p [0] = - acos (v_x0 [0] / sqrt (v_x0_2 [0] + v_z_2 [0]));
			med_r = sqrt (v_x0_2 [1] + v_y0_2 [1]);
			if (med_r != 0) {
				v_theta_p [1] = v_theta_p [3] = acos (v_x0 [1] / med_r);
				if (v_y0 [1] < 0.0) 
					v_theta_p [1] = v_theta_p [3] = - v_theta_p [1];
			}
			v_theta_p [2] = acos (v_z [2] / sqrt (v_x0_2 [2] + v_z_2 [2]));
			if (v_x0 [2] < 0.0)
				v_theta_p [2] = - v_theta_p [2];
			v_theta_p [4] = acos (- v_z [4] / sqrt (v_x0_2 [4] + v_z_2 [4]));
			if (v_x0 [4] < 0.0)
				v_theta_p [4] = - v_theta_p [4];
			v_theta_p [5] = acos (v_x0 [5] / sqrt (v_x0_2 [5] + v_z_2 [5]));

			for (int i = 0; i < 6; i++) {
				if (v_next_cos_theta_o [i] >= 0.0) {
					v_distance_L [i] = v_dist [i] - a * v_next_cos_theta_o [i];
					v_distance_R [i] = v_dist [i] + a * (pi / 2.0 - acos (v_next_cos_theta_o [i]));
				} else {
					v_distance_L [i] = v_dist [i] + a * (acos (v_next_cos_theta_o [i]) - pi / 2.0);
					v_distance_R [i] = v_dist [i] + a * v_next_cos_theta_o [i];
				}
				// level limiter
				v_next_decay [i] = ref / std::max (v_dist [i], min_dist);
			}
		}

		// update of smoothing parameters
		if (gp.first_frame) {
			cos_theta_o = next_cos_theta_o;
			decay_L = next_decay_L;
			decay_R = next_decay_R;
			if (reflection) {
				v_cos_theta_o = v_next_cos_theta_o;
				v_decay = v_next_decay;
			}
		} else {
			delta_cos_theta_o = (next_cos_theta_o - cos_theta_o) / frame_len;
			delta_decay_L = (next_decay_L - decay_L) / frame_len;
			delta_decay_R = (next_decay_R - decay_R) / frame_len;
			if (reflection) {
				v_delta_cos_theta_o = (v_next_cos_theta_o - v_cos_theta_o) / frame_len;
				v_delta_decay = (v_next_decay - v_decay) / frame_len;
			}
		}
		gp.first_frame = false;
	}
}

// non real-time parameters update
void SODSPparam:: nrt_param_update (struct GUI_param &gp, IParameterChanges* outParam, const double sampleRate)
{
	inv_cT = sampleRate / gp.c;
	v_limit = max_speed * frame_len / inv_cT;
	a = gp.a / 1'000.0;		// [mm] to [m]
	a_2 = pow (a, 2.0);

	// parameters range check
	gp.c_x = std::max (gp.c_x, a);
	gp.c_y = std::max (gp.c_y, a);
	gp.c_z = std::max (gp.c_z, a);
	gp.r_x = std::min (gp.r_x, gp.c_x + max_side_len / 2.0);
	gp.r_y = std::min (gp.r_y, gp.c_y + max_side_len / 2.0);
	gp.r_z = std::min (gp.r_z, gp.c_z + max_side_len / 2.0);

	// parameters feedback
	if (outParam) {
		int32 q_index = 0;		// paramQueue index
		int32 p_index = 0;		// parameter index
		int32 p_offset = 0;		// parameter offset
		IParamValueQueue* paramQueue = outParam->addParameterData (R_X, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (gp.r_x - r_x_min) / (r_x_max - r_x_min), p_index);
		q_index++;
		paramQueue = outParam->addParameterData (R_Y, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (gp.r_y - r_y_min) / (r_y_max - r_y_min), p_index);
		q_index++;
		paramQueue = outParam->addParameterData (R_Z, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (gp.r_z - r_z_min) / (r_z_max - r_z_min), p_index);
		q_index++;
		paramQueue = outParam->addParameterData (C_X, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (gp.c_x - c_x_min) / (c_x_max - c_x_min), p_index);
		q_index++;
		paramQueue = outParam->addParameterData (C_Y, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (gp.c_y - c_y_min) / (c_y_max - c_y_min), p_index);
		q_index++;
		paramQueue = outParam->addParameterData (C_Z, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, (gp.c_z - c_z_min) / (c_z_max - c_z_min), p_index);
	}

	// update of reflected waves parameters
	v_z [0] = gp.c_z;
	v_z [1] = gp.c_y;
	v_z [2] = gp.r_x - gp.c_x;
	v_z [3] = gp.r_y - gp.c_y;
	v_z [4] = gp.c_x;
	v_z [5] = gp.r_z - gp.c_z;
	v_z_2 = pow (v_z, 2.0);

	// update of specular reflections parameters
	v_ref_ydir [1] = - v_z [1];
	v_ref_ydir [3] = v_z [3];
}

// parameters smoothing
void SODSPparam:: param_smoothing ()
{
	if (smoothing) {
		cos_theta_o += delta_cos_theta_o;
		decay_L += delta_decay_L;
		decay_R += delta_decay_R;
		if (reflection) {
			v_cos_theta_o += v_delta_cos_theta_o;
			v_decay += v_delta_decay;
		}
	}
}

} // namespace suzumushi