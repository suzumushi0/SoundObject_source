//
// Copyright (c) 2021 suzumushi
//
// 2021-8-8		SOconfig.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

namespace suzumushi {

constexpr int frame_len {1'000};						// frame length [samples]
constexpr double min_dist {0.5};						// minimum limit of decay distance [m] 
constexpr double max_speed {0.1};						// maximum speed of the acoustic source (c * max_speed) [m/s]
constexpr double max_side_len {20.0};					// maximum side length of the reverberation room [m]

}

