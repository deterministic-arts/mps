/* config.h: MPS CONFIGURATION
 *
 * $Id$
 * Copyright (c) 2001-2003, 2006 Ravenbrook Limited.  See end of file for license.
 * Portions copyright (c) 2002 Global Graphics Software.
 *
 * PURPOSE
 *
 * This module translates from high-level symbols defined by the
 * external build system (gnumake, nmake, etc.) into specific sets
 * of features used by MPS modules.
 *
 * DESIGN
 *
 * See <design/config/>.
 */

#ifndef config_h
#define config_h


/* CONFIG_VAR_* -- variety Configuration
 *
 * These are translated into the directives CONFIG_ASSERT, CONFIG_STATS,
 * CONFIG_LOG, etc. which control actual compilation features.
 */

/* CONFIG_VAR_RASH -- the rash and reckless variety
 *
 * This variety switches off as many features as possible for maximum
 * performance, but is therefore unsafe and undebuggable.  It is not intended
 * for use, but for comparison with the hot variety, to check that assertion,
 * logging, etc. have negligible overhead.
 */

#if defined(CONFIG_VAR_RASH)
/* no asserts */
/* no statistic meters */
/* no telemetry log events */


/* CONFIG_VAR_DIAG -- diagnostic variety
 *
 * Deprecated.  The diagnostic variety prints messages about the internals
 * of the MPS to an output stream.  This is being replaced by an extended
 * telemetry system.  RB 2012-08-31
 */

#elif defined(CONFIG_VAR_DIAG) /* Diagnostic variety */
#define CONFIG_ASSERT
#ifndef CHECKLEVEL
#define CHECKLEVEL      CheckLevelMINIMAL
#endif
#define CONFIG_STATS
/* For diagnostics, choose a DIAG_WITH_... output method.
 * (We need to choose because the DIAG output system is under 
 * development.  RHSK 2007-05-21).
 */
#define DIAG_WITH_STREAM_AND_WRITEF
/* #define DIAG_WITH_PRINTF */
#define CONFIG_LOG


/* CONFIG_VAR_COOL -- cool variety
 *
 * The cool variety is intended for use when developing an integration with
 * the MPS or debugging memory problems or collecting detailed telemetry
 * data for performance analysis.  It has more thorough consistency checking
 * and data collection and output, and full debugging information.
 */

#elif defined(CONFIG_VAR_COOL)
#define CONFIG_ASSERT
#define CONFIG_ASSERT_ALL
#define CONFIG_STATS
#ifndef CHECKLEVEL
#define CHECKLEVEL      CheckLevelSHALLOW
#endif
#define CONFIG_LOG
#define CONFIG_LOG_ALL


#else /* CONFIG_VAR_* */

/* CONFIG_VAR_HOT -- the hot variety
 *
 * This variety is the default variety for distribution in products that use
 * the MPS.  It has maximum performance while retaining a good level of
 * consistency checking and allowing some debugging and telemetry features.
 */

/* #elif defined(CONFIG_VAR_HOT) */
#define CONFIG_ASSERT
#ifndef CHECKLEVEL
#define CHECKLEVEL      CheckLevelMINIMAL
#endif
/* no statistic meters */
#define CONFIG_LOG

#endif /* CONFIG_VAR_* */


/* Build Features */


#if defined(CONFIG_ASSERT)
/* asserts: AVER, AVERT, NOTREACHED, CHECKx */
/* note: a direct call to ASSERT() will *still* fire */
#define AVER_AND_CHECK
#if defined(CONFIG_ASSERT_ALL)
#define AVER_AND_CHECK_ALL
#define MPS_ASSERT_STRING "assertastic"
#else /* CONFIG_ASSERT_ALL, not */
#define MPS_ASSERT_STRING "asserted"
#endif /* CONFIG_ASSERT_ALL */
#else /* CONFIG_ASSERT, not */
#define AVER_AND_CHECK_NONE
#define MPS_ASSERT_STRING "nonasserted"
#endif


#if defined(CONFIG_STATS)
/* CONFIG_STATS = STATISTICS = METERs */
/* Note: the STATISTICS define used to be called "DIAGNOSTICS" (even */
/* though it controls the STATISTIC system), but the term */
/* "diagnostic" means something else now: see design/diag/. */
/* RHSK 2007-06-28 */
/* WARNING: this may change the size and fields of MPS structs */
/* (...but see STATISTIC_DECL, which is invariant) */
#define STATISTICS
#define MPS_STATS_STRING "stats"
#else
#define STATISTICS_NONE
#define MPS_STATS_STRING "nonstats"
#endif


#if defined(CONFIG_LOG)
/* TELEMETRY = LOG = EVENTs */
#define EVENT
#if defined(CONFIG_LOG_ALL)
#define EVENT_ALL 1     /* log events on critical path */
#define MPS_LOG_STRING "logtastic"
#else /* CONFIG_LOG_ALL, not */
#define EVENT_ALL 0     /* don't log events on critical path */
#define MPS_LOG_STRING "logging"
#endif /* CONFIG_LOG_ALL */
#else /* CONFIG_LOG, not */
#define EVENT_NONE
#define MPS_LOG_STRING "nonlogging"
#endif /* CONFIG_LOG */


/* CONFIG_PLINTH_NONE -- exclude the ANSI plinth
 *
 * Some MPS deployment environments want to avoid dependencies on the
 * standard C library.  In this case, the plinth, defined in mpslib.h must
 * be supplied when linking.
 *
 * For example, Open Dylan on Windows does not link the C library, but
 * supplies its own plinth directly using Windows and Dylan interfaces.
 *
 * CONFIG_PLINTH_NONE tells mps.c to exclude the ANSI plinth and removes
 * all standard C library dependencies.  e.g.
 *
 *     cc -O2 -c -DCONFIG_PLINTH_NONE mps.c
 */

#if defined(CONFIG_PLINTH_NONE)
#define PLINTH_NONE
#endif


#define MPS_VARIETY_STRING \
  MPS_ASSERT_STRING "." MPS_LOG_STRING "." MPS_STATS_STRING


/* Platform Configuration */

#include "mpstd.h"

/* Suppress Visual C warnings at warning level 4, */
/* see mail.richard.1997-09-25.13-26. */
/* Essentially the same settings are done in testlib.h. */

#ifdef MPS_BUILD_MV

/* "unreferenced inline function has been removed" (windows.h) */
#pragma warning(disable: 4514)

/* "constant conditional" (MPS_END) */
#pragma warning(disable: 4127)

/* "unreachable code" (ASSERT, if cond is constantly true). */
#pragma warning(disable: 4702)

/* "expression evaluates to a function which is missing an argument list" */
#pragma warning(disable: 4550)

/* "local variable is initialized but not referenced" */
#pragma warning(disable: 4189)

/* "not all control paths return a value" */
#pragma warning(disable: 4715)

/* MSVC 2.0 generates a warning when using NOCHECK or UNUSED */
#ifdef _MSC_VER
#if _MSC_VER < 1000
#pragma warning(disable: 4705)
#endif
#else /* _MSC_VER */
#error "Expected _MSC_VER to be defined for builder.mv"
#endif /* _MSC_VER */


/* Non-checking varieties give many spurious warnings because parameters
 * are suddenly unused, etc.  We aren't interested in these
 */

#if defined(AVER_AND_CHECK_NONE)

/* "unreferenced formal parameter" */
#pragma warning(disable: 4100)

/* "unreferenced local function has been removed" */
#pragma warning(disable: 4505)

#endif /* AVER_AND_CHECK_NONE */

#endif /* MPS_BUILD_MV */


/* EPVMDefaultSubsequentSegSIZE is a default for the alignment of
 * subsequent segments (non-initial at each save level) in EPVM.  See
 * design.mps.poolepvm.arch.segment.size.
 */

#define EPVMDefaultSubsequentSegSIZE ((Size)64 * 1024)


/* Arena Configuration -- see <code/arena.c>
 *
 * .client.seg-size: ARENA_CLIENT_PAGE_SIZE is the size in bytes of a
 * "page" (i.e., segment granule) in the client arena.  It's set at 8192
 * with no particular justification.
 */

#define ARENA_CONTROL_EXTENDBY  ((Size)4096)
#define ARENA_CONTROL_AVGSIZE   ((Size)32)
#define ARENA_CONTROL_MAXSIZE   ((Size)65536)

#define ArenaPollALLOCTIME (65536.0)

#define ARENA_ZONESHIFT         ((Shift)20)

#define ARENA_CLIENT_PAGE_SIZE          ((Size)8192)

#define ArenaDefaultZONESET (ZoneSetUNIV << (MPS_WORD_WIDTH / 2))
/* @@@@ knows the implementation of ZoneSets */

/* .segpref.default: For EPcore, non-DL segments should be placed high */
/* to reduce fragmentation of DL pools (see request.epcore.170193). */
#define SegPrefDEFAULT { \
  SegPrefSig,          /* sig */ \
  TRUE,                /* high */ \
  ArenaDefaultZONESET, /* zoneSet */ \
  FALSE,               /* isCollected */ \
  FALSE,               /* isGen */ \
  (Serial)0,           /* gen */ \
}

#define LDHistoryLENGTH ((Size)4)


/* Stack configuration */

/* Currently StackProbe has a useful implementation only on
 * Intel platforms and only when using Microsoft build tools (builder.mv)
 */
#if defined(MPS_ARCH_I3) && defined(MPS_BUILD_MV)
#define StackProbeDEPTH ((Size)500)
#else
#define StackProbeDEPTH ((Size)0)
#endif /* MPS_ARCH_I3 */


/* Shield Configuration -- see <code/shield.c> */

#define ShieldCacheSIZE ((size_t)16)
#define ShieldDepthWIDTH (4)


/* VM Configuration -- see <code/vm*.c> */

#define VMANPageALIGNMENT ((Align)4096)
#define VMJunkBYTE ((unsigned char)0xA9)

/* Protection Configuration see <code/prot*.c>

   For each architecture/OS that uses protix.c or protsgix.c, we need to
   define what signal number to use, and what si_code value to check.
*/

#if defined(MPS_OS_FR)
#define PROT_SIGNAL (SIGSEGV)
#elif defined(MPS_OS_XC)
#define PROT_SIGNAL (SIGBUS)
#endif

#if defined(MPS_OS_XC)
#define PROT_SIGINFO_GOOD(info) (1)
#elif defined(MPS_OS_FR)
#define PROT_SIGINFO_GOOD(info) ((info)->si_code == SEGV_ACCERR)
#endif


/* Tracer Configuration -- see <code/trace.c> */

#define TraceLIMIT ((size_t)1)
/* I count 4 function calls to scan, 10 to copy. */
#define TraceCopyScanRATIO (1.5)

/* Chosen so that the RememberedSummaryBlockStruct packs nicely into
   pages */
#define RememberedSummaryBLOCK 15


/* Events
 *
 * EventBufferSIZE is the number of words in the global event buffer.
 */

#define EventBufferSIZE ((size_t)4096)
#define EventStringLengthMAX ((size_t)255) /* Not including NUL */


/* Assert Buffer */

#define ASSERT_BUFFER_SIZE      ((Size)512)


/* Diagnostics Buffer */

#ifdef DIAG_WITH_STREAM_AND_WRITEF
/* DIAG_BUFFER_SIZE: 100 screenfuls: 100x80x25 = 200000 */
#define DIAG_BUFFER_SIZE      ((Size)200000)
#else
#define DIAG_BUFFER_SIZE      ((Size)1)
#endif

#define DIAG_PREFIX_TAGSTART "MPS."
#define DIAG_PREFIX_LINE     " "
#define DIAG_PREFIX_TAGEND   ""


/* memory operator configuration
 *
 * We need efficient operators similar to memcpy, memset, and memcmp.
 * In general, we cannot use the C library mem functions directly as
 * that would not be freestanding.  However, on some platforms we can do
 * this, because they are inlined by the compiler and so do not actually
 * create a dependence on an external library.
 */

#if defined(MPS_PF_W3I3MV)
/* MSVC on Intel inlines mem* when optimizing */
#define mps_lib_memset(p, c, l) memset(p, c, l)
#define mps_lib_memcpy(p, q, s) memcpy(p, q, s)
#define mps_lib_memcmp(p, q, s) memcmp(p, q, s)
/* get prototypes for ANSI mem* */
#include <string.h>
#endif


/* Product Configuration
 *
 * Deprecated, see design/config/#req.prod>.  This now only contains the
 * configuration used by the former "MPS" product, which is now the only
 * product.
 */

#define MPS_PROD_STRING         "mps"
#define MPS_PROD_MPS
#define ARENA_INIT_SPARE_COMMIT_LIMIT   ((Size)10uL*1024uL*1024uL)
#define THREAD_MULTI
#define PROTECTION
#define PROD_CHECKLEVEL_INITIAL CheckLevelSHALLOW


/* Pool Class AMC configuration */

/* AMC treats segments of this many pages (or more) as "Large" */
#define AMCLargeSegPAGES ((Count)8)


/* Pool Class AWL configuration -- see poolawl.c for usage */

#define AWL_HAVE_SEG_SA_LIMIT   TRUE
#define AWL_SEG_SA_LIMIT        200     /* TODO: Improve guesswork with measurements */
#define AWL_HAVE_TOTAL_SA_LIMIT FALSE
#define AWL_TOTAL_SA_LIMIT      0


#endif /* config_h */


/* C. COPYRIGHT AND LICENSE
 *
 * Copyright (C) 2001-2003, 2006 Ravenbrook Limited <http://www.ravenbrook.com/>.
 * All rights reserved.  This is an open source license.  Contact
 * Ravenbrook for commercial licensing options.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Redistributions in any form must be accompanied by information on how
 * to obtain complete source code for this software and any accompanying
 * software that uses this software.  The source code must either be
 * included in the distribution or be available for no more than the cost
 * of distribution plus a nominal fee, and must be freely redistributable
 * under reasonable conditions.  For an executable file, complete source
 * code means the source code for all modules it contains. It does not
 * include source code for modules or files that typically accompany the
 * major components of the operating system on which the executable file
 * runs.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, OR NON-INFRINGEMENT, ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */