#ifndef UBIQ_FPE_DEBUG_H
#define UBIQ_FPE_DEBUG_H

// #define DEBUG_ON // UNCOMMENT to Enable DEBUG macro

#ifdef DEBUG_ON
#define DEBUG(x,y) {x && y;}
#else
#define DEBUG(x,y)
#endif


#endif