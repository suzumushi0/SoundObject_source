//
// Copyright (c) 2021-2024 suzumushi
//
// 2024-3-10		SOudsampling.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

#include "SODDL.h"
#include "SOconfig.h"
#ifdef _MSC_VER			// Visual C++
#include <numbers>
using std::numbers::pi;
#endif


namespace suzumushi {

// Polyphase decomposition-based up and down sampling.

// configurations 
constexpr int IR_L {67};						// Length of impulse response (prior to upsampling)
constexpr int ODL_LEN {32'768};					// Length of output delay line for direct wave (2^15)

// constant values
constexpr int N {frame_len};					// Constant for Doppler effect (fo / fs = N / M)
constexpr int IR_LEN {(IR_L - 1) * N + 1};		// Logical length of impulse response
constexpr int IR_CENTER {(IR_L - 1) * N / 2};	// Ceter of impulse response


template <typename TYPE>
class SOudsampling {
public:
	void setup (const TYPE inv_cT, const TYPE distance);
	TYPE process (const TYPE xn, const TYPE decay);
	void reset ();
	SOudsampling ();
private:
	// IR_lookup functions and variables
	TYPE IR (const int ir_at) const;							// Common IR lookup function
	TYPE up_IR (const int ir_at, const int offset) const;		// IR lookup function for N > M
	TYPE dn_IR (const int ir_at, const int m) const;			// IR lookup function for M > N
	static inline TYPE IR_TBL [IR_CENTER + 1];					// Impulse response table
	int IR_TBL_LEN {IR_LEN};									// Logical length of impulse response table
	int TBL_OFFSET {0};											// IR lookup offset for N > M 

	// variables for polyphase decomposition
	SODDL <TYPE, IR_L> IDL;										// Input delay line
	SODDL <TYPE, ODL_LEN> ODL;									// Output delay line
	int odl_at {0};												// Delay of output data
	int M {N};													// Parameter for Doppler effect (fo / fs = N / M)
	int phase {0};												// IR lookup phase for polyphase decomposition
	bool first_frame {true};									// The first frame after reset() or initialization
};

template <typename TYPE>
SOudsampling <TYPE>:: SOudsampling ()
{
	if (IR_TBL [IR_CENTER] == 0.0) {
		// sinc function 
		for (int i = 0; i < IR_CENTER; i++) {
			TYPE x = pi * ((TYPE)i - IR_CENTER) / N;
			IR_TBL [i] = sin (x) / x;
		}
		IR_TBL [IR_CENTER] = 1.0;
		// Blackman window
		for (int i = 0; i <= IR_CENTER; i++)
			IR_TBL [i] *= 0.42 - 0.5 * cos (pi * i / IR_CENTER) + 0.08 *  cos (2.0 * pi * i / IR_CENTER);
		// normalization
		TYPE sum = 0.0;
		for (int i = 0; i < IR_CENTER; i++)
			sum += IR_TBL [i];
		sum = (sum * 2.0 + IR_TBL [IR_CENTER]) / N;
		for (int i = 0; i <= IR_CENTER; i++)
			IR_TBL [i] /= sum;
	}
}

template <typename TYPE>
inline TYPE SOudsampling <TYPE>:: IR (const int ir_at) const
{
	if (ir_at <= IR_CENTER)
		return (IR_TBL [ir_at]);
	else
		return (IR_TBL [IR_LEN - 1 - ir_at]);
}

template <typename TYPE>
inline TYPE SOudsampling <TYPE>:: up_IR (const int ir_at, const int offset) const
{
	return (SOudsampling <TYPE>:: IR (ir_at + offset));	
}

template <typename TYPE>
inline TYPE SOudsampling <TYPE>:: dn_IR (const int ir_at, const int m) const
{
	return (SOudsampling <TYPE>:: IR ((ir_at * N * 2 + m) / (m * 2)));	// round off
}

template <typename TYPE>
void SOudsampling <TYPE>:: setup (const TYPE inv_cT, const TYPE distance)
{
	if (first_frame) {
		first_frame = false;
		M = N;
		odl_at = distance * inv_cT + 0.5;
	} else {
		int delay = distance * inv_cT + 0.5;
		M = N - (odl_at - delay);
	}
	IR_TBL_LEN  = (IR_L - 1) * M + 1;
	TBL_OFFSET = (IR_L - 1) * (N - M) / 2;
}

template <typename TYPE>
TYPE SOudsampling <TYPE>:: process (const TYPE xn, const TYPE decay)
{
	IDL.enqueue (xn);
	if (M > N) {
		TYPE sum = 0.0;
		int idl_at = IR_L - 1;
		for (int ir_at = phase; ir_at < IR_TBL_LEN; ir_at += M)
			sum += IDL.read (idl_at--) * dn_IR (ir_at, M);
		ODL.add (odl_at, sum * decay);
		phase += N;
		if (phase < M) {
			sum = 0.0;
			idl_at = IR_L - 1;
			for (int ir_at = phase; ir_at < IR_TBL_LEN; ir_at += M)
				sum += IDL.read (idl_at--) * dn_IR (ir_at, M);
			ODL.add (++odl_at, sum * decay);
			phase += N;
		}
		phase -= M;
	} else if (M < N) {
		if (phase < M) {
			TYPE sum = 0.0;
			int idl_at = IR_L - 1;
			for (int ir_at = phase; ir_at < IR_TBL_LEN; ir_at += M)
				sum += IDL.read (idl_at--) * up_IR (ir_at, TBL_OFFSET);
			ODL.add (odl_at, sum * decay);
			phase += N;
		} else {
			odl_at--;
		}
		phase -= M;
	} else {		// M == N
		ODL.add (odl_at, IDL.read ((IR_L - 1) / 2) * decay);
	}
	return (ODL.dequeue ());
}

template <typename TYPE>
void SOudsampling <TYPE>:: reset ()
{
	ODL.reset ();
	IDL.reset ();
	first_frame = true;
	phase = 0;
}

} // namespace suzumushi