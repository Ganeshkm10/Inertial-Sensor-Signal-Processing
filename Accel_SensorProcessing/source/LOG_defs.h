/*
 * LOG_defs.h
 *
 *  Created on: 05-Dec-2021
 *      Author: Ganesh KM
 */

#ifndef LOG_DEFS_H_
#define LOG_DEFS_H_

#include "stdio.h"

#ifdef DEBUG
	#define LOG printf
#else
	#define LOG(...)
#endif


#endif /* LOG_DEFS_H_ */
