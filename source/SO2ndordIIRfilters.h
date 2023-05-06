//
// Copyright (c) 2021-2023 suzumushi
//
// 2023-4-13		SO2ndordIIRfilters.h
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
	virtual TYPE process (const TYPE xn);
	virtual void reset ();
protected:
	TYPE za [2] = {0.0, 0.0};			// delay registers for feedback filter
	TYPE zb [2] = {0.0, 0.0};			// delay registers for feedforward filter
	TYPE a [3] = {0.0, 0.0, 0.0};		// feedback filter coefficients. a [0] is unused
	TYPE b [3] = {0.0, 0.0, 0.0};		// feedforward filter coefficients
};

template <typename TYPE>
TYPE SO2ndordIIRfilter <TYPE>:: process (const TYPE xn)
{
	TYPE yn = b [0] * xn + b [1] * zb [0] + b [2] * zb [1] + a [1] * za [0] + a [2] * za [1];
	za [1] = za [0];
	za [0] = yn;
	zb [1] = zb [0];
	zb [0] = xn;
	return (yn);
}

template <typename TYPE>
inline void SO2ndordIIRfilter <TYPE>:: reset ()
{
	za [0] = za [1] = zb [0] = zb [0] = 0.0;
}


// sphere scattering effect filter 

template <typename TYPE>
class SOsphere_scattering: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE cT, const TYPE a);
	TYPE process (const TYPE xn, const TYPE cos_thetao);
	void process (const TYPE xn, const TYPE sin_phiL, TYPE& para, TYPE& cross);
};

template <typename TYPE>
void SOsphere_scattering <TYPE>:: setup (const TYPE cT, const TYPE a)
{
	TYPE omega_a = tan (cT / (2.0 * a));								// omega_a = tan (c * T / (2 * a))
	TYPE omega_a_2 = 2.0 * pow (omega_a, 2.0);							// omega_a_2 = 2 * omega_a^2
	this->a [0] = omega_a_2 + 2.0 * omega_a + 1.0;						// a [0] = 2 * omega_a^2 + 2 * omega_a + 1
	this->a [1] = -2.0 * (omega_a_2 - 1.0) / this->a [0];				// a [1] = -2 * (2 * omega_a^2 - 1) / a [0]
	this->a [2] = - (omega_a_2 - 2.0 * omega_a + 1.0) / this->a [0];	// a [2] = -(2 * omega_a^2 - 2 * omega_a + 1) / a [0]
	this->b [0] = (omega_a + 1.0) / this->a [0];						// b [0] = (omega_a + 1) / a [0]
	this->b [1] = -2.0 / this->a [0];									// b [1] = -2 / a [0]
	this->b [2] = - (omega_a - 1.0) / this->a [0];						// b [2] = -(omega_a - 1) / a [0]
}

template <typename TYPE>
inline TYPE SOsphere_scattering <TYPE>:: process (const TYPE xn, const TYPE cos_thetao)
{
	return (xn + SO2ndordIIRfilter <TYPE>:: process (xn) * cos_thetao);
}

template <typename TYPE>
void SOsphere_scattering <TYPE>:: process (const TYPE xn, const TYPE sin_phiL, TYPE& para, TYPE& cross)
{
	TYPE scatter = SO2ndordIIRfilter <TYPE>:: process (xn) * sin_phiL;
	para = (3.0 * xn - scatter) / 4.0;
	cross = (scatter - xn) / 4.0;
}

// LPF with pass through mode

template <typename TYPE, TYPE FC_MAX = 20'000.0>
class SOLPF: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE SR, const TYPE fc, const TYPE Q = 0.5);
	TYPE process (const TYPE xn) override;
private:
	bool pass_through {false};			// pass through mode
};

template <typename TYPE, TYPE FC_MAX>
void SOLPF <TYPE, FC_MAX>:: setup (const TYPE SR, const TYPE fc, const TYPE Q)
{
	if (fc >= FC_MAX) {
		pass_through = true;
		this->reset ();
	} else {
		pass_through = false;
		TYPE omega_a = tan (pi * fc / SR);
		TYPE omega_a_2 = pow (omega_a, 2.0);							// omega_a_2 = omega_a^2
		TYPE omega_a_Q = omega_a / Q;									// omega_a_Q = omega_a / Q
		this->a [0] = omega_a_2 + omega_a_Q + 1.0;						// a [0] = omega_a^2 + omega_a / Q + 1
		this->a [1] = -2.0 * (omega_a_2 - 1.0) / this->a [0];			// a [1] = -2 * (omega_a^2 - 1) / a [0]
		this->a [2] = - (omega_a_2 - omega_a_Q + 1.0) / this->a [0];	// a [2] = -(omega_a^2 - omega_a / Q + 1) / a [0]
		this->b [0] = this->b [2] = omega_a_2 / this->a [0];			// b [0] = b_[2] = omega_a^2 / a [0]
		this->b [1] = 2.0 * omega_a_2 / this->a [0];					// b [1] = 2 * omega_a^2 / a [0]
	}
}

template <typename TYPE, TYPE FC_MAX>
TYPE SOLPF <TYPE, FC_MAX>:: process (const TYPE xn)
{
	if (pass_through) 
		return (xn);
	else
		return (SO2ndordIIRfilter <TYPE>:: process (xn));
}

// HPF with initial mute mode (MUTE_LEN [ms])

template <typename TYPE, int MUTE_LEN = 100>	
class SOHPF: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE SR, const TYPE fc, const TYPE Q = 0.5);
	TYPE process (const TYPE xn) override;
	void reset () override;
private:
	bool mute {true};
	int mute_timer {0};
};

template <typename TYPE, int MUTE_LEN>
void SOHPF <TYPE, MUTE_LEN>:: setup (const TYPE SR, const TYPE fc, const TYPE Q)
{
	if (mute && mute_timer == 0)
		mute_timer = (int)(SR + 0.5) * MUTE_LEN / 1000;

	TYPE omega_a = tan (pi * fc / SR);
	TYPE omega_a_2 = pow (omega_a, 2.0);								// omega_a_2 = omega_a^2
	TYPE omega_a_Q = omega_a / Q;										// omega_a_Q = omega_a / Q
	this->a [0] = omega_a_2 + omega_a_Q + 1.0;							// a [0] = omega_a^2 + omega_a / Q + 1
	this->a [1] = -2.0 * (omega_a_2 - 1.0) / this->a [0];				// a [1] = -2 * (omega_a^2 - 1) / a [0]
	this->a [2] = - (omega_a_2 - omega_a_Q + 1.0) / this->a [0];		// a [2] = -(omega_a^2 - omega_a / Q + 1) / a [0]
	this->b [0] = this->b [2] = 1.0 / this->a [0];						// b [0] = b_[2] = 1 / a [0]
	this->b [1] = -2.0 / this->a [0];									// b [1] = -2 / a [0]
}

template <typename TYPE, int MUTE_LEN>
TYPE SOHPF <TYPE, MUTE_LEN>:: process (const TYPE xn)
{
	TYPE yn = SO2ndordIIRfilter <TYPE>:: process (xn);
	if (mute) {
		if (--mute_timer == 0)
			mute = false;
		return (0.0);
	} else
		return (yn);
}

template <typename TYPE, int MUTE_LEN>
void SOHPF <TYPE, MUTE_LEN>:: reset ()
{
	mute = true;
	SO2ndordIIRfilter <TYPE>:: reset ();
}

// BPF (constant peak gain)

template <typename TYPE>
class SOBPF_G: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE SR, const TYPE fc, const TYPE Q);
};

template <typename TYPE>
void SOBPF_G <TYPE>:: setup (const TYPE SR, const TYPE fc, const TYPE Q)
{
	TYPE omega_a = tan (pi * fc / SR);
	TYPE omega_a_2 = pow (omega_a, 2.0);								// omega_a_2 = omega_a^2
	TYPE omega_a_Q = omega_a / Q;										// omega_a_Q = omega_a / Q
	this->a [0] = omega_a_2 + omega_a_Q + 1.0;							// a [0] = omega_a^2 + omega_a / Q + 1
	this->a [1] = -2.0 * (omega_a_2 - 1.0) / this->a [0];				// a [1] = -2 * (omega_a^2 - 1) / a [0]
	this->a [2] = - (omega_a_2 - omega_a_Q + 1.0) / this->a [0];		// a [2] = -(omega_a^2 - omega_a / Q + 1) / a [0]
	this->b [0] = omega_a_Q / this->a [0];								// b [0] = omega_a / Q / a [0]
	this->b [1] = 0.0;													// b [1] = 0
	this->b [2] = - this->b [0];										// b [2] = -omega_a / Q / a [0]
}

// BPF (constant peak bandwidth)

template <typename TYPE>
class SOBPF_B: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE SR, const TYPE fc, const TYPE Q);
};

template <typename TYPE>
void SOBPF_B <TYPE>:: setup (const TYPE SR, const TYPE fc, const TYPE Q)
{
	TYPE omega_a = tan (pi * fc / SR);
	TYPE omega_a_2 = pow (omega_a, 2.0);								// omega_a_2 = omega_a^2
	TYPE omega_a_Q = omega_a / Q;										// omega_a_Q = omega_a / Q
	this->a [0] = omega_a_2 + omega_a_Q + 1.0;							// a [0] = omega_a^2 + omega_a / Q + 1
	this->a [1] = -2.0 * (omega_a_2 - 1.0) / this->a [0];				// a [1] = -2 * (omega_a^2 - 1) / a [0]
	this->a [2] = - (omega_a_2 - omega_a_Q + 1.0) / this->a [0];		// a [2] = -(omega_a^2 - omega_a / Q + 1) / a [0]
	this->b [0] = omega_a / this->a [0];								// b [0] = omega_a / a [0]
	this->b [1] = 0.0;													// b [1] = 0
	this->b [2] = - this->b [0];										// b [2] = -omega_a / a [0]
}

// BPF (constant gain) experiment

template <typename TYPE>
class SOBPF_E: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE SR, const TYPE fc, const TYPE Q);
};

template <typename TYPE>
void SOBPF_E <TYPE>:: setup (const TYPE SR, const TYPE fc, const TYPE Q)
{
	TYPE omega_a = tan (pi * fc / SR);
	TYPE omega_a_2 = pow (omega_a, 2.0);								// omega_a_2 = omega_a^2
	TYPE omega_a_Q2 = omega_a / pow (Q, 2.0);							// omega_a_Q2 = omega_a / Q^2
	this->a [0] = omega_a_2 + omega_a_Q2 + 1.0;							// a [0] = omega_a^2 + omega_a / Q^2 + 1
	this->a [1] = -2.0 * (omega_a_2 - 1.0) / this->a [0];				// a [1] = -2 * (omega_a^2 - 1) / a [0]
	this->a [2] = - (omega_a_2 - omega_a_Q2 + 1.0) / this->a [0];		// a [2] = -(omega_a^2 - omega_a / Q^2 + 1) / a [0]
	this->b [0] = omega_a / Q / this->a [0];							// b [0] = omega_a / Q / a [0]
	this->b [1] = 0.0;													// b [1] = 0
	this->b [2] = - this->b [0];										// b [2] = -omega_a / Q / a [0]
}

// PEF (peaking equalizer filter) [A: sqrt (peak gain)]

template <typename TYPE>
class SOPFE: public SO2ndordIIRfilter <TYPE> {
public:
	void setup (const TYPE SR, const TYPE fc, const TYPE Q, const TYPE A);
};

template <typename TYPE>
void SOPFE <TYPE>:: setup (const TYPE SR, const TYPE fc, const TYPE Q, const TYPE A)
{
	TYPE omega_a = tan (pi * fc / SR);
	TYPE omega_a_2 = pow (omega_a, 2.0);								// omega_a_2 = omega_a^2
	TYPE omega_a_AQ = omega_a / (A * Q);								// omega_a_AQ = omega_a / (AQ)
	TYPE omega_a_A_Q = omega_a * A / Q;									// omega_a_A_Q = omega_a * A / Q
	this->a [0] = omega_a_2 + omega_a_AQ + 1.0;							// a [0] = omega_a^2 + omega_a / (AQ) + 1
	this->a [2] = - (omega_a_2 - omega_a_AQ + 1.0) / this->a [0];		// a [2] = -(omega_a^2 - omega_a / (AQ) + 1) / a [0]
	this->b [0] = (omega_a_2 + omega_a_A_Q + 1.0) / this->a [0];		// b [0] = (omega_a^2 + omega_a * A / Q + 1) / a [0]
	this->b [2] = (omega_a_2 - omega_a_A_Q + 1.0) / this->a [0];		// b [2] = (omega_a^2 - omega_a * A / Q + 1) / a [0]
	this->b [1] = 2.0 * (omega_a_2 - 1.0) / this->a [0];				// b [1] = 2 * (omega_a^2 - 1) / a [0]
	this->a [1] = - this->b [1];										// a [1] = -2 * (omega_a^2 - 1) / a [0]
}

}	// namespace suzumushi
