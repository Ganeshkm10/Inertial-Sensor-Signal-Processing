/*
 * utils.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "MKL25Z4.h"


#define ENDIANSWAP_16(x)		( (((x) >> 8) & 0x00FF) | (((x)& 0x00FF) << 8) )
#define ENDIANSWAP_32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

typedef enum {
	FROM_LITTLE_ENDIAN = 0,
	FROM_BIG_ENDIAN = 1
} endian_t;

/**
 * @brief Test the Endianness of the data generated from the sensor
 *
 * @param: test_var - Sample data Unsigned 32 bit
 *
 * @return: If Little Endian: Return True
 * 			If Big Endian : Return False
 */
static unsigned int test_endian( uint32_t test_var)
{
    unsigned char *test_endian = (unsigned char*)&test_var;

    return (test_endian[0] == 0);
}

/**
 * @brief Determines if endian correction to machine endianness is required given a source endianness
 *
 * @param[in] None
 *
 * @return zero if endianness is same, nonzero otherwise
 */

static inline uint8_t endianCorrectionRequired()
{
	int test_var = 1;
	unsigned char *test_endian = (unsigned char*)&test_var;

	return (test_endian[0] == 0);
}

/**
 * @brief Converts from a given endianness to machine endianness
 *
 * @param[in] value The value to convert
 *
 * @param[in] sourcEndianness The endianness of the value
 */
static inline uint16_t endianCorrect16(register uint16_t value, const register endian_t sourceEndianness)
{
	/* if in little endian mode */
	if ( test_endian( (uint32_t) value) )
	{
		return ENDIANSWAP_16(value);
	}

	/* finally or if in big endian mode */
	return value;
}

/**
 * @brief Converts from a given endianness to machine endianness
 *
 * @param[in] value The value to convert
 *
 * @param[in] sourcEndianness The endianness of the value
 */
static inline uint32_t endianCorrect32(register uint32_t value, const register endian_t sourceEndianness)
{
	/* if in little endian mode */
	if (test_endian( (uint32_t) value) )
	{
		return ENDIANSWAP_32(value);
	}

	/* finally or if in big endian mode */
	return value;
}



#endif /* UTILS_H_ */
