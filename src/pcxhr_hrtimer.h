/*
 * pcx_hrtimer.h
 *
 *  Created on: 30 juin 2017
 *      Author: sylvain
 */

#ifndef PCXHR_PCX_HRTIMER_H_
#define PCXHR_PCX_HRTIMER_H_

#include <linux/hrtimer.h>
#include "pcxhr.h"
#include "pcxhr_core.h"

/* interrupt handling */
enum hrtimer_restart pcxhr_hr_timer_callback(struct hrtimer* hr_timer);

void pcxhr_hr_timer_tasklet(unsigned long data);
void pcxhr_hr_timer_trig(struct pcxhr_mgr *mgr, int start, unsigned int rate);
void pcxhr_hr_timer_init(struct pcxhr_mgr *mgr); 
void pcxhr_hr_timer_kill(struct pcxhr_mgr *mgr);

#endif /* PCXHR_PCX_HRTIMER_H_ */
