/*
 * kernel/power/autosleep.c
 *
 * Opportunistic sleep support.
 *
 * Copyright (C) 2012 Rafael J. Wysocki <rjw@sisk.pl>
 */

#include <linux/device.h>
#include <linux/mutex.h>
#ifdef CONFIG_SUSPEND_SYNC_WORKQUEUE 
#include <linux/syscalls.h> /* sys_sync */
#endif /* CONFIG_SUSPEND_SYNC_WORKQUEUE */
#include <linux/pm_wakeup.h>

#include "power.h"

#define _TAG_PM_M "[Ker_PM]"
#define pm_log(fmt, ...)	pr_info("[%s][%s]" fmt, _TAG_PM_M, __func__, ##__VA_ARGS__);

#define HIB_AUTOSLEEP_DEBUG 0
extern bool console_suspend_enabled; // from printk.c
#define _TAG_HIB_M "HIB/AUTOSLEEP"
#undef pm_log
#if (HIB_AUTOSLEEP_DEBUG)
#define pm_log(fmt, ...) if (!console_suspend_enabled) pr_warn("[%s][%s]" fmt, _TAG_HIB_M, __func__, ##__VA_ARGS__);
#else
#define pm_log(fmt, ...)
#endif
#undef pm_warn
#define pm_warn(fmt, ...) if (!console_suspend_enabled) pr_warn("[%s][%s]" fmt, _TAG_HIB_M, __func__, ##__VA_ARGS__);

static suspend_state_t autosleep_state;
static struct workqueue_struct *autosleep_wq;
/*
 * Note: it is only safe to mutex_lock(&autosleep_lock) if a wakeup_source
 * is active, otherwise a deadlock with try_to_suspend() is possible.
 * Alternatively mutex_lock_interruptible() can be used.  This will then fail
 * if an auto_sleep cycle tries to freeze processes.
 */
static DEFINE_MUTEX(autosleep_lock);
static struct wakeup_source *autosleep_ws;


#ifdef CONFIG_SUSPEND_SYNC_WORKQUEUE
static int suspend_sys_sync_count;
static DEFINE_SPINLOCK(suspend_sys_sync_lock);
static struct workqueue_struct *suspend_sys_sync_work_queue;
static DECLARE_COMPLETION(suspend_sys_sync_comp);
#endif /*  CONFIG_SUSPEND_SYNC_WORKQUEUE */

#ifdef CONFIG_MTK_HIBERNATION
extern bool system_is_hibernating;
extern int mtk_hibernate_via_autosleep(suspend_state_t *autoslp_state);
#endif

#ifdef  CONFIG_SUSPEND_SYNC_WORKQUEUE
static void suspend_sys_sync(struct work_struct *work)
{
	
	printk(KERN_INFO "PM: Syncing filesystems ... ");

	sys_sync();

	printk("sync done.\n");

	spin_lock(&suspend_sys_sync_lock);
	suspend_sys_sync_count--;
	spin_unlock(&suspend_sys_sync_lock);
}
static DECLARE_WORK(suspend_sys_sync_work, suspend_sys_sync);

void suspend_sys_sync_queue(void)
{
	int ret;

	spin_lock(&suspend_sys_sync_lock);
	ret = queue_work(suspend_sys_sync_work_queue, &suspend_sys_sync_work);
	if (ret)
		suspend_sys_sync_count++;
	spin_unlock(&suspend_sys_sync_lock);
}

static bool suspend_sys_sync_abort;
static void suspend_sys_sync_handler(unsigned long);
static DEFINE_TIMER(suspend_sys_sync_timer, suspend_sys_sync_handler, 0, 0);
/* value should be less then half of input event wake lock timeout value
 * which is currently set to 5*HZ (see drivers/input/evdev.c)
 */
#define SUSPEND_SYS_SYNC_TIMEOUT (HZ/4)
static void suspend_sys_sync_handler(unsigned long arg)
{
	unsigned int count;


	if (suspend_sys_sync_count == 0) {
		complete(&suspend_sys_sync_comp);

	} else if (pm_get_wakeup_count(&count, false)) {
		suspend_sys_sync_abort = true;
		complete(&suspend_sys_sync_comp);
	} else {
		mod_timer(&suspend_sys_sync_timer, jiffies +
				SUSPEND_SYS_SYNC_TIMEOUT);
	}
}

int suspend_sys_sync_wait(void)
{
	suspend_sys_sync_abort = false;

	if (suspend_sys_sync_count != 0) {
		mod_timer(&suspend_sys_sync_timer, jiffies +
				SUSPEND_SYS_SYNC_TIMEOUT);
		wait_for_completion(&suspend_sys_sync_comp);
	}
	if (suspend_sys_sync_abort) {
		pr_info("suspend aborted....while waiting for sys_sync\n");
		return -EAGAIN;
	}

	return 0;
}
#endif /*  CONFIG_SUSPEND_SYNC_WORKQUEUE */
static void try_to_suspend(struct work_struct *work)
{
	unsigned int initial_count, final_count;

	pm_log("pm_get_wakeup_count\n");

	if (!pm_get_wakeup_count(&initial_count, true))
		goto out;

	mutex_lock(&autosleep_lock);

	pm_log("pm_save_wakeup_count\n");
	if (!pm_save_wakeup_count(initial_count)) {
		mutex_unlock(&autosleep_lock);
		goto out;
	}

	if (autosleep_state == PM_SUSPEND_ON) {
        pm_warn("leaving state(%d)\n", autosleep_state);
#ifdef CONFIG_MTK_HIBERNATION
        system_is_hibernating = false;
#endif
		mutex_unlock(&autosleep_lock);
		return;
	}

#ifdef CONFIG_MTK_HIBERNATION
	if (autosleep_state >= PM_SUSPEND_MAX) {
        mtk_hibernate_via_autosleep(&autosleep_state);
    }
    else {
		pm_log("pm_suspend: state(%d)\n", autosleep_state);
        if (!system_is_hibernating) {
            pm_warn("calling pm_suspend() state(%d)\n", autosleep_state);
            pm_suspend(autosleep_state);
        }
        else {
            pm_warn("system is hibernating: so changing state(%d->%d)\n",  autosleep_state, PM_SUSPEND_MAX);
            autosleep_state = PM_SUSPEND_MAX;
        }
	}
#else // !CONFIG_MTK_HIBERNATION
    if (autosleep_state >= PM_SUSPEND_MAX)
        hibernate();
    else {
        pm_log("pm_suspend\n");
        pm_suspend(autosleep_state);
    }
#endif // CONFIG_MTK_HIBERNATION
	mutex_unlock(&autosleep_lock);

	if (!pm_get_wakeup_count(&final_count, false))
		goto out;

	/*
	 * If the wakeup occured for an unknown reason, wait to prevent the
	 * system from trying to suspend and waking up in a tight loop.
	 */
	if (final_count == initial_count)
		schedule_timeout_uninterruptible(HZ / 2);

 out:
	pm_log("queue_up_suspend_work again\n");
	queue_up_suspend_work();
}

static DECLARE_WORK(suspend_work, try_to_suspend);

void queue_up_suspend_work(void)
{
	if (!work_pending(&suspend_work) && autosleep_state > PM_SUSPEND_ON)
	{
		pm_log("queue_work autosleep_state(%d)\n", autosleep_state);
		queue_work(autosleep_wq, &suspend_work);
	}
}

suspend_state_t pm_autosleep_state(void)
{
	return autosleep_state;
}

int pm_autosleep_lock(void)
{
	return mutex_lock_interruptible(&autosleep_lock);
}

void pm_autosleep_unlock(void)
{
	mutex_unlock(&autosleep_lock);
}

int pm_autosleep_set_state(suspend_state_t state)
{

#ifndef CONFIG_HIBERNATION
	if (state >= PM_SUSPEND_MAX)
		return -EINVAL;
#endif

	__pm_stay_awake(autosleep_ws);

	mutex_lock(&autosleep_lock);

	autosleep_state = state;

	__pm_relax(autosleep_ws);

	if (state > PM_SUSPEND_ON) {
		pm_log("pm_wakeup_autosleep_enable[true] queue_up_suspend_work");
		pm_wakep_autosleep_enabled(true);
		queue_up_suspend_work();
	} else {
		pm_log("pm_wakeup_autosleep_enable[false]");
		pm_wakep_autosleep_enabled(false);
	}

	mutex_unlock(&autosleep_lock);
	return 0;
}

int __init pm_autosleep_init(void)
{

#ifdef CONFIG_SUSPEND_SYNC_WORKQUEUE
	INIT_COMPLETION(suspend_sys_sync_comp);
	suspend_sys_sync_work_queue =
		create_singlethread_workqueue("suspend_sys_sync");
	if (suspend_sys_sync_work_queue == NULL) {
		return -ENOMEM;
	}
#endif /* CONFIG_SUSPEND_SYNC_WORKQUEUE */


	autosleep_ws = wakeup_source_register("autosleep");
	if (!autosleep_ws)
		return -ENOMEM;

	autosleep_wq = alloc_ordered_workqueue("autosleep", 0);
	if (autosleep_wq)
		return 0;


	wakeup_source_unregister(autosleep_ws);
	return -ENOMEM;
}
