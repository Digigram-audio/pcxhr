/*
 * Driver for Digigram pcxhr soundcards
 *
 * main header file
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

#ifndef __SOUND_PCXHR_H
#define __SOUND_PCXHR_H

#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <sound/pcm.h>
#include "pcxhr_hrtimer.h"

/* Driver version :  2.0.13 */
#define PCXHR_DRIVER_VERSION		0x020013
#define PCXHR_DRIVER_VERSION_STRING	"2.0.13"	

/* Workaround for CentOS/RHEL */
#ifdef RHEL_RELEASE_CODE
#  define HAVE_SND_CARD_NEW (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7,5))
#  define PREALLOCATE_PAGES_FOR_ALL_RETURNS_NO_ERR (RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7,5))
#else
#  define HAVE_SND_CARD_NEW (LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0))
#  define PREALLOCATE_PAGES_FOR_ALL_RETURNS_NO_ERR (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
#endif

#define PCXHR_MAX_CARDS		6
#define PCXHR_PLAYBACK_STREAMS	4

#define PCXHR_GRANULARITY	96	/* min 96 and multiple of 48 */
/* transfer granularity of pipes and the dsp time (MBOX4) */
#define PCXHR_GRANULARITY_MIN	96
/* TODO : granularity could be 64 or 128 */
#define PCXHR_GRANULARITY_HR22	192	/* granularity for stereo cards */

struct snd_pcxhr;
struct pcxhr_mgr;

struct pcxhr_stream;
struct pcxhr_pipe;

enum pcxhr_clock_type {
	PCXHR_CLOCK_TYPE_INTERNAL = 0,
	PCXHR_CLOCK_TYPE_WORD_CLOCK,
	PCXHR_CLOCK_TYPE_AES_SYNC,
	PCXHR_CLOCK_TYPE_AES_1,
	PCXHR_CLOCK_TYPE_AES_2,
	PCXHR_CLOCK_TYPE_AES_3,
	PCXHR_CLOCK_TYPE_AES_4,
	PCXHR_CLOCK_TYPE_MAX = PCXHR_CLOCK_TYPE_AES_4,
	HR22_CLOCK_TYPE_INTERNAL = PCXHR_CLOCK_TYPE_INTERNAL,
	HR22_CLOCK_TYPE_AES_SYNC,
	HR22_CLOCK_TYPE_AES_1,
	HR22_CLOCK_TYPE_MAX = HR22_CLOCK_TYPE_AES_1,
};

enum pcxhr_board_revision_id{
	PCXHR_BOARD_REVISION_BEFORE_5 = 0,
	PCXHR_BOARD_REVISION_5 = 1,
	PCXHR_BOARD_REVISION_7 = 2,
};

struct pcxhr_dbg_cpt {
	unsigned int interrupts_all;
	unsigned int interrupts_none;
	unsigned int interrupts_handled;
	unsigned int interrupts_dummy;
	unsigned int interrupts_irq;
	unsigned int interrupts_async;
	unsigned int interrupts_incr;
	unsigned int interrupts_loop;
	unsigned int interrupts_false_irq_none;
};

struct pcxhr_mgr {
	unsigned int num_cards;
	struct snd_pcxhr *chip[PCXHR_MAX_CARDS];

	struct pci_dev *pci;

	int irq;

	int granularity;

	/* card access with 1 mem bar and 2 io bar's */
	unsigned long port[3];
	unsigned long dsp_reg_offset;

	/* share the name */
	char shortname[32];		/* short name of this soundcard */
	char longname[96];		/* name of this soundcard */

	/* message tasklet */
	struct tasklet_struct msg_taskq;
	struct pcxhr_rmh *prmh;
	/* trigger tasklet */
	struct tasklet_struct trigger_taskq;

	spinlock_t lock;		/* interrupt spinlock */
	spinlock_t msg_lock;		/* message spinlock */

	struct mutex setup_mutex;	/* mutex used in hw_params, open and close */
	struct mutex mixer_mutex;	/* mutex for mixer */

	/* hardware interface */
	unsigned int dsp_loaded;	/* bit flags of loaded dsp indices */
	unsigned int dsp_version;	/* read from embedded once firmware is loaded */
	int playback_chips;
	int capture_chips;
	int fw_file_set;
	int firmware_num;
	unsigned int is_hr_stereo:1;
	unsigned int board_has_aes1:1;	/* if 1 board has AES1 plug and SRC */
	unsigned int board_has_analog:1; /* if 0 the board is digital only */
	unsigned int board_has_mic:1; /* if 1 the board has microphone input */
	unsigned int board_aes_in_192k:1;/* if 1 the aes input plugs do support 192kHz */
	unsigned int mono_capture:1; /* if 1 the board does mono capture */
	unsigned int capture_ltc:1; /* if 1 the board captures LTC input */

	struct snd_dma_buffer hostport;

	enum pcxhr_clock_type use_clock_type;	/* clock type selected by mixer */
	enum pcxhr_clock_type cur_clock_type;	/* current clock type synced */
	int sample_rate;
	int ref_count_rate;
	int timer_toggle;		/* timer interrupt toggles between the two values 0x200 and 0x300 */
	int dsp_time_last;		/* the last dsp time (read by interrupt) */
	int dsp_time_err;		/* dsp time errors */
	int hr_timer_calls;		/* hrtimer calls */
	unsigned int src_it_dsp;	/* dsp interrupt source */
	unsigned int io_num_reg_cont;	/* backup of IO_NUM_REG_CONT */
	unsigned int codec_speed;	/* speed mode of the codecs */
	unsigned int sample_rate_real;	/* current real sample rate */
	int last_reg_stat;
	int async_err_stream_xrun;
	int async_err_pipe_xrun;
	int async_err_other_last;

	unsigned char xlx_cfg;		/* copy of PCXHR_XLX_CFG register */
	unsigned char xlx_selmic;	/* copy of PCXHR_XLX_SELMIC register */
	unsigned char dsp_reset;	/* copy of PCXHR_DSP_RESET register */

	ktime_t hr_timer_period_time;
	atomic_t hr_timer_running;
	struct hrtimer hr_timer;
	struct tasklet_struct hr_timer_tasklet;

	struct pcxhr_dbg_cpt dbg_cpt;

	enum pcxhr_board_revision_id board_revision;
};

/* pcxhr_mgr helpers */
static inline unsigned int pcxhr_is_board_revision5 (const struct pcxhr_mgr* toTest)
{
	return toTest->board_revision == PCXHR_BOARD_REVISION_5;
}

static inline unsigned int pcxhr_is_board_revision7 (const struct pcxhr_mgr* toTest)
{
	return toTest->board_revision == PCXHR_BOARD_REVISION_7;
}

static inline unsigned int pcxhr_is_board_before_revision5 (const struct pcxhr_mgr* toTest)
{
	return toTest->board_revision == PCXHR_BOARD_REVISION_BEFORE_5;
}


enum pcxhr_stream_status {
	PCXHR_STREAM_STATUS_FREE,
	PCXHR_STREAM_STATUS_OPEN,
	PCXHR_STREAM_STATUS_SCHEDULE_RUN,
	PCXHR_STREAM_STATUS_STARTED,
	PCXHR_STREAM_STATUS_RUNNING,
	PCXHR_STREAM_STATUS_SCHEDULE_STOP,
	PCXHR_STREAM_STATUS_STOPPED,
	PCXHR_STREAM_STATUS_PAUSED
};

struct pcxhr_stream {
	struct snd_pcm_substream *substream;
	snd_pcm_format_t format;
	struct pcxhr_pipe *pipe;

	enum pcxhr_stream_status status;	/* free, open, running, draining, pause */

	u_int64_t timer_abs_periods;	/* timer: samples elapsed since TRIGGER_START (multiple of period_size) */
	u_int32_t timer_period_frag;	/* timer: samples elapsed since last call to snd_pcm_period_elapsed (0..period_size) */
	u_int32_t timer_buf_periods;	/* nb of periods in the buffer that have already elapsed */
	int timer_is_synced;		/* if(0) : timer needs to be resynced with real hardware pointer */

	int channels;
};


enum pcxhr_pipe_status {
	PCXHR_PIPE_UNDEFINED,
	PCXHR_PIPE_DEFINED
};

struct pcxhr_pipe {
	enum pcxhr_pipe_status status;
	int is_capture;		/* this is a capture pipe */
	int first_audio;	/* first audio num */
};


struct snd_pcxhr {
	struct snd_card *card;
	struct pcxhr_mgr *mgr;
	int chip_idx;		/* zero based */

	struct snd_pcm *pcm;		/* PCM */

	struct pcxhr_pipe playback_pipe;	/* 1 stereo pipe only */
	struct pcxhr_pipe capture_pipe[2];	/* 1 stereo or 2 mono pipes */

	struct pcxhr_stream playback_stream[PCXHR_PLAYBACK_STREAMS];
	struct pcxhr_stream capture_stream[2];	/* 1 stereo or 2 mono streams */
	int nb_streams_play;
	int nb_streams_capt;

	int analog_playback_active[2];	/* Mixer : Master Playback !mute */
	int analog_playback_volume[2];	/* Mixer : Master Playback Volume */
	int analog_capture_volume[2];	/* Mixer : Master Capture Volume */
	int digital_playback_active[PCXHR_PLAYBACK_STREAMS][2];
	int digital_playback_volume[PCXHR_PLAYBACK_STREAMS][2];
	int digital_capture_volume[2];	/* Mixer : Digital Capture Volume */
	int monitoring_active[2];	/* Mixer : Monitoring Active */
	int monitoring_volume[2];	/* Mixer : Monitoring Volume */
	int audio_capture_source;	/* Mixer : Audio Capture Source */
	int mic_volume;			/* used by cards with MIC only */
	int mic_boost;			/* used by cards with MIC only */
	int mic_active;			/* used by cards with MIC only */
	int analog_capture_active;	/* used by cards with MIC only */
	int phantom_power;		/* used by cards with MIC only */

	unsigned char aes_bits[5];	/* Mixer : IEC958_AES bits */
};

struct pcxhr_hostport
{
	char purgebuffer[6];
	char reserved[2];
};

/* exported */
int pcxhr_create_pcm(struct snd_pcxhr *chip);
int pcxhr_set_clock(struct pcxhr_mgr *mgr, unsigned int rate);
int pcxhr_get_external_clock(struct pcxhr_mgr *mgr,
			     enum pcxhr_clock_type clock_type,
			     int *sample_rate);

#endif /* __SOUND_PCXHR_H */
