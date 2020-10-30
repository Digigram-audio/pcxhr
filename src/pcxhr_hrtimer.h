/*
 * Driver for Digigram pcxhr compatible soundcards
 *
 * Copyright (c) 2004 by Digigram <alsa@digigram.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
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
