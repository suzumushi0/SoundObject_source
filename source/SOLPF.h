//
// Copyright (c) 2021 suzumushi
//
// 2021-8-8		SOLPF.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

namespace suzumushi {

// Simple LPF (Butterworth, -6dB/oct.)
// fc: cut-off frequency [KHz]

template <typename TYPE>
class SOLPF {
public:
	void setup (const double fc, const double SR);
	TYPE process (const TYPE xn);
	void reset ();
private:
	TYPE z1 {0.0};			// delay register
	TYPE a1 {0.0};			// feedback filter coefficient
	TYPE b0 {0.0};			// feedforward filter coefficient
};

template <typename TYPE>
void SOLPF <TYPE>:: setup (const double fc, const double SR)
{
	if (fc == fc_max)
		a1 = 0.0;
	else
		a1 = exp (- fc * 1'000.0 * 2.0 * pi / SR);
	b0 = 1.0 - a1;
}

template <typename TYPE>
TYPE SOLPF <TYPE>:: process (const TYPE xn)
{
	return (z1 = a1 * z1 + b0 * xn);
}

template <typename TYPE>
inline void SOLPF <TYPE>:: reset ()
{
	z1 = 0.0;
}

} // namespace suzumushi