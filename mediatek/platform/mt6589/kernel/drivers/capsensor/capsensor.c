/**
  ISQ128 capture sensor driver
*/
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include "cust_gpio_usage.h"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <cust_eint.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/hrtimer.h>
#include <linux/device.h>

//hwmsensor interface header defined here
#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>

#include <linux/io.h>
#include <asm/io.h>
#include "capsensor.h"

#define MAJOR_NUM 219
#define capsdev "capsdev"
#define CUST_EINT_CAPS_SENSITIVE 3
#if 0
#define EINT169  169
#else
#define EINT169 CUST_EINT_CAPSENSOR_EINT_NUM
#endif

#define DISABLE_CAPSENSOR 0
#define ENABLE_CAPSENSOR 1

#ifndef HARDWARE_RF_REQUIRE
#define HARDWARE_RF_REQUIRE
#endif

extern int IsCameraOk;

//EXPORT_SYMBOL(IsCameraOk);

static s32 capvalue = 0; // 1 means touched otherwise is 0
static CAPSENSOR_priv *cap_iqs128_obj = NULL;
static struct platform_driver capsensor_iqs128_driver;


//declared function here
extern void mt65xx_eint_mask(unsigned int line);
static int iqs_resume(struct platform_device *pdev);
static int iqs_suspend(struct platform_device *pdev, pm_message_t state);
static int get_sensorValue();
static bool cal_load_delay(bool reset);
static int iqs_caps_create_attr(struct device_driver *driver);
static int iqs_caps_remove_attr(struct device_driver *driver);
static int caps_ioctl(struct file *file, int cmd,  int arg);


//deamon thread
#ifdef HARDWARE_RF_REQUIRE
static int lookupCameraOn(void *p)
{
   daemonize("mythread");
   while(1) {
        printk("%s:%d IsCameraOk %d\n", __FILE__, __LINE__, IsCameraOk);
        if(IsCameraOk) {
           //hwPowerOn(MT65XX_POWER_LDO_VGP6, 3300, "capsensor");
 	   iqs_resume(NULL);


           //mt65xx_eint_mask(EINT169);
           //hwPowerDown(MT65XX_POWER_LDO_VGP6, "capsensor");
           if(cap_iqs128_obj) {
	      atomic_set(&cap_iqs128_obj->onoff, 1);
	   }
        }
             
        if(cal_load_delay(false)) 
	   break;
	
        //msleep(100);
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(10*HZ);
   }
   return 0;
}
#endif



int cpasensor_operate(void *self, uint32_t command, void *buff_in, int size_in,
		void *buff_out, int size_out, int*actualout)
{
	int err = 0;
	hwm_sensor_data *sensor_data;
	PCAPSENSOR_priv pcap_priv = (PCAPSENSOR_priv)self;

	switch(command) {
	case SENSOR_DELAY:
		printk("cap-sensor operate function don't implement this command \"SENSOR_DELAY\"\n");
		break;
	case SENSOR_ENABLE:
		printk("cap-sensor operate function don't implement this command \"SENSOR_ENABLE\"\n");
		break;
	case SENSOR_GET_DATA:
		if((buff_out == NULL) || (size_out < sizeof(hwm_sensor_data))) {
			printk("get sensor data parameter error!\n");
			err = -EINVAL;
		}
		else {
			sensor_data = (hwm_sensor_data *)buff_out;
			sensor_data->values[0] = capvalue; //pcap_priv->sensor_val;
			sensor_data->value_divide = 1;
			sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
		}
		break;
	default:
		printk("cap-sensor operate function no this parameter %d\n", command);
		err = -1;
		break;
	};
	return err;
}


//define fops here
static int caps_read(struct inode *inode, struct file *file)
{
	return 0;
}

static int caps_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int caps_release(struct inode *indoe, struct file *file)
{
	return 0;
}
static int caps_ioctl(struct file *file, int cmd,  int arg)
{
   int power = 0;
   printk("%s:%d cmd %d\n", __FUNCTION__, __LINE__, cmd);
   
   if(cap_iqs128_obj == NULL)
	return 0;

   power = atomic_read(&cap_iqs128_obj->onoff);

   switch(cmd) {
   case ENABLE_CAPSENSOR:
      if(!power) {
         iqs_resume(NULL);
         atomic_set(&cap_iqs128_obj->onoff, 1);
         printk("%s: cmd ENABLE_CAPSENSOR\n", __FILE__);
      }
      break;
   case DISABLE_CAPSENSOR:
      if(power) {
	 hwPowerDown(MT65XX_POWER_LDO_VGP6, "capsensor");
         atomic_set(&cap_iqs128_obj->onoff, 0);
         printk("%s: cmd DISABLE_CAPSENSOR\n", __FILE__);
      }
      break;
   default: 
      break;
   };
   return 0;
}


struct file_operations capsensor_fops =
{
   .owner = THIS_MODULE,
   .open = caps_open,
   .read = caps_read,
   .release = caps_release,
   .unlocked_ioctl = caps_ioctl,
};

static struct miscdevice misc_caps_dev = {
   .minor = MISC_DYNAMIC_MINOR,
   .name = capsdev,
   .fops = &capsensor_fops, 
};


extern void mt65xx_eint_unmask(unsigned int line);
//extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
extern void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol);

#if 0
#define GPIO_CAPS_EINT_PIN GPIO189
#else
#define GPIO_CAPS_EINT_PIN GPIO_CAPSENSOR_PIN
#endif

#define  DBG_LEVEL


////////////////////////////////////////////////////////////////
//capsensor interrupt funcitons defined here
	static void caps_eint_interrupt_handler(struct work_struct *work);
        static void caps_eint_down_handler(struct work_struct *work);
//other static funcs declare here
	static void caps_close_gpio(void);
	static void caps_open_gpio(void);
	static void caps_eint_init(void);
	static int caps_init(void);
	static int caps_exit(void);
        static void eint_work_func(void);
/////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////
//close gpio
static void caps_close_gpio(void)
{
#ifdef DBG_LEVEL
   printk("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
   mt_set_gpio_mode(GPIO_CAPS_EINT_PIN, GPIO_MODE_00); //02 iinterrupt 0 gpio mode 
   mt_set_gpio_dir(GPIO_CAPS_EINT_PIN, GPIO_DIR_IN);
//   mt_set_gpio_out(GPIO189, GPIO_OUT_ZERO);   
}

//intialize gipo 
static void caps_open_gpio(void)
{
  int err = 0;
  struct hwmsen_object obj_sensor;
  CAPSENSOR_priv *obj;
  
  if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL))) {
	err = -ENOMEM;
 	goto exit;
  }
  memset(obj, 0, sizeof(*obj));
  obj->sensor_val = 0; //init to 0

  //for interrup work mode support
  INIT_DELAYED_WORK(&obj->eint_work, caps_eint_interrupt_handler);
  INIT_DELAYED_WORK(&obj->eint_work_down, caps_eint_down_handler);
  cap_iqs128_obj = obj;


  //power state machine
  atomic_set(&cap_iqs128_obj->onoff, 0);

  obj_sensor.polling = 0; 
  obj_sensor.self = cap_iqs128_obj;
  obj_sensor.sensor_operate = cpasensor_operate;
  if((err = hwmsen_attach(ID_CAPSENSOR, &obj_sensor))) {
	printk("%s:%dcap-sensor attach fail = %d\n",  __FILE__, __LINE__, err);
	goto exit;
  }


  msleep(10);

#ifdef DBG_LEVEL
   printk("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
#if 1
    mt_set_gpio_mode(GPIO_CAPS_EINT_PIN, GPIO_MODE_02);
//    mt_set_gpio_dir(GPIO189, GPIO_DIR_IN);
//    mt_set_gpio_out(GPIO189, GPIO_OUT_ONE);
//    mt_set_gpio_pull_enable(GPIO189, GPIO_PULL_DISABLE);

//    mt_set_gpio_out(GPIO189, GPIO_OUT_ZERO);  
//    msleep(10);  
//    mt_set_gpio_out(GPIO189, GPIO_OUT_ONE);


#endif
#ifdef DBG_LEVEL
   printk("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
#endif

  //EINT 
  mt65xx_eint_set_sens(EINT169, CUST_EINT_LEVEL_SENSITIVE); //edge sensitive
  //mt65xx_eint_set_sens(EINT169, CUST_EINT_EDGE_SENSITIVE); //LEVEL sensitive
  //mt65xx_eint_set_hw_debounce(EINT169, 3); //times 3ms
  //mt65xx_eint_registration(EINT169, 1, CUST_EINT_POLARITY_LOW, caps_eint_interrupt_handler, 1);


//  mt65xx_eint_registration(EINT169, 0, CUST_EINT_POLARITY_HIGH, caps_eint_interrupt_handler, 1);
  mt65xx_eint_registration(EINT169, 1, CUST_EINT_POLARITY_HIGH, eint_work_func, 0);
  mt65xx_eint_unmask(EINT169);

  return;
exit:
	kfree(obj);

}

//binding eint169 to gpio189
static void caps_eint_init(void)
{
#ifndef TEST_IN_MIPI
   volatile unsigned int * p;
   //hwPowerOn(MT65XX_POWER_LDO_VGP6, 3300, "capsensor");
   hwPowerDown(MT65XX_POWER_LDO_VGP6, "capsensor");
   p = (volatile unsigned int *)(0xf00051b0);
   *p |= (1 << 13);
#else
   hwPowerOn(MT65XX_POWER_LDO_VGP6, 3300, "capsensor");
#endif
   
   caps_open_gpio();
}

static void eint_work_func(void)
{
    struct CAPSENSOR_priv *obj = cap_iqs128_obj; 
    uint32_t time_delay_ms;
    int value;
    time_delay_ms;
    int power = 0;

    //if power state was down then go away
    power = atomic_read(&cap_iqs128_obj->onoff);
    if(!power)    
	return;

    mt65xx_eint_mask(EINT169);
    if(!obj)
	return;

    value = get_sensorValue();
    printk("%s:%d capvalue %d iscameraok %d\n", __FILE__, __LINE__, value, IsCameraOk);
    if(!value) {
            //cal_load_delay(true);
	    time_delay_ms = 3000;
	    schedule_delayed_work(&obj->eint_work, msecs_to_jiffies(time_delay_ms) + 1);	
    }
    else {
        //    bool keeps = cal_load_delay(false);
        //    if(!keeps) { 
		cancel_delayed_work(&obj->eint_work);
 	//	printk("%d cancel delay work\n", __LINE__);
	//    }
	    time_delay_ms = 10;
	    schedule_delayed_work(&obj->eint_work_down, msecs_to_jiffies(time_delay_ms) + 1);	
   }
   mt65xx_eint_unmask(EINT169);
}

static bool cal_load_delay(bool reset)
{
#define VIBRATE_DELAY_60S 60
    static unsigned long prevUs = 0;
    static unsigned long vibSec = 0;
    struct timeval curr;

    do_gettimeofday(&curr);
    if(reset) {
	vibSec = 0;
	prevUs = 0;	
    }

    if(prevUs && prevUs < curr.tv_sec) {
	vibSec += curr.tv_sec - prevUs;
    } 
    prevUs = curr.tv_sec;
    if(vibSec >= VIBRATE_DELAY_60S)
	return true;
    else
	return false; 
}

static void caps_eint_down_handler(struct work_struct *work)
{
    hwm_sensor_data sensor_data;
    u8 databuf[3];
    int pollvalue;

    //callback sensor data
    sensor_data.values[0] = 1;
    sensor_data.value_divide = 1;
    sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
    if(hwmsen_get_interrupt_data(ID_CAPSENSOR, &sensor_data)) {
         printk("%s:%d call hwmsen_get_interrupt_data fail\n", __FILE__, __LINE__);
    }
}

static void caps_eint_interrupt_handler(struct work_struct *work)
{

    hwm_sensor_data sensor_data;
    u8 databuf[3];
    int pollvalue;
    
    //callback sensor data
    sensor_data.values[0] = 0;
    sensor_data.value_divide = 1;
    sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
    if(hwmsen_get_interrupt_data(ID_CAPSENSOR, &sensor_data)) { 
         printk("%s:%d call hwmsen_get_interrupt_data fail\n", __FILE__, __LINE__);	
    }
}


static int caps_init()
{
   int ret = 0;
   //ret = register_chrdev(MAJOR_NUM, capsdev, &capsensor_fops);
   ret = misc_register(&misc_caps_dev);
   if(ret) {
      printk("capsensor_fops register failure");      
   }
   else {
      printk("capsensor_fops register successfully");

   }

   caps_eint_init();
#ifdef DBG_LEVEL
   printk("capsensor initialize\n");
#endif
   return ret;
}

static int caps_exit()
{
   int ret;
   
   misc_deregister(&misc_caps_dev);
   hwmsen_detach(ID_CAPSENSOR); 
   if(!cap_iqs128_obj) 
	kfree(cap_iqs128_obj);
   cap_iqs128_obj = NULL;
   
/**
   ret = unregister_chrdev_region(MAJOR_NUM, capsdev);
   if(ret) {
      printk("capsensor_fops unregister failure\n");
   }
   else {
      printk("capsensor_fops unregister successfully\n");
   }
*/  
   caps_close_gpio();
#ifdef DBG_LEVEL
   printk("capsensor exit\n");
#endif
   return 0;
}


/////////////////////////////////////////////////////////////////
static int iqs_probe(struct platform_device *pdev)
{
	//struct CAPSENSOR_priv *obj;


#ifdef HARDWARE_RF_REQUIRE
	//printk("%s:%s\n", __FILE__, __LINE__);
        cal_load_delay(true);
	//printk("%s:%s\n", __FILE__, __LINE__);
        kernel_thread(lookupCameraOn, NULL, CLONE_KERNEL | SIGCHLD);
	//printk("%s:%s\n", __FILE__, __LINE__);
#endif

        //for interrup work mode support
        //INIT_WORK()
        iqs_caps_create_attr(&capsensor_iqs128_driver.driver);
	return caps_init();
}

static int iqs_remove(struct platform_device *pdev)
{
	iqs_caps_remove_attr(&capsensor_iqs128_driver.driver);
	return caps_exit();
}

static int iqs_suspend(struct platform_device *pdev, pm_message_t state)
{
       printk("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
       mt65xx_eint_mask(EINT169);
       hwPowerDown(MT65XX_POWER_LDO_VGP6, "capsensor");
       return 0;
}

static int get_sensorValue()
{
        mt_set_gpio_mode(GPIO_CAPS_EINT_PIN, GPIO_MODE_00); //02 interrupt 0 gpio mode
        mt_set_gpio_dir(GPIO_CAPS_EINT_PIN, GPIO_DIR_IN);
        capvalue = mt_get_gpio_in(GPIO_CAPS_EINT_PIN);
        mt_set_gpio_mode(GPIO_CAPS_EINT_PIN, GPIO_MODE_02); //02 iinterrupt 0 gpio mode
        mt_set_gpio_dir(GPIO_CAPS_EINT_PIN, GPIO_DIR_OUT);
        mt65xx_eint_set_sens(EINT169, CUST_EINT_LEVEL_SENSITIVE); //LEVEL sensitive 
        if(capvalue) {
                mt65xx_eint_registration(EINT169, 1, CUST_EINT_POLARITY_LOW, eint_work_func, 0);
        }
        else {  
                mt65xx_eint_registration(EINT169, 1, CUST_EINT_POLARITY_HIGH, eint_work_func, 0);
        }
//        mt65xx_eint_unmask(EINT169);
        return capvalue;
}

static int iqs_resume(struct platform_device *pdev)
{
       printk("%s:%s:%d\n", __FILE__, __FUNCTION__, __LINE__);
       mt65xx_eint_mask(EINT169);
       hwPowerOn(MT65XX_POWER_LDO_VGP6, 3300, "capsensor");
       mt_set_gpio_mode(GPIO_CAPS_EINT_PIN, GPIO_MODE_00); //02 interrupt 0 gpio mode
       mt_set_gpio_dir(GPIO_CAPS_EINT_PIN, GPIO_DIR_IN);
       capvalue = mt_get_gpio_in(GPIO_CAPS_EINT_PIN);
       mt_set_gpio_mode(GPIO_CAPS_EINT_PIN, GPIO_MODE_02); //02 iinterrupt 0 gpio mode
       mt_set_gpio_dir(GPIO_CAPS_EINT_PIN, GPIO_DIR_OUT);
       mt65xx_eint_set_sens(EINT169, CUST_EINT_LEVEL_SENSITIVE); //LEVEL sensitive 
       if(capvalue) {
	       mt65xx_eint_registration(EINT169, 1, CUST_EINT_POLARITY_LOW, eint_work_func, 0);
       }
       else {
	       mt65xx_eint_registration(EINT169, 1, CUST_EINT_POLARITY_HIGH, eint_work_func, 0);
       }
       mt65xx_eint_unmask(EINT169);
       return 0;
}


static ssize_t show_caps_onoff_value(struct device_driver *ddri, char *buf)
{
       printk("%s:%d\n", __FUNCTION__, __LINE__);
       return 0;
}

static ssize_t store_caps_onoff_value(struct device_driver *ddri, const char *buf, size_t count)
{
    int err;
    u8 cmd[4];
    if(sysfs_streq(buf, "1")) {
        caps_ioctl(NULL, 1, 0);
	printk("%s:%d\n", __FUNCTION__, __LINE__);
    }
    else if(sysfs_streq(buf, "0")){
        caps_ioctl(NULL, 0, 0);
	printk("%s:%d\n", __FUNCTION__, __LINE__);
    }
    else
	printk("%s:%d\n", __FUNCTION__, __LINE__);
	
   return count;
}

/**
  Forward declare sysfs interface using by hardware testing
*/
static DRIVER_ATTR(onoff, S_IWUSR | S_IRUGO, show_caps_onoff_value, store_caps_onoff_value);

static struct driver_attribute *caps_attr_list[] = {
	&driver_attr_onoff // capensor ic power on/off setting interface 
};


/**
  Add attribute for driver caps 
*/
static int iqs_caps_create_attr(struct device_driver *driver)
{
   int idx, err = 0;
   int sum = (int)(sizeof(caps_attr_list)/sizeof(caps_attr_list[0]));
   if(driver == NULL)
	return -EINVAL;
   for(idx = 0; idx < sum; idx++) {	
	if(err == driver_create_file(driver, caps_attr_list[idx])) {	
	   printk("%s:%d driver_create_file(%s) = %d\n", caps_attr_list[idx]->attr.name, err);
           continue;
	}
   }
   return err;
}

/**
 delete attribute for driver caps
*/
static int iqs_caps_remove_attr(struct device_driver *driver)
{
   int idx, err = 0;
   int sum = (int)(sizeof(caps_attr_list)/sizeof(caps_attr_list[0]));
   if(driver == NULL)
	return -EINVAL;
   for(idx = 0; idx < sum; idx++) {
	driver_remove_file(driver, caps_attr_list[idx]);	
   }
   return err;
}



static struct platform_driver capsensor_iqs128_driver = {
	.probe = iqs_probe,
	.remove = iqs_remove,
	.suspend = iqs_suspend,
	.resume = iqs_resume,
	.driver = {
		.name = "iqs_capsensor",
	}
};



static int __init iqs_caps_init(void)
{
	if(platform_driver_register(&capsensor_iqs128_driver)) {
		printk("failed to register driver iqs capsensor\n");
		return -ENODEV;
	}
	return 0;
} 

static int __exit iqs_caps_exit(void)
{
	platform_driver_unregister(&capsensor_iqs128_driver);
}

module_init(iqs_caps_init);
module_exit(iqs_caps_exit);
MODULE_LICENSE("copyright by lenovo corp");
MODULE_AUTHOR("lipeng1");
