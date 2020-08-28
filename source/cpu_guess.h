#ifndef cpu_guess_h
#define cpu_guess_h

/* ARM detection */
#if defined ARM || defined __arm__ || defined _ARM
#	define CPU_GUESS_ARM
#endif

/* x86, x86-64 detection */
#if defined __X86__ || defined __i386__ || defined i386 || defined _M_IX86 || defined __386__ || defined __x86_64__ || defined _M_X64
#	define CPU_GUESS_X86
#	if defined __x86_64__ || defined _M_X64
#		define CPU_GUESS_X86_64
#	endif
#endif

/* ARMv6 detection */
#if defined __ARM_ARCH_6__ || defined __ARM_ARCH_6J__ || defined __ARM_ARCH_6K__ || defined __ARM_ARCH_6Z__ || defined __ARM_ARCH_6ZK__ || defined __ARM_ARCH_6T2__
#	define CPU_GUESS_ARM_V6
#endif

/* ARMv7 detection */
#if defined __ARM_ARCH_7__ || defined __ARM_ARCH_7A__ || defined __ARM_ARCH_7R__ || defined __ARM_ARCH_7M__
#	define CPU_GUESS_ARM_V7
#endif

#endif /* cpu_guess_h */
