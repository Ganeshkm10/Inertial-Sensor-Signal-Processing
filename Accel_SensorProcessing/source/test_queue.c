/*
 * test_queue.c
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */


#include <LOG_defs.h>
#include "test_queue.h"
#include "uart.h"

#define max(x,y) ((x) > (y) ? (x) : (y))

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

Q_T Q;

/*
​ * ​ ​ Function to test Circular buffer
 *
​ * ​ ​ parameter :​ void
 *
 *   Retrun : ​ void
 */
void queue_test_setup() {
	char *str =
	     "To be, or not to be: that is the question: \n"
	      "Whether 'tis nobler in the mind to suffer \n"
	      "The slings and arrows of outrageous fortune, \n"
	      "Or to take arms against a sea of troubles, \n"
	      "And by opposing end them? To die, to sleep— \n"
	      "No more—and by a sleep to say we end \n"
	      "The heart-ache and the thousand natural shocks \n"
	      "That flesh is heir to, 'tis a consummation \n"
	      "Devoutly to be wish'd. To die, to sleep; \n"
	      "To sleep: perchance to dream: ay, there's the rub; \n"
	      "For in that sleep of death what dreams may come \n"
	      "When we have shuffled off this mortal coil, \n"
	      "Must give us pause. \n"
	    ;

	char temp_str[1024];
	const int limit = Q_Capacity(&Q);


	Q_Init(&Q);

	test_equal(Q_Length(&Q), 0);
	test_equal(Q_Dequeue(&Q, temp_str , limit), 0);
	test_equal(Q_Dequeue(&Q, temp_str , 1), 0);
	test_assert(!Q_Full(&Q));

	test_equal(Q_Enqueue(&Q, str , 5), 5);
	test_assert(!Q_Full(&Q));
	test_equal(Q_Length(&Q), 5);
	test_equal(Q_Dequeue(&Q, temp_str , 5), 5);
	test_equal(strncmp(temp_str, str, 5), 0);
	test_equal(Q_Length(&Q), 0);
	test_assert(!Q_Full(&Q));


	test_equal(Q_Enqueue(&Q, str , 10), 10);
	test_equal(Q_Length(&Q), 10);
	test_equal(Q_Dequeue(&Q, temp_str , 5), 5);
	test_equal(Q_Length(&Q), 5);
	test_equal(Q_Dequeue(&Q, temp_str+5 , 5), 5);
	test_equal(Q_Length(&Q), 0);
	test_equal(strncmp(temp_str, str, 10), 0);
	test_assert(!Q_Full(&Q));

	test_equal(Q_Enqueue(&Q, str , limit), limit);
	test_equal(Q_Length(&Q), limit);
	test_assert(Q_Full(&Q));
	test_equal(Q_Enqueue(&Q, str , 1), 0);
	test_assert(Q_Full(&Q));
	test_equal(Q_Dequeue(&Q, temp_str , limit), limit);
	test_assert(!Q_Full(&Q));
	test_equal(Q_Length(&Q), 0);
	test_equal(strncmp(temp_str, str, limit), 0);

	test_equal(Q_Enqueue(&Q, str , 25), 25);
	test_assert(!Q_Full(&Q));
	test_equal(Q_Length(&Q), 25);
	test_equal(Q_Dequeue(&Q, temp_str , 23), 23);
	test_assert(!Q_Full(&Q));
	test_equal(Q_Length(&Q), 2);
	test_equal(strncmp(temp_str, str, 23), 0);

	int val = (limit-2) / 4;
	for(int i = 0; i<4; i++) {
		test_equal(Q_Enqueue(&Q, str + i*val , val), val);
		test_equal(Q_Length(&Q), (i+1)*val +2);
	}
	test_equal(Q_Length(&Q), 4*val +2);
	test_equal(Q_Dequeue(&Q, temp_str , 2), 2);
	test_equal(strncmp(temp_str, str+23, 2), 0);

	for(int i=0; i<val*4; i++) {
		test_equal(Q_Dequeue(&Q, temp_str+i , 1), 1);
		test_equal(Q_Length(&Q), val*4 -i -1);
	}

	test_equal(strncmp(temp_str, str, val*4), 0);
	test_equal(Q_Enqueue(&Q, str , 50), 50);
	test_equal(Q_Enqueue(&Q, str+50 , limit), limit-50);
	test_equal(Q_Length(&Q), limit);
	test_assert(Q_Full(&Q));
	test_equal(Q_Dequeue(&Q, temp_str , limit), limit);
	test_equal(Q_Length(&Q), 0);
	test_equal(strncmp(temp_str, str, limit), 0);

	test_equal(Q_Enqueue(&Q, str , 0), 0);
	test_equal(Q_Length(&Q), 0);

}


/*
​ * ​ Function to track testing
 *
​ * ​ ​ parameter ​: void
 *
 *   return​ : ​ void
 */
void test_queue()
{
  g_tests_passed = 0;
  g_tests_total = 0;
  g_skip_tests = 0;

  queue_test_setup();
  LOG("\r\n");
  LOG(" Queue test Function is Running ");
  LOG("\r\n %s: passed %d/%d test cases \r\n", __FUNCTION__, g_tests_passed, g_tests_total);

}
