/*
 * test_i2c.c
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#include <LOG_defs.h>
#include "test_i2c.h"
#include "mma8451q.h"
#include "assert.h"

#define test_assert(value) {                                            \
  g_tests_total++;                                                      \
  if (!g_skip_tests) {                                                  \
    if (value) {                                                        \
      g_tests_passed++;                                                 \
    } else {                                                            \
      printf("ERROR: test failure at line %d\n", __LINE__);             \
      g_skip_tests = 1;                                                 \
    }                                                                   \
  }                                                                     \
}

#define test_equal(value1, value2) {                                    \
  g_tests_total++;                                                      \
  if (!g_skip_tests) {                                                  \
    long res1 = (long)(value1);                                         \
    long res2 = (long)(value2);                                         \
    if (res1 == res2) {                                                 \
      g_tests_passed++;                                                 \
    } else {                                                            \
      printf("ERROR: test failure at line %d: %ld != %ld\n", __LINE__, res1, res2); \
      g_skip_tests = 1;                                                 \
    }                                                                   \
  }                                                                     \
}


static int g_tests_passed = 0;
static int g_tests_total = 0;
static int g_skip_tests = 0;

void i2c_test_setup() {

	mma8451q_acc_t val;
	MMA8451Q_InitializeData(&val);
	read_full_xyz(&val);

	uint8_t id = I2C_ReadRegister(MMA8451Q_I2CADDR, MMA8451Q_REG_WHOAMI); //Address check
	test_equal(id, 0x1A);
	assert(id == 0x1A);

	id = I2C_ReadRegister(0x00, 0x00); // Negative test case for Invalid slave address
	test_equal(id, 255);
	assert(id == 255);

	test_equal(1, val.x != 0);
	test_equal(1, val.y != 0);
	test_equal(1, val.z != 0);

}

void test_i2c_mma() {

	i2c_test_setup();
	LOG("\r\n");
	LOG("\r\n I2C Test Function is Running ...");
	LOG("\r\n There is possibility that all test cases may not pass which is still acceptable ");
	LOG("\r\n %s: passed %d/%d test cases", __FUNCTION__, g_tests_passed, g_tests_total);
}
