#ifndef UBIQ_FPE_INTERNAL_RADIX_H
#define UBIQ_FPE_INTERNAL_RADIX_H

#include <sys/cdefs.h>

__BEGIN_DECLS

int radix_convert(char * const out, const char * const oalpha,
                  const char * const inp, const char * const ialpha);

__END_DECLS

#endif
