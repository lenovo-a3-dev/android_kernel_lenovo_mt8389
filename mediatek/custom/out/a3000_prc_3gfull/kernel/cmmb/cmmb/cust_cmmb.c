/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
#include <cust_cmmb.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <mach/eint.h>
#include <cust_eint.h>
#include <linux/module.h>
#include <linux/spi/spi.h>

#include <mach/mt_pm_ldo.h>

#define inno_msg(fmt, arg...)	printk(KERN_ERR "[cmmb-drv]%s: " fmt "\n", __func__, ##arg)
void cust_cmmb_power_on(void)
{
	hwPowerOn(MT65XX_POWER_LDO_VGP5,VOL_1200,"CMMB");
	hwPowerOn(MT65XX_POWER_LDO_VGP6,VOL_1800,"CMMB");

	mt_set_gpio_out(GPIO_ANT_SW_PIN, GPIO_OUT_ZERO); 

	//mt_set_gpio_mode(GPIO_CMMB_LDO_EN_PIN, GPIO_CMMB_LDO_EN_PIN_M_GPIO);
	//mt_set_gpio_dir(GPIO_CMMB_LDO_EN_PIN, GPIO_DIR_OUT);
	//mt_set_gpio_out(GPIO_CMMB_LDO_EN_PIN, GPIO_OUT_ONE);  
//	mt_set_gpio_pull_enable(GPIO_CMMB_LDO_EN_PIN, 1);                              // no need to pull, beause BB output power is enough
//	mt_set_gpio_pull_select(GPIO_CMMB_LDO_EN_PIN,  1);
	//inno_msg("CMMB GPIO LDO PIN mode:num:%d, %d,out:%d, dir:%d,pullen:%d,pullup%d",GPIO_CMMB_LDO_EN_PIN,mt_get_gpio_mode(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_out(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_dir(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_pull_enable(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_pull_select(GPIO_CMMB_LDO_EN_PIN));    

}
EXPORT_SYMBOL(cust_cmmb_power_on);

void cust_cmmb_power_off(void)
{
	mt_set_gpio_out(GPIO_ANT_SW_PIN, GPIO_OUT_ONE); 

	hwPowerDown(MT65XX_POWER_LDO_VGP5,"CMMB");
	hwPowerDown(MT65XX_POWER_LDO_VGP6,"CMMB");

	//mt_set_gpio_mode(GPIO_CMMB_LDO_EN_PIN, GPIO_CMMB_LDO_EN_PIN_M_GPIO);
	//mt_set_gpio_out(GPIO_CMMB_LDO_EN_PIN, GPIO_OUT_ZERO);  
	//mt_set_gpio_dir(GPIO_CMMB_LDO_EN_PIN, GPIO_DIR_IN);
	//inno_msg("CMMB GPIO LDO PIN mode:num:%d, %d,out:%d, dir:%d,pullen:%d,pullup%d",GPIO_CMMB_LDO_EN_PIN,mt_get_gpio_mode(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_out(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_dir(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_pull_enable(GPIO_CMMB_LDO_EN_PIN),mt_get_gpio_pull_select(GPIO_CMMB_LDO_EN_PIN));    

	mt_set_gpio_mode(GPIO_CMMB_RST_PIN, GPIO_CMMB_RST_PIN_M_GPIO);
	mt_set_gpio_out(GPIO_CMMB_RST_PIN, GPIO_OUT_ZERO); 			 
	mt_set_gpio_dir(GPIO_CMMB_RST_PIN, GPIO_DIR_IN);
	inno_msg("CMMB GPIO RST PIN mode:num:%d, %d,out:%d, dir:%d,pullen:%d,pullup%d",GPIO_CMMB_RST_PIN,mt_get_gpio_mode(GPIO_CMMB_RST_PIN),mt_get_gpio_out(GPIO_CMMB_RST_PIN),mt_get_gpio_dir(GPIO_CMMB_RST_PIN),mt_get_gpio_pull_enable(GPIO_CMMB_RST_PIN),mt_get_gpio_pull_select(GPIO_CMMB_RST_PIN));    	 
}
EXPORT_SYMBOL(cust_cmmb_power_off);

static struct spi_board_info spi_board_devs __initdata = {
	.modalias="cmmb-spi",
	.bus_num = 0,
	.chip_select=0,
	.mode = SPI_MODE_3,
};
static int __init init_cust_cmmb_spi()
{
      spi_register_board_info(&spi_board_devs, 1);
      return 0;
}
static void __exit exit_cust_cmmb_spi()
{
       return;
}
module_init(init_cust_cmmb_spi);
module_exit(exit_cust_cmmb_spi);
