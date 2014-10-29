#ifndef TOUCHPANEL_H__
#define TOUCHPANEL_H__

/* Pre-defined definition */
#define TPD_TYPE_CAPACITIVE
#define TPD_TYPE_RESISTIVE
#define TPD_POWER_SOURCE         
#define TPD_I2C_NUMBER           0
#define TPD_WAKEUP_TRIAL         60
#define TPD_WAKEUP_DELAY         100

#define TPD_DELAY                (2*HZ/100)
//#define TPD_RES_X                480
//#define TPD_RES_Y                800
//#define TPD_CALIBRATION_MATRIX  {-1980,0,1980*993,0,1805,0,0,0};
//#define TPD_CALIBRATION_MATRIX_NORMAL   {-2618,0,4190208,0,-958,2453504,0,0};
//#define TPD_CALIBRATION_MATRIX_NORMAL   {-3274,0,5238784,0,-1278,3272704,0,0};
#define TPD_CALIBRATION_MATRIX_NORMAL   {1279,0,0,0,1278,0,0,0};
#define TPD_CALIBRATION_MATRIX_FACTORY  {1279,0,0,0,1278,0,0,0};


#define TPD_HAVE_CALIBRATION
#define TPD_HAVE_BUTTON
//#define TPD_HAVE_TREMBLE_ELIMINATION
#define TPD_KEY_COUNT           3
#define TPD_KEYS                { KEY_BACK, KEY_HOME,KEY_MENU}
#define TPD_KEYS_DIM            {{80,850,160,100},{240,850,160,100},{400,850,160,100}}

/* Register */
#define RMI_PAGE_SELECT_REGISTER 0xff
#define FD_ADDR_MAX    	         0xE9
#define FD_ADDR_MIN    	         0x05
#define FD_BYTE_COUNT            6

/* f01 device control register bits */
#define RMI_SLEEP_MODE_NORMAL       0x80
#define RMI_SLEEP_MODE_SENSOR_SLEEP 0x81
#define RMI_SLEEP_MODE_RESERVED0    0x82
#define RMI_SLEEP_MODE_RESERVED1    0x83
#define RMI_DEVICE_RESET_CMD        0x01

#define RMI_I2C_PAGE(addr) (((addr) >> 8) & 0xff)


#define TPD_UPDATE_FIRMWARE

#endif /* TOUCHPANEL_H__ */

