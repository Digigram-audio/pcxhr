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