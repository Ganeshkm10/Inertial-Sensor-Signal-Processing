/*
 * test_queue.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef TEST_QUEUE_H_
#define TEST_QUEUE_H_

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "MKL25Z4.h"
#include "queue.h"

/*
​ * ​ ​ Function to test Circular buffer
 *
​ * ​ ​ parameter :​ void
 *
 *   Retrun : ​ void
 */
void queue_test_setup();

/*
​ * ​ Function to track testing
 *
​ * ​ ​ parameter ​: void
 *
 *   return​ : ​ void
 */
void test_queue();


#endif /* TEST_QUEUE_H_ */
