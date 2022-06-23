#ifndef UBIQ_FPE_DEBUG_H
#define UBIQ_FPE_DEBUG_H

// #define FPE_DEBUG_ON // UNCOMMENT to Enable FPE_DEBUG macro

#ifdef FPE_DEBUG_ON
#define FPE_DEBUG(x,y) {x && y;}
#else
#define FPE_DEBUG(x,y)
#endif


#endif