//
// Zynq partial reconfiguration test code
// Marco Pagani - 2016 - marco.pag<#a#t#>outlook.com
//

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include "xtime_l.h"

typedef struct Stopwatch_ {

	XTime t_begin;
	XTime t_end;

} Stopwatch;

//TODO: static

static inline
void stopwatch_start(Stopwatch *self)
{
	XTime_GetTime(&self->t_begin);
}

static inline
void stopwatch_stop(Stopwatch *self)
{
	XTime_GetTime(&self->t_end);
}

static inline
double stopwatch_get_us(const Stopwatch *self)
{
	return ((self->t_end - self->t_begin) / (COUNTS_PER_SECOND / 1000000.0));
}

static inline
double stopwatch_get_ms(const Stopwatch *self)
{
	return ((self->t_end - self->t_begin) / (COUNTS_PER_SECOND / 1000.0));
}

static inline
double stopwatch_get_s(const Stopwatch *self)
{
	return ((self->t_end - self->t_begin) / (COUNTS_PER_SECOND / 1.0));
}

static inline
double stopwatch_get_ratio(const Stopwatch *self, const Stopwatch *other)
{
	return ((double)(self->t_end - self->t_begin) / (double)(other->t_end - other->t_begin));
}

#endif /* STOPWATCH_H_ */
