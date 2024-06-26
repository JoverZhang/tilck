/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * This is a TEMPLATE. The actual config file is generated by CMake and stored
 * in <BUILD_DIR>/tilck_gen_headers/.
 */

#pragma once

/* ------ Value-based config variables -------- */
#define TIMER_HZ               @TIMER_HZ@

/* --------- Boolean config variables --------- */
#cmakedefine01 KRN_RESCHED_ENABLE_PREEMPT
#cmakedefine01 KRN_MINIMAL_TIME_SLICE

/*
 * --------------------------------------------------------------------------
 *                  Hard-coded global & derived constants
 * --------------------------------------------------------------------------
 *
 * Here below there are some pseudo-constants not designed to be easily changed
 * because of the code makes assumptions about them. Because of that, those
 * constants are hard-coded and not available as CMake variables. With time,
 * some of those constants get "promoted" and moved in CMake, others remain
 * here. See the comments and think about the potential implications before
 * promoting a hard-coded constant to a configurable CMake variable.
 */
#define MEASURE_BOGOMIPS_TICKS        (TIMER_HZ / 10)
#define BOGOMIPS_CONST                          10000


#if !KRN_MINIMAL_TIME_SLICE

   /* Default case */
   #define TIME_SLICE_TICKS (TIMER_HZ / 25)

#else

   /*
    * DEBUG configuration used trigger as many context switches as possible
    * and reproduce race conditions in the kernel.
    */
   #define TIME_SLICE_TICKS 1

#endif
