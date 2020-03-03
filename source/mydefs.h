/*	mydefs.h ***********************************************************
 * 	User definitions for the whole project.
 */

#ifndef MYDEFS_H_
#define MYDEFS_H_

// DEBUG alreazdy defined in ECLIPSE
//#define DEBUG

#ifdef DEBUG
#define DEBUG_TEST 1
#else
#define DEBUG_TEST 0
#endif

#define NOASSERT
//#undef NOASSERT
#undef NOTRACE
#define NOTRACE

// Std includes
//#include <stddef.h>
#include <stdbool.h>	// provides bool as a macro to _Bool.

#if !defined(NOTRACE)
#include <trace.h>		// no las uso, pero las debo incluir
#endif

#if !defined(NOASSERT)
#include <assert.h>		// no las uso, pero las debo incluir
#endif

#define ALL_ONES	(0xFFFFFFFF)	// All-ones 32-bit word.
#define ALL_ZEROS	(0x00000000)	// All-zeros 32-bit word.
#define forever() 	for(;;)			// Infinite loop.

// Math defines
#ifndef ARM_MATH_CM4
#define PI			3.1415926536f	// PI number as float.
#endif /* ARM_MATH_CM4 */
#define TWO_PI		2*PI
#define _180_DIV_PI	180/PI

//	Keyword "__ramfunc" instructs the linker to locate the function in RAM.
#define __ramfunc __attribute__((long_call, section (".ramfunc")))

//	Keyword "__inlinefunc" force the compiler to assemble the function within the caller.
#define __inlinefunc __attribute__((always_inline));

/* Function inlining command. */
#define INLINE	inline __attribute__((gnu_inline)) //__attribute__ (( always_inline ));

/* String definitions */
#define STRING_EOL    "\r"

#define CLAMP(number, low, high) min(high, max(low, number))

//------------------------------------------------------------------------------
//      Macros
//------------------------------------------------------------------------------

#define	tokenize(A, B)	A ## B		// Create the token AB.
#define	tokenize3(A, B, C)	A ## B ## C	// Create the token ABC.

// There is no format specifier for bool. You can print it using
// some of the existing specifiers for printing integral types or
// do something more fancy:
#define btoa(x) ((x)?"true":"false")	// binary-to-ASCII

#define ceil_div(x, y) ((x) + (y) - 1) / (y) // divide, rounding up.
#define round_div(x, divisor)  (((x) + ((divisor) / 2)) / (divisor))

#define DEG2RAD(d)   (((d)*PI)/180.0f)


/* DEBUG Printting with macros
 * It assumes you are using C99 (the variable argument list notation
 * is not supported in earlier versions). The do { ... } while (0)
 * idiom ensures that the code acts like a statement (function call).
 * The unconditional use of the code ensures that the compiler always
 * checks that your debug code is valid — but the optimizer will
 * remove the code when DEBUG is 0.
 */
#define debug_print(fmt, ...) \
            do { if (DEBUG_TEST) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
// with more diagnostics:
/*
#define debug_print(fmt, ...) \
        do { if (DEBUG_TEST) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)
*/



// Set or clear flag(s) in a register.
// TODO: esto colapse con las definiciones en c++
//#define set(register, flags)        ((register) = (register) | (flags))
//#define clear(register, flags)      ((register) &= ~(flags))


//------------------------------------------------------------------------------
//      Useful enumerations
//------------------------------------------------------------------------------

enum CommonYesNo { NO , YES = !NO};
enum CommonOffOnUndef { OFF , ON = !OFF , UNDEF = -1 };
enum CommonStopStart { STOP , START = !STOP , PAUSE = -1};
enum CommonEnableDisable { DISABLE , ENABLE = !DISABLE};
enum CommonEnabledDisabled { DISABLED , ENABLED = !DISABLED};
enum CommonFailedSuccessful { FAILED = -1 , NOTHING , SUCCESSFUL };

/*
 * En c++ CommonEnableDisable no compila, pues DISABLE es algo que no se que es
 */

#endif /* MYDEFS_H_ */
