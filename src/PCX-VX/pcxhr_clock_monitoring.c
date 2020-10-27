


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>
#include <linux/slab.h>
#include <sound/core.h>
#include "pcxhr_clock_monitoring.h"
#include "pcxhr.h"

/*
	use case 1
	on démarre en AES et la clock disparait.

	use case 2
	La clock aes a disparut à un moment... et elle réapparait.

*/
void clock_monitoring_treatment(struct pcxhr_mgr *mgr) {
	int rate = 0;
	
	if((mgr != NULL) )
	{
		mutex_lock(&mgr->timer_mutex);
		if( mgr->use_clock_type != PCXHR_CLOCK_TYPE_INTERNAL ) { //on est cencé être sur de l AES
			pcxhr_get_external_clock(mgr,mgr->use_clock_type,&rate);
			if( rate == 0 ) {
				snd_printk(KERN_ERR "\tpcxhr_clock_monitoring_tasklet external clock disappears...\n");
				mgr->expected_clock_type = mgr->use_clock_type;
				mgr->use_clock_type = PCXHR_CLOCK_TYPE_INTERNAL;
				pcxhr_set_clock(mgr, mgr->sample_rate_real);
			}
		}
		else {  //on est déjà sur l horloge interne on test si l aes est de retour 
			pcxhr_get_external_clock(mgr,mgr->expected_clock_type,&rate);
			if( rate != 0) {
				snd_printk(KERN_ERR "\tpcxhr_clock_monitoring_tasklet external clock reappears...\n");
				mgr->use_clock_type = mgr->expected_clock_type;
				pcxhr_set_clock(mgr, rate);
			}
		}
		mutex_unlock(&mgr->timer_mutex);
	}else {
		snd_printk(KERN_ERR "\tclock_monitoring_treatment mgr pointer null\n");
	}
}

int clock_monitoring_thread(void *ptr) {
	struct pcxhr_mgr *mgr = (struct pcxhr_mgr*) ptr;

	printk(KERN_INFO "In thread %s", mgr->shortname);
	while (1){
		set_current_state(TASK_INTERRUPTIBLE);
		schedule();
		//snd_printk(KERN_ERR "+");
		clock_monitoring_treatment(mgr);
	}
	snd_printk(KERN_INFO "clock_monitoring_thread on sort\n");
	return 0;
}

int clock_monitoring_thread_init(struct pcxhr_mgr *mgr) {
   
	char* our_thread="clockMonitoring thread";

	mgr->thread = kthread_create(clock_monitoring_thread,mgr,our_thread);
	if(mgr->thread == NULL) {
		snd_printk(KERN_ERR "clock_monitoring_thread_init kthread_create FAILED\n");
		return -ENOMEM; 
	}
	wake_up_process(mgr->thread);
	return 0;
}

//void thread_cleanup(void) {
//}

static void clockMonitor_timedout(unsigned long ptr) {
	struct pcxhr_mgr *mgr = (struct pcxhr_mgr*) ptr;

	if( (mgr != NULL) && (mgr->timer_state == 1) ) {
		mod_timer(mgr->clockMonitor_timer, (jiffies + msecs_to_jiffies(1000)));
		wake_up_process(mgr->thread);
	}

	if(mgr->timer_state == 0) {
		snd_printk(KERN_INFO "\tclockMonitor_timedout timer should stop do not reload it\n");
	}
}


int clockMonitor_stop(struct pcxhr_mgr *mgr){
	int result = 0;
	mutex_lock(&mgr->timer_mutex);

	snd_printk(KERN_INFO " clockMonitor_stop %s\n", mgr->shortname);
	if((mgr->clockMonitor_timer != NULL)&& (mgr->timer_state == 1))	{
//		result = del_timer(mgr->clockMonitor_timer);
		result = del_timer_sync(mgr->clockMonitor_timer);
		mgr->timer_state = 0;
	}
	mutex_unlock(&mgr->timer_mutex);
	return 0;
}

int clockMonitor_start(struct pcxhr_mgr *mgr){
	mutex_lock(&mgr->timer_mutex);
	if(mgr->timer_state == 0) { //déjà était démarré... 
		snd_printk(KERN_INFO "clockMonitor_start %s\n", mgr->shortname);

		init_timer(mgr->clockMonitor_timer);
		mgr->clockMonitor_timer->function = clockMonitor_timedout;
		mgr->clockMonitor_timer->data = (unsigned long)mgr;
		mgr->clockMonitor_timer->expires = jiffies + msecs_to_jiffies(2000);
		add_timer(mgr->clockMonitor_timer);	
		mgr->timer_state = 1;
	}
	mutex_unlock(&mgr->timer_mutex);
	return 0;
}


int clockMonitor_init(struct pcxhr_mgr *mgr){
	snd_printk(KERN_INFO "clockMonitor_init %s\n", mgr->shortname);

	mutex_init(&mgr->timer_mutex);

	mgr->clockMonitor_timer = (struct timer_list*) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	if(mgr->clockMonitor_timer == NULL){
		snd_printk(KERN_INFO "clockMonitor_init failed mgr->clockMonitor_timer NULL\n");
		return -ENOMEM; 
	}
	//init_timer(mgr->clockMonitor_timer);

	mgr->timer_state = 0;

	return clock_monitoring_thread_init(mgr);
}

int clockMonitor_free(struct pcxhr_mgr *mgr){
	snd_printk(KERN_INFO "clockMonitor_free %s\n", mgr->shortname);

	if (mgr->clockMonitor_timer != NULL)
	{
		kfree(mgr->clockMonitor_timer);
	}
	mgr->timer_state = 0;

	return 0;
}







