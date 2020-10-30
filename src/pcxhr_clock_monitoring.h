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
