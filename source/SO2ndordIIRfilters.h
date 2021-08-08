//
// Copyright (c) 2021 suzumushi
//
// 2021-8-8		SO2ndordIIRfilters.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

namespace suzumushi {

// second-order IIR filter (biquad filter)

template <typename TYPE>
class SO2ndordIIRfilter {
public:
	static void setup (const TYPE* cof_a, const TYPE* cof_b);
	virtual TYPE process (const TYPE xn);
	void reset ();
private:
	TYPE z [3] = {0.0, 0.0, 0.0};	// delay registers. z [n] represents z^(-n)
	static TYPE a [3];				// feedback filter coefficients. a [0] is unused
	static TYPE b [3];				// feedforward filter coefficients
};

template <typename TYPE>
TYPE SO2ndordIIRfilter <TYPE>:: a [3] = {0.0, 0.0, 0.0};

template <typename TYPE>
TYPE SO2ndordIIRfilter <TYPE>:: b [3] = {0.0, 0.0, 0.0};

template <typename TYPE>
void SO2ndordIIRfilter <TYPE>:: setup (const TYPE* cof_a, const TYPE* cof_b)
{
	a [1] = *++cof_a;	// a [1] = cof_a [1]
	a [2] = *++cof_a;	// a [2] = cof_a [2]
	b [0] = *cof_b++;	// b [0] = cof_b [0]
	b [1] = *cof_b++;	// b [1] = cof_b [1]
	b [2] = *cof_b;		// b [2] = cof_b [2]
}

template <typename TYPE>
TYPE SO2ndordIIRfilter <TYPE>:: process (const TYPE xn)
{
	z [0] = xn + a [1] * z [1] + a [2] * z [2];
	TYPE yn = b [0] * z [0] + b [1] * z [1] + b [2] * z [2];
	z [2] = z [1];
	z [1] = z [0];
	return (yn);
}

template <typename TYPE>
inline void SO2ndordIIRfilter <TYPE>:: reset ()
{
	z [1] = z [2] = 0.0;
}


// sphere scattering effect filter 

template <typename TYPE>
class SOsphere_scattering: public SO2ndordIIRfilter <TYPE> {
public:
	static void setup (const TYPE cT, const TYPE a);
	TYPE process (const TYPE xn, const TYPE cos_thetao);
};

template <typename TYPE>
void SOsphere_scattering <TYPE>:: setup (const TYPE cT, const TYPE a)
{
	TYPE omega_a = tan (cT / (2.0 * a));				// omaga_a = tan (c * T / (2 * a))
	TYPE omega_a_2 = 2.0 * pow (omega_a, 2.0);			// omega_a_2 = 2 * omega_a^2
	TYPE divisor = omega_a_2 + 2.0 * omega_a + 1.0;		// divisor = 2 * omega_a^2 + 2 * omaga_a + 1

	TYPE cof_a [3], cof_b [3];
	cof_a [0] = 0.0;									// cof_a [0]: unused
	cof_a [1] = -2.0 * (omega_a_2 - 1.0) / divisor;		// cof_a [1] = -2 * (2 * omega_a^2 - 1) / divisor
	cof_a [2] = - (omega_a_2 - 2.0 * omega_a + 1.0) / divisor;
														// cof_a [2] = -(2 * omega_a^2 - 2 * omaga_a + 1) / divisor
	cof_b [0] = (omega_a + 1.0) / divisor;				// cof_b [0] = (omega_a + 1) / divisor
	cof_b [1] = -2.0 / divisor;							// cof_b [1] = -2 / divisor
	cof_b [2] = - (omega_a - 1.0) / divisor;			// cof_b [2] = -(omega_a - 1) /divisor
	SO2ndordIIRfilter <TYPE>:: setup (cof_a, cof_b);
}

template <typename TYPE>
inline TYPE SOsphere_scattering <TYPE>:: process (const TYPE xn, const TYPE cos_thetao)
{
	return (xn + SO2ndordIIRfilter <TYPE>:: process (xn) * cos_thetao);
}

}	// namespace suzumushi
