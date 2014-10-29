cd ~/android/working
export CROSS_COMPILE=~/android/4.6.x-google/bin/arm-eabi-
./mk -o=TARGET_BUILD_VARIANT=user a3000_prc_3gfull n k
source pack_bootimage.sh a3000_prc_3gfull
cp ~/android/working/kernel/out/drivers/hid/hid-logitech-dj.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/hid-logitech-dj.ko
cp ~/android/working/kernel/out/drivers/scsi/scsi_wait_scan.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/scsi_wait_scan.ko.ko
cp ~/android/working/kernel/out/drivers/scsi/scsi_tgt.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/scsi_tgt.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/fmradio/mtk_fm_drv.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_fm_drv.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/dual_ccci/ccci.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/ccci.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_uart.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_stp_uart.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_bt.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_stp_bt.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_wmt.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_stp_wmt.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_wmt_wifi.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_wmt_wifi.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_hif_sdio.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_hif_sdio.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_gps.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtk_stp_gps.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/drv_wlan/mt6628/wlan/wlan_mt6628.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/wlan_mt6628.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/gpu/pvr/pvrsrvkm.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/pvrsrvkm.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/gpu/pvr/mtklfb.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/mtklfb.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/videocodec/vcodec_kernel_driver.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/vcodec_kernel_driver.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/masp/sec.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/sec.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/dual_ccci/ccci_plat.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/ccci_plat.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/devapc/devapc.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/devapc.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/devinfo/devinfo.ko ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/devinfo.ko
~/android/4.6.x-google/bin/arm-eabi-strip -g -S -d --strip-unneeded -x -I elf32-littlearm -O elf32-littlearm ~/android/working/out/target/product/a3000_prc_3gfull/system/lib/modules/*.ko