/**
 cap-sensor driver
 IC: Azoteq corp, IQS128 IQ SWitch --ProxSense Serial
 Copyright by lenovo corp
 author lipeng1
*/
	
#ifndef __CAP_SENSOR__H__
#define __CAP_SENSOR__H__
	
	
#include <linux/ioctl.h>
#include <linux/workqueue.h>
#include <asm/atomic.h>
	
// cap-sensor private data defined here
typedef struct CAPSENSOR_priv {
	int  sensor_val;
	struct delayed_work eint_work;
        struct delayed_work eint_work_down;
	atomic_t onoff;
}CAPSENSOR_priv, *PCAPSENSOR_priv;
	
#endif //__CAP_SENSOR__H__
