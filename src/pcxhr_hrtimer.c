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


#include "pcxhr_hrtimer.h"

void pcxhr_hr_timer_tasklet(unsigned long data)
{
	pcxhr_interrupt(0, (void*)data);
}

enum hrtimer_restart pcxhr_hr_timer_callback(struct hrtimer* hr_timer)
{
	struct pcxhr_mgr *mgr;

	mgr = container_of(hr_timer, struct pcxhr_mgr, hr_timer);
	if (!atomic_read(&mgr->hr_timer_running))
		return HRTIMER_NORESTART;
	tasklet_schedule(&mgr->hr_timer_tasklet);
	hrtimer_forward_now(hr_timer, mgr->hr_timer_period_time);
	return HRTIMER_RESTART;
}


void pcxhr_hr_timer_trig(struct pcxhr_mgr *mgr, int start, unsigned int rate) {
	if (start)
	{
		unsigned int period = mgr->granularity/2;
		long sec;
		unsigned long nsecs;

		sec = period/rate;
		period %= rate;
		nsecs = div_u64((u64)period * 1000000000UL + rate - 1, rate);

		tasklet_kill(&mgr->hr_timer_tasklet); // Make sure the tasklet is not running.
		mgr->hr_timer_period_time = ktime_set(sec, nsecs);
		atomic_set(&mgr->hr_timer_running, 1);
		hrtimer_start(&mgr->hr_timer, mgr->hr_timer_period_time, HRTIMER_MODE_REL);
	}
	else
	{
		atomic_set(&mgr->hr_timer_running, 0);
		hrtimer_cancel(&mgr->hr_timer);
	}

}


void pcxhr_hr_timer_init(struct pcxhr_mgr *mgr) {
	hrtimer_init( &mgr->hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
	mgr->hr_timer.function = pcxhr_hr_timer_callback;
	atomic_set(&mgr->hr_timer_running, 0);
	tasklet_init(&mgr->hr_timer_tasklet, pcxhr_hr_timer_tasklet,
		     (unsigned long)mgr);

}

void pcxhr_hr_timer_kill(struct pcxhr_mgr *mgr) {
	tasklet_kill(&mgr->hr_timer_tasklet);
}
