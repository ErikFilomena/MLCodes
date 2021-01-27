#pragma once

#include "Defines.h"
#include "Utils.h"

/**
* Interface for RNG and LDS generators
**/
template <class NBR>
struct Generator{
	/**
	* Generates a sequence of random numbers or a low discrepancy sequence 
	* @param dst the destination block of memoy (must be of size len*sizeof(NBR) * dim)
	**/
	virtual void GenerateSequence(NBR* dst, size_t len) = 0;
	/**
	* Generates a sequence of random numbers in a range LDS generators should not implement this
	* @param dst the destination block of memoy (must be of size len*sizeof(NBR) * dim)
	**/
	virtual void GenerateRange(NBR* dst, size_t len, NBR a, NBR b) = 0;
	/**
	* Generates a single random, number. LDS generators should not implement this
	**/
	virtual NBR GenRnd() = 0;
	/**
	* Generates a single random, number in a range. LDS generators should not implement this
	**/
	virtual NBR GenRange(NBR a, NBR b) = 0;
};