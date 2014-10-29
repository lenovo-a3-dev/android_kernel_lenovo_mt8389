/************************************************************
 * hall.c - Hall sensor driver
 * 
 * Copyright Lenovo MIDH
 * 
 * DESCRIPTION:
 *     This file provid the hall sensor drivers 
 *
 ***********************************************************/
#ifdef LENOVO_HALL_SENSOR_SUPPORT
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/workqueue.h>
#include <linux/switch.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

//#ifdef MT6577 -david
#include <mtk_kpd.h>		/* custom file */
//#include <cust_kpd.h>
//#endif

#include <mach/irqs.h>
#include <mach/eint.h>
#include <mach/mt_gpio.h>

#define SET_HALL_STATUS _IOW('O', 10, unsigned int)
#define GET_HALL_STATUS _IOR('O', 11, unsigned int)

#define DEBUG
#ifdef DEBUG
#define DOCK_DEBUG printk
#else
#define DOCK_DEBUG
#endif

#define DOCK_ERR pr_err

//extern struct input_dev *kpd_input_dev;
extern BOOL is_early_suspended;
extern BOOL is_uboot_refresh;
extern BOOL BL_turn_on;

static u8 hall_cover_state = 0;
static BOOL has_hall_drv_file = false;
static struct switch_dev hall_data;


static void hall_cover_eint_handler(void);

static struct work_struct hall_eint_work;
static struct workqueue_struct * hall_eint_workqueue = NULL;



/******************************************************************************
Device driver structure
*****************************************************************************/
static struct platform_device hall_device =
{
    .name = "hall-sensor",
    .id   = -1,
};


/******************************************************************************
Global Definations
******************************************************************************/
static void hall_cover_eint_handler(void)
{
	queue_work(hall_eint_workqueue, &hall_eint_work);
	
}
static void hall_eint_work_callback(struct work_struct *work)
{
	bool cover;
	u8 old_state = hall_cover_state;
	
	mt65xx_eint_mask(CUST_EINT_MHALL_NUM);
	DOCK_DEBUG("[hall] hall_cover_state = %d\n", hall_cover_state );
        hall_cover_state = !mt_get_gpio_in(GPIO_MHALL_EINT_PIN);
	if ( old_state ==  hall_cover_state )
	{
		DOCK_ERR("[hall] no hall state changed!!!\n");
		//mt65xx_eint_unmask(CUST_EINT_MHALL_NUM);
		//return;
	}
	switch_set_state((struct switch_dev *)&hall_data, hall_cover_state);

	//input_report_switch(kpd_input_dev, SW_LID, hall_cover_state);
	//input_sync(kpd_input_dev);
	mt65xx_eint_set_polarity(CUST_EINT_MHALL_NUM, !old_state);
	mt65xx_eint_unmask(CUST_EINT_MHALL_NUM);
}

extern s32 mt_set_gpio_ies(u32 pin, u32 enable);
static void hall_init_hw(void)
{
	mt_set_gpio_mode(GPIO_MHALL_EINT_PIN, GPIO_MHALL_EINT_PIN_M_EINT);
	mt_set_gpio_dir(GPIO_MHALL_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_MHALL_EINT_PIN, GPIO_PULL_DISABLE); 
	mt_set_gpio_ies(GPIO_MHALL_EINT_PIN, GPIO_IES_ENABLE);

    	mt65xx_eint_set_sens(CUST_EINT_MHALL_NUM, CUST_EINT_MHALL_SENSITIVE);
	mt65xx_eint_set_polarity(CUST_EINT_MHALL_NUM, CUST_EINT_MHALL_POLARITY);
	mt65xx_eint_set_hw_debounce(CUST_EINT_MHALL_NUM, CUST_EINT_MHALL_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_MHALL_NUM, CUST_EINT_MHALL_DEBOUNCE_EN, CUST_EINT_MHALL_POLARITY, hall_cover_eint_handler, 0);
	mt65xx_eint_unmask(CUST_EINT_MHALL_NUM);  

}



static int hall_probe(struct platform_device *dev)	
{
	DOCK_DEBUG("[hall] %s  \n", __func__);
	hall_data.name = "hall";
        hall_data.index = 0;
        hall_data.state = hall_cover_state;

        switch_dev_register(&hall_data);

	
	hall_eint_workqueue = create_singlethread_workqueue("hall_eint");
       
        INIT_WORK(&hall_eint_work, hall_eint_work_callback);

	hall_init_hw();
	return 0;
}
static int hall_remove(struct platform_device *dev)	
{
	destroy_workqueue(hall_eint_workqueue);
	switch_dev_unregister(&hall_data);
	return 0;
}


static struct platform_driver hall_driver= {
        .probe          = hall_probe,
        .remove   = hall_remove,
        .driver     = {
        .name       = "hall-sensor",
        },
};


/******************************************************************************
 * hall_mod_init
 * 
 * DESCRIPTION:
 *   Register the hall sensor driver ! 
 * 
 * PARAMETERS: 
 *   None
 * 
 * RETURNS: 
 *   None
 * 
 * NOTES: 
 *   0 : Success
 * 
 ******************************************************************************/
static s32 __devinit hall_mod_init(void)
{	
	s32 ret;

	ret = platform_driver_register(&hall_driver);
	if (ret != 0){
		DOCK_ERR("[Hall]Unable to register hall sensor device (%d)\n", ret);
        	return ret;
	}

	DOCK_DEBUG("[Hall]hall_mod_init Done \n");
 
	return 0;
}

/******************************************************************************
 * hall_mod_exit
 * 
 * DESCRIPTION: 
 *   Free the device driver ! 
 * 
 * PARAMETERS: 
 *   None
 * 
 * RETURNS: 
 *   None
 * 
 * NOTES: 
 *   None
 * 
 ******************************************************************************/
 
static void __exit hall_mod_exit(void)
{
	
	platform_driver_unregister(&hall_driver);
	DOCK_DEBUG("[Hall]hall_mod_exit Done \n");
}

module_init(hall_mod_init);
module_exit(hall_mod_exit);
MODULE_AUTHOR("Lenovo");
MODULE_DESCRIPTION("Lenovo Hall Sensor Driver");
MODULE_LICENSE("GPL");
#endif
