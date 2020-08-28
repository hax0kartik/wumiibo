#ifndef bswap_h
#define bswap_h

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu_guess.h"

#if defined _MSC_VER && defined CPU_GUESS_X86_64
#include <intrin.h>
#endif

static inline unsigned short bswap_16(unsigned short x) {
#if defined _MSC_VER && defined CPU_GUESS_X86_64
	return _byteswap_ushort(x);
#elif defined _MSC_VER && defined CPU_GUESS_X86
	__asm lea  edx, [x]
	__asm mov  ax, [edx]
	__asm xchg ah, al
	__asm mov  [edx], ax
	return x;
#elif defined CPU_GUESS_X86
	__asm__("rorw $8, %0" : "+r"(x));
	return x;
#elif defined CPU_GUESS_ARM_V6 || defined CPU_GUESS_ARM_V7
	__asm__("rev16 %0, %0" : "+r"(x));
	return x;
#else
	return (x >> 8) | (x << 8);
#endif
}

static inline unsigned int bswap_32(unsigned int x) {
#if defined _MSC_VER && defined CPU_GUESS_X86_64
	return _byteswap_ulong(x);
#elif defined _MSC_VER && defined CPU_GUESS_X86
	__asm lea  edx, [x]
	__asm mov  eax, [edx]
	__asm bswap eax
	__asm mov  [edx], eax
	return x;
#elif defined CPU_GUESS_X86
#	if __CPU__ != 386
	__asm__("bswap   %0" : "+r"(x));
	return x;
#	else
	__asm__(
			"rorw    $8,  %w0 \n\t"
			"rorl    $16, %0  \n\t"
			"rorw    $8,  %w0"
			: "+r"(x));
	return x;
#	endif
#elif defined CPU_GUESS_ARM_V6 || defined CPU_GUESS_ARM_V7
	__asm__("rev %0, %0" : "+r"(x));
	return x;
#elif defined CPU_GUESS_ARM
	unsigned int t;
	__asm__(
			"eor %1, %0, %0, ror #16 \n\t"
			"bic %1, %1, #0xFF0000   \n\t"
			"mov %0, %0, ror #8      \n\t"
			"eor %0, %0, %1, lsr #8  \n\t"
			: "+r"(x), "=&r"(t));
	return x;
#else
	x = ((x << 8) & 0xFF00FF00) | ((x >> 8) & 0x00FF00FF);
	return (x >> 16) | (x << 16);
#endif
}

static inline unsigned long long bswap_64(unsigned long long x) {
#if defined _MSC_VER && defined CPU_GUESS_X86_64
	return _byteswap_uint64(x);
#elif defined _MSC_VER && defined CPU_GUESS_X86
	__asm lea  edx, [x]
	__asm mov  eax, [edx]
	__asm bswap eax
	__asm xchg eax, [edx+4]
	__asm bswap eax
	__asm mov  [edx], eax
	return x;
#elif defined CPU_GUESS_X86_64
	__asm__("bswap  %0": "=r"(x) : "0"(x));
	return x;
#else
	union {
		unsigned long long ll;
		unsigned int l[2];
	} w, r;
	w.ll = x;
	r.l[0] = bswap_32(w.l[1]);
	r.l[1] = bswap_32(w.l[0]);
	return r.ll;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* bswap_h */
