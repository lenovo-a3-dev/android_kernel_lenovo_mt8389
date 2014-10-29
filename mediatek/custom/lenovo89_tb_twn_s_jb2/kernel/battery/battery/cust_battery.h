#ifndef _CUST_BAT_H_
#define _CUST_BAT_H_


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
#define CONFIG_USB_IF 						0   
#define USB_CHARGER_CURRENT_SUSPEND			0		// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT_UNCONFIGURED	100	// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT_CONFIGURED		500	// def CONFIG_USB_IF
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
	{3641,10},
	{3708,20},
	{3741,30},
	{3765,40},
	{3793,50},
	{3836,60},
	{3891,70},
	{3960,80},
	{4044,90},
	{4183,100},
};

/* Precise Tunning */
#define BATTERY_AVERAGE_SIZE 	30
//#define BATTERY_AVERAGE_SIZE   3

/* Common setting */
#define R_CURRENT_SENSE 2				// 0.2 Ohm
#define R_BAT_SENSE 4					// times of voltage
#define R_I_SENSE 4						// times of voltage
#define R_CHARGER_1 330
#define R_CHARGER_2 39
#define R_CHARGER_SENSE ((R_CHARGER_1+R_CHARGER_2)/R_CHARGER_2)	// times of voltage
#define V_CHARGER_MAX 6000				// 7 V->6V
#define V_CHARGER_MIN 4400				// 4.4 V
#define V_CHARGER_ENABLE 0				// 1:ON , 0:OFF

/* Teperature related setting */
#define RBAT_PULL_UP_R             39000
//#define RBAT_PULL_UP_VOLT          2500
#define RBAT_PULL_UP_VOLT          1800
#define TBAT_OVER_CRITICAL_LOW     68237
//#define TBAT_OVER_CRITICAL_LOW     483954
#define BAT_TEMP_PROTECT_ENABLE    1
#define BAT_NTC_10 1
#define BAT_NTC_47 0

/* Battery Notify */
//#define BATTERY_NOTIFY_CASE_0001
//#define BATTERY_NOTIFY_CASE_0002
//#define BATTERY_NOTIFY_CASE_0003
//#define BATTERY_NOTIFY_CASE_0004
//#define BATTERY_NOTIFY_CASE_0005

//#define CONFIG_POWER_VERIFY

#endif /* _CUST_BAT_H_ */ 
