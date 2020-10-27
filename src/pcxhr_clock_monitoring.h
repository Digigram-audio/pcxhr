


#ifndef __SOUND_PCXHR_CLOCK_MONITORING_H
#define __SOUND_PCXHR_CLOCK_MONITORING_H
#include "pcxhr.h"



/*
A timer wake up a hread wich monitor clock.
*/


//initialise clock management
int clockMonitor_init(struct pcxhr_mgr *mgr);
//clean all kmalloc,etc...
int clockMonitor_free(struct pcxhr_mgr *mgr);

//stop clock management
int clockMonitor_stop(struct pcxhr_mgr *mgr);
//start clock management. needs clockMonitor_init before
int clockMonitor_start(struct pcxhr_mgr *mgr);





#endif //__SOUND_PCXHR_CLOCK_MONITORING_H
