#ifndef _CUST_BAT_H_
#define _CUST_BAT_H_

#include <platform/mt_typedefs.h>

typedef struct{	
	unsigned int BattVolt;
	unsigned int BattPercent;
}VBAT_TO_PERCENT;

/* Battery Temperature Protection */
#define MAX_CHARGE_TEMPERATURE  50
#define MIN_CHARGE_TEMPERATURE  0
#define ERR_CHARGE_TEMPERATURE  0xFF

/* Recharging Battery Voltage */
#define RECHARGING_VOLTAGE      4110

/* Charging Current Setting */
#define USB_CHARGER_CURRENT					500
#ifdef S6000L_ROW_COMMON
#define AC_CHARGER_CURRENT                                      1524
#else
#define AC_CHARGER_CURRENT					2036
#endif

/* Battery Meter Solution */
#define CONFIG_ADC_SOLUTION 	1

/* Battery Voltage and Percentage Mapping Table */
VBAT_TO_PERCENT Batt_VoltToPercent_Table[] = {
	/*BattVolt,BattPercent*/
	{3400,0},
	{3686,10},
	{3740,20},
	{3771,30},
	{3789,40},
	{3820,50},
	{3873,60},
	{3943,70},
	{4013,80},
	{4100,90},
	{4189,100},
};

/* Precise Tunning */
//#define BATTERY_AVERAGE_SIZE 	600
#define BATTERY_AVERAGE_SIZE 	60


#define CHARGING_IDLE_MODE	 1

#define CHARGING_PICTURE	 1

/* Common setting */
#define R_CURRENT_SENSE 2				// 0.2 Ohm
#define R_BAT_SENSE 4					// times of voltage
#define R_I_SENSE 4						// times of voltage
#define R_CHARGER_1 330
#define R_CHARGER_2 39
#define R_CHARGER_SENSE ((R_CHARGER_1+R_CHARGER_2)/R_CHARGER_2)	// times of voltage
//#define V_CHARGER_MAX 6000				// 6 V
#define V_CHARGER_MAX 6000				// 7 V->6V
#define V_CHARGER_MIN 4400				// 4.4 V
#define V_CHARGER_ENABLE 0				// 1:ON , 0:OFF
#define BACKLIGHT_KEY 10					// camera key

/* Teperature related setting */
#define RBAT_PULL_UP_R             39000
//#define RBAT_PULL_UP_VOLT          2500
#define RBAT_PULL_UP_VOLT          1800
#define TBAT_OVER_CRITICAL_LOW     68237
//#define TBAT_OVER_CRITICAL_LOW     483954
#define BAT_TEMP_PROTECT_ENABLE    1
#define BAT_NTC_10 1
#define BAT_NTC_47 0

/* The option of new charging animation */
#define ANIMATION_NEW

#endif /* _CUST_BAT_H_ */ 
