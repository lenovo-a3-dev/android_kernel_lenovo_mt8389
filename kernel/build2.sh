#!/bin/bash
#Stop script if something is broken
set -e

# Temp fix
#rm -rf ../mediatek
#cp -R mediatek ../

export TARGET_PRODUCT=a3000_row_call
export DT=/home/olivier/cm/device/lenovo/a3/prebuilt

# Toolchain
export PATH=/home/olivier/a3-kernel/toolchains/arm-eabi-4.6/bin:$PATH
export CROSS_COMPILE=arm-eabi-

#Echo actual vars
echo "TARGET_PRODUCT = $TARGET_PRODUCT "
echo "CROSS_COMPILE = $CROSS_COMPILE "

#Create vars for OUT, SCRIPTS and RAMDISK directories
OUT_DIRECTORY=/home/olivier/a3-kernel/out/$TARGET_PRODUCT
RAMDISK_DIRECTORY=/home/olivier/a3-kernel/ramdisk/$TARGET_PRODUCT
SCRIPTS_DIRECTORY=/home/olivier/a3-kernel/scripts/$TARGET_PRODUCT
CERTIFICATES_DIRECTORY=/home/olivier/a3-kernel/.certificates

#Create and clean out directory for your device
mkdir -p $OUT_DIRECTORY
if [ "$(ls -A $OUT_DIRECTORY)" ]; then
rm $OUT_DIRECTORY/* -R
fi

#Kernel part
TARGET_BUILD_VARIANT=user make $1
/home/olivier/android/working/mediatek/build/tools/mkimage arch/arm/boot/zImage KERNEL > $OUT_DIRECTORY/zImage
#cp arch/arm/boot/zImage $OUT_DIRECTORY/

#Modules part
make TARGET_BUILD_VARIANT=user INSTALL_MOD_STRIP=--strip-unneeded INSTALL_MOD_PATH=$OUT_DIRECTORY/system INSTALL_MOD_DIR=$OUT_DIRECTORY/system android_modules_install

# Update device tree
#rm $DT/modules/*
#rm $DT/kernel
#cp $OUT_DIRECTORY/system/lib/modules/* $DT/modules
#cp $OUT_DIRECTORY/zImage $DT/kernel

#Repack part
if [ -d "$RAMDISK_DIRECTORY" ]; then
/home/olivier/a3-kernel/mtk-tools/repack-MTK.pl -boot $OUT_DIRECTORY/zImage $RAMDISK_DIRECTORY $OUT_DIRECTORY/boot.img
rm $OUT_DIRECTORY/zImage

#Flashable zip build
if [ -d "$SCRIPTS_DIRECTORY" ]; then
cp $SCRIPTS_DIRECTORY/* $OUT_DIRECTORY -R
FLASHABLE_ZIP="$OUT_DIRECTORY/kernel_$TARGET_PRODUCT-`date +"%Y%m%d"`-`git rev-parse --short HEAD`"
FLASHABLE_ZIP_2="kernel_$TARGET_PRODUCT-`date +"%Y%m%d"`-`git rev-parse --short HEAD`"
echo "Creating flashable ZIP at $FLASHABLE_ZIP.zip"
pushd $OUT_DIRECTORY
rm -r symbols
zip -r -0 "$FLASHABLE_ZIP_2".zip .
popd
if [ ! -d "$CERTIFICATES_DIRECTORY" ]; then
echo "Warning: $FLASHABLE_ZIP.zip couldn't be signed, you need to run ./certificates.sh"
else
java -jar $SCRIPTS_DIRECTORY/../signapk.jar $CERTIFICATES_DIRECTORY/certificate.pem $CERTIFICATES_DIRECTORY/key.pk8 "$FLASHABLE_ZIP".zip "$FLASHABLE_ZIP"-signed.zip
fi
fi
fi
