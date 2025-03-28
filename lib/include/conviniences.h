#ifndef CONVINIENCES_H
#define CONVINIENCES_H

#define elsizeof(ARR_) (sizeof(*ARR_))
#define lenof(ARR_)    (sizeof(ARR_) / elsizeof(ARR_))

#define forceinline __attribute__((always_inline)) inline

#endif
