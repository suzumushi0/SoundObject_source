//
// Copyright (c) 2021-2024 suzumushi
//
// 2024-3-21		SODSPparam.cpp
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#include "SODSPparam.h"
#include "SOXYPad.h"


namespace suzumushi {

// real-time parameters update
void SODSPparam:: rt_param_update (struct GUI_param &gp, IParameterChanges* outParam)
{
	// smoothing mode
	if (! gp.reset && gp.param_changed)
		smoothing = true;
	else
		smoothing = false;

	// parameters update
	if (gp.reset || gp.param_changed) {
		gp.param_changed = false;

		// update of acoustic source position
		if (gp.r_theta_changed || gp.phi_changed) {
			double theta_rad = gp.theta * pi / 180.0;
			double phi_rad = gp.phi * pi / 180.0;
			double r_cos_theta = gp.r * cos (theta_rad);
			gp.s_x = std::min (std::max (r_cos_theta * cos (phi_rad), s_x.min), s_x.max);
			gp.s_y = std::min (std::max (r_cos_theta * sin (phi_rad), s_y.min), s_y.max);
			if (gp.r_theta_changed)
				gp.s_z = std::min (std::max (gp.r * sin (theta_rad), s_z.min), s_z.max);
			gp.r_theta_changed = gp.phi_changed = false;
		}
		if (gp.yzpad_changed) {
			gp.yzpad_changed = false;
			double y, z;
			VSTGUI::SOXYPad::calculateXY (gp.yzpad, y, z);
			gp.s_y = VSTGUI::SOXYPad:: norm_to_taper (y);
			gp.s_z = VSTGUI::SOXYPad:: norm_to_taper (z);
		} 
		if (gp.xypad_changed) {
			gp.xypad_changed = false;
			double x, y;
			VSTGUI::SOXYPad::calculateXY (gp.xypad, y, x);
			gp.s_x = VSTGUI::SOXYPad:: norm_to_taper (x);
			gp.s_y = VSTGUI::SOXYPad:: norm_to_taper (y);
		}

		// position limiter
		gp.s_x = std::min (std::max (gp.s_x, edge [1]), edge [0]);
		gp.s_y = std::min (std::max (gp.s_y, edge [3]), edge [2]);
		gp.s_z = std::min (std::max (gp.s_z, edge [5]), edge [4]);

		// re-calculate positions
		double gp_s_x_2_y_2 = gp.s_x * gp.s_x + gp.s_y * gp.s_y;
		gp.r = sqrt (gp_s_x_2_y_2 + gp.s_z * gp.s_z);
		if (gp.r != 0.0) {
			gp.theta = asin (gp.s_z / gp.r) * 180.0 / pi;
			gp.phi= acos (gp.s_x / sqrt (gp_s_x_2_y_2)) * 180.0 / pi;
			if (gp.s_y < 0.0)
				gp.phi = 360.0 - gp.phi;
		}
		double x_axis = VSTGUI::SOXYPad::taper_to_norm (gp.s_y);
		gp.xypad = VSTGUI::SOXYPad:: calculateValue (x_axis, VSTGUI::SOXYPad:: taper_to_norm (gp.s_x));
		gp.yzpad = VSTGUI::SOXYPad:: calculateValue (x_axis, VSTGUI::SOXYPad:: taper_to_norm (gp.s_z));

		// parameters feedback
		if (outParam) {
			int32 q_index = 0;		// paramQueue index
			int32 p_index = 0;		// parameter index
			int32 p_offset = 0;		// parameter offset
			IParamValueQueue* paramQueue = outParam->addParameterData (s_x.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.s_x, s_x), p_index);
			paramQueue = outParam->addParameterData (s_y.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.s_y, s_y), p_index);
			paramQueue = outParam->addParameterData (s_z.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.s_z, s_z), p_index);
			paramQueue = outParam->addParameterData (r.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.r, r), p_index);
			paramQueue = outParam->addParameterData (theta.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.theta, theta), p_index);
			paramQueue = outParam->addParameterData (phi.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.phi, phi), p_index);
			paramQueue = outParam->addParameterData (xypad.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.xypad, xypad), p_index);
			paramQueue = outParam->addParameterData (yzpad.tag, q_index);
			if (paramQueue)
				paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.yzpad, yzpad), p_index);
		}

		// velocity limiter
		double s_x = gp.s_x;
		double s_y = gp.s_y;
		double s_z = gp.s_z;
		if (! gp.reset) {
			double d_s_x = s_x - prev_s_x;
			double d_s_y = s_y - prev_s_y;
			double d_s_z = s_z - prev_s_z;
			double v = sqrt (d_s_x * d_s_x + d_s_y * d_s_y + d_s_z * d_s_z);
			if (v > v_limit) {
				double cof = v_limit / v;
				s_x = d_s_x * cof + prev_s_x;
				s_y = d_s_y * cof + prev_s_y;
				s_z = d_s_z * cof + prev_s_z;
				gp.param_changed = true;
			} 
		}
		prev_s_x = s_x;
		prev_s_y = s_y;
		prev_s_z = s_z;
		double s_x_2 = s_x * s_x;
		double s_y_2 = s_y * s_y;
		double s_z_2 = s_z * s_z;


		// update of direct wave parameters
		double r_2 = s_x_2 + s_y_2 + s_z_2;
		double r = sqrt (r_2);
		double med_r = sqrt (s_x_2 + s_z_2);
		theta_p = 0.0;
		if (r > a_r && med_r != 0.0) {
			theta_p = acos (s_x / med_r);
			if (s_z < 0.0)
				theta_p = - theta_p;			
		}

		double r_cos_theta_o = s_y;											// because cos_theta_o = s_y / r
		double next_cos_theta_o = (a_r >= r ? 0.0: (r_cos_theta_o / r));
		double theta_o {0.0};
		double theta_d {0.0};
		if (r > a_r) {
			theta_o = acos (next_cos_theta_o);								// theta_o [0 .. pi]
			theta_d = acos (a_r / r);
		}
		if (a_r <= r_cos_theta_o || a_r >= r)
			distance_L = sqrt (a_2 + r_2 - 2.0 * a_r * r_cos_theta_o);
		else
			distance_L = sqrt (r_2 - a_2) + a_r * (theta_o - theta_d);
		if (a_r <= - r_cos_theta_o || a_r >= r)
			distance_R = sqrt (a_2 + r_2 + 2.0 * a_r * r_cos_theta_o);
		else
			distance_R = sqrt (r_2 - a_2) + a_r * ((pi - theta_o) - theta_d);

		double d_att = gp.d_att / -6.0;
		double next_decay_L = 1.0 / std::max (pow (distance_L, d_att), min_dist);
		double next_decay_R = 1.0 / std::max (pow (distance_R, d_att), min_dist);


		// update of reflected waves parameters
		double v_s_x [] {2.0 * edge [0] - s_x, 2.0 * edge [1] - s_x, s_x, s_x, s_x, s_x};
		double v_s_y [] {s_y, s_y, 2.0 * edge [2] - s_y, 2.0 * edge [3] - s_y, s_y, s_y};
		double v_s_z [] {s_z, s_z, s_z, s_z, 2.0 * edge [4] - s_z, 2.0 * edge [5] - s_z};
		double v_s_x_2 [] {v_s_x [0] * v_s_x [0], v_s_x [1] * v_s_x [1], s_x_2, s_x_2, s_x_2, s_x_2};
		double v_s_y_2 [] {s_y_2, s_y_2, v_s_y [2] * v_s_y [2], v_s_y [3] * v_s_y [3], s_y_2, s_y_2};
		double v_s_z_2 [] {s_z_2, s_z_2, s_z_2, s_z_2, v_s_z [4] * v_s_z [4], v_s_z [5] * v_s_z [5]};

		double v_s_x_2_z_2 [6];
		for (int i = 0; i < 6; i++)
			v_s_x_2_z_2 [i] = v_s_x_2 [i] + v_s_z_2 [i];
		double v_med_r [6];
		for (int i = 0; i < 6; i++)
			v_med_r [i] = sqrt (v_s_x_2_z_2 [i]);

		for (int i = 0; i < 6; i++) {
			if (v_med_r [i] != 0.0)
				v_theta_p [i] = acos (v_s_x [i] / v_med_r [i]);
			else
				v_theta_p [i] = 0.0;
			if (v_s_z [i] < 0.0)
				v_theta_p [i] = - v_theta_p [i];
		}

		double v_r [6];
		for (int i = 0; i < 6; i++)
			v_r [i] = sqrt (v_s_x_2_z_2 [i] + v_s_y_2 [i]);
		double v_next_cos_theta_o [6];
		for (int i = 0; i < 6; i++)
			v_next_cos_theta_o [i] = v_s_y [i] / v_r [i];
		double v_near_side [6];
		for (int i = 0; i < 6; i++)
			v_near_side [i] = a_r * v_next_cos_theta_o [i];
		double v_far_side [6];
		for (int i = 0; i < 6; i++)
			v_far_side [i] = a_r * (pi / 2.0 - acos (v_next_cos_theta_o [i]));

		for (int i = 0; i < 6; i++) {
			if (v_next_cos_theta_o [i] >= 0.0) {
				v_distance_L [i] = v_r [i] - v_near_side [i];
				v_distance_R [i] = v_r [i] + v_far_side [i];
			} else {
				v_distance_L [i] = v_r [i] - v_far_side [i];
				v_distance_R [i] = v_r [i] + v_near_side [i];
			}
		}

		double ref = rangeParameter:: dB_to_ratio (gp.reflectance);
		double v_next_decay [6];
		for (int i = 0; i < 6; i++)		
			v_next_decay [i] = ref / std::max (pow (v_r [i], d_att), min_dist);


		// update of parameter for crosstalk canceller 
		sin_phiL = sin (gp.phiL * pi / 180.0);

		// update of smoothing parameters
		if (gp.reset) {
			cos_theta_o = next_cos_theta_o;
			decay_L = next_decay_L;
			decay_R = next_decay_R;
			for (int i = 0; i < 6; i++) {
				v_cos_theta_o [i] = v_next_cos_theta_o [i];
				v_decay [i] = v_next_decay [i];
			}
		} else {
			delta_cos_theta_o = (next_cos_theta_o - cos_theta_o) / frame_len;
			delta_decay_L = (next_decay_L - decay_L) / frame_len;
			delta_decay_R = (next_decay_R - decay_R) / frame_len;
			for (int i = 0; i < 6; i++) {
				v_delta_cos_theta_o [i] = (v_next_cos_theta_o [i] - v_cos_theta_o [i]) / frame_len;
				v_delta_decay [i] = (v_next_decay [i] - v_decay [i]) / frame_len;
			}
		}
		gp.reset = false;
	}
}

// non real-time parameters update
void SODSPparam:: nrt_param_update (struct GUI_param &gp, IParameterChanges* outParam, const double sampleRate)
{
	inv_cT = sampleRate / gp.c;
	v_limit = max_speed * frame_len / inv_cT;
	a_r = gp.a / 1'000.0;		// [mm] to [m]
	a_2 = a_r * a_r;

	// parameters range check
	if (gp.r_x >= max_side_len / 2.0 + min_dist)
		gp.c_x = std::max (gp.c_x, gp.r_x - max_side_len / 2.0);
	else
		gp.c_x = std::min (gp.c_x, gp.r_x - min_dist);
	if (gp.r_y >= max_side_len / 2.0 + min_dist)
		gp.c_y = std::max (gp.c_y, gp.r_y - max_side_len / 2.0);
	else
		gp.c_y = std::min (gp.c_y, gp.r_y - min_dist);
	if (gp.r_z >= max_side_len / 2.0 + min_dist)
		gp.c_z = std::max (gp.c_z, gp.r_z - max_side_len / 2.0);
	else
		gp.c_z = std::min (gp.c_z, gp.r_z - min_dist);

	// parameters feedback
	if (outParam) {
		int32 q_index = 0;		// paramQueue index
		int32 p_index = 0;		// parameter index
		int32 p_offset = 0;		// parameter offset
		IParamValueQueue* paramQueue = outParam->addParameterData (c_x.tag, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.c_x, c_x), p_index);
		paramQueue = outParam->addParameterData (c_y.tag, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.c_y, c_y), p_index);
		paramQueue = outParam->addParameterData (c_z.tag, q_index);
		if (paramQueue)
			paramQueue->addPoint (p_offset, rangeParameter:: toNormalized (gp.c_z, c_z), p_index);
	}

	// update of reflected waves parameters
	edge [0] = gp.r_x - gp.c_x;
	edge [1] = - gp.c_x;
	edge [2] = gp.r_y - gp.c_y;
	edge [3] = - gp.c_y;
	edge [4] = gp.r_z - gp.c_z;
	edge [5] = - gp.c_z;
}

// parameters smoothing
void SODSPparam:: param_smoothing ()
{
	if (smoothing) {
		cos_theta_o += delta_cos_theta_o;
		decay_L += delta_decay_L;
		decay_R += delta_decay_R;
		for (int i = 0; i < 6; i++) {
			v_cos_theta_o [i] += v_delta_cos_theta_o [i];
			v_decay [i] += v_delta_decay [i];
		}

	}
}

} // namespace suzumushi