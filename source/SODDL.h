//
// Copyright (c) 2021 suzumushi
//
// 2021-8-8		SODDL.h
//
// Licensed under Creative Commons Attribution-NonCommercial-ShareAlike 4.0 (CC BY-NC-SA 4.0).
//
// https://creativecommons.org/licenses/by-nc-sa/4.0/
//

#pragma once

namespace suzumushi {

// Digtal Delay Line

template <typename TYPE, unsigned int N>
class SODDL {
public:
	void add (const int at, const TYPE val);
	void push (const TYPE val);
	TYPE read ();
	TYPE read (const int at);
	void reset ();
private:
	TYPE delay_line [N] {};
	TYPE* head = delay_line;
};

template <typename TYPE, unsigned int N>
void SODDL <TYPE, N>:: add (const int at, const TYPE val)
{
	TYPE* at_ptr = head + at;
	if (at_ptr >= delay_line + N)
		at_ptr -= N;
	*at_ptr += val;
}

template <typename TYPE, unsigned int N>
void SODDL <TYPE, N>:: push (const TYPE val)
{
	*head++ = val;						// head points the last data
	if (head == delay_line + N)
		head = delay_line;
}

template <typename TYPE, unsigned int N>
TYPE SODDL <TYPE, N>:: read ()
{
	TYPE ret = *head;
	*head++ = 0.0;						// read then clear
	if (head == delay_line + N)
		head = delay_line;
	return (ret);
}

template <typename TYPE, unsigned int N>
TYPE SODDL <TYPE, N>:: read (const int at)
{
	TYPE* at_ptr = head + at;
	if (at_ptr >= delay_line + N)
		at_ptr -= N;
	return (*at_ptr);
}

template <typename TYPE, unsigned int N>
void SODDL <TYPE, N>:: reset ()
{
	for (head = delay_line; head < delay_line + N; head++) {
		*head = 0.0;
	}
	head = delay_line;
}

} // suzumushi

