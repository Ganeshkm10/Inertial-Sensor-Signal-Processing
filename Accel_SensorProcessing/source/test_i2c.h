/*
 * test_i2c.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Windows
 */

#ifndef TEST_I2C_H_
#define TEST_I2C_H_


#include <LOG_defs.h>
#include "i2c.h"
#include "MKL25Z4.h"

/*
​ * ​ ​ Test suite for I2C Module
 *
​ * ​ ​ parameter :​ void
 *
 *   Return : ​ void
 */
void i2c_test_setup();

/*
​ * ​ ​ ​function to test I2C Module
 *
 * ​ ​ parameter :​ void
 *
 *   Return : ​ void
 */
void test_i2c_mma(void);



#endif /* TEST_I2C_H_ */
