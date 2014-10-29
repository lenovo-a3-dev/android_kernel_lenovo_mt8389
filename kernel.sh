export PATH=/home/olivier/a3-kernel/toolchains/arm-eabi-4.6/bin:$PATH
export CROSS_COMPILE=arm-eabi-
export TARGET_PRODUCT=a3000_row_call
export FLASHABLE_ZIP="./kernel-$TARGET_PRODUCT-`date +"%Y%m%d"`.zip"

./mk -o=TARGET_BUILD_VARIANT=user $TARGET_PRODUCT n k
mkdir -p ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules
source /home/olivier/a3000_kernel_jb/pack_bootimage.sh
cp ~/android/working/kernel/out/drivers/hid/hid-logitech-dj.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/hid-logitech-dj.ko
cp ~/android/working/kernel/out/drivers/scsi/scsi_wait_scan.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/scsi_wait_scan.ko.ko
cp ~/android/working/kernel/out/drivers/scsi/scsi_tgt.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/scsi_tgt.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/fmradio/mtk_fm_drv.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_fm_drv.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/dual_ccci/ccci.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/ccci.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_uart.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_stp_uart.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_bt.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_stp_bt.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_wmt.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_stp_wmt.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_wmt_wifi.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_wmt_wifi.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_hif_sdio.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_hif_sdio.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/common/mtk_stp_gps.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtk_stp_gps.ko
cp ~/android/working/kernel/out/mediatek/kernel/drivers/combo/drv_wlan/mt6628/wlan/wlan_mt6628.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/wlan_mt6628.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/gpu/pvr/pvrsrvkm.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/pvrsrvkm.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/gpu/pvr/mtklfb.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/mtklfb.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/videocodec/vcodec_kernel_driver.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/vcodec_kernel_driver.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/masp/sec.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/sec.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/dual_ccci/ccci_plat.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/ccci_plat.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/devapc/devapc.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/devapc.ko
cp ~/android/working/kernel/out/mediatek/platform/mt6589/kernel/drivers/devinfo/devinfo.ko ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/devinfo.ko
/home/olivier/a3-kernel/toolchains/arm-eabi-4.6/bin/arm-eabi-strip -g -S -d --strip-unneeded -x -I elf32-littlearm -O elf32-littlearm ~/android/working/out/target/product/$TARGET_PRODUCT/system/lib/modules/*.ko
cp -R /home/olivier/a3-kernel/scripts/$TARGET_PRODUCT/META-INF ~/android/working/out/target/product/$TARGET_PRODUCT/
cd ~/android/working/out/target/product/$TARGET_PRODUCT/
rm -rf *.zip
zip -r $FLASHABLE_ZIP META-INF system boot.img
