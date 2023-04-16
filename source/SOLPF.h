//
// Copyright (c) 2021-2023 suzumushi
//
// 2023-4-15		SOLPF.h
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
class SOSLPF {
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
void SOSLPF <TYPE>:: setup (const double f, const double SR)
{
	if (f == fc.max)
		a1 = 0.0;
	else
		a1 = exp (- f * 1'000.0 * 2.0 * pi / SR);
	b0 = 1.0 - a1;
}

template <typename TYPE>
TYPE SOSLPF <TYPE>:: process (const TYPE xn)
{
	return (z1 = a1 * z1 + b0 * xn);
}

template <typename TYPE>
inline void SOSLPF <TYPE>:: reset ()
{
	z1 = 0.0;
}

} // namespace suzumushi