#!/bin/bash
#Stop script if something is broken
set -e

export TARGET_PRODUCT=a3000_row_call
export DT=/home/olivier/lenovo_a3

# Toolchain
export PATH=/home/olivier/a3-kernel/toolchains/arm-eabi-4.6/bin:$PATH
export CROSS_COMPILE=arm-eabi-

#Echo actual vars
echo "TARGET_PRODUCT = $TARGET_PRODUCT "
echo "CROSS_COMPILE = $CROSS_COMPILE "

#Create vas for OUT
OUT_DIRECTORY=/home/olivier/a3-kernel/out/$TARGET_PRODUCT

#Create and clean out directory for your device
mkdir -p $OUT_DIRECTORY
if [ "$(ls -A $OUT_DIRECTORY)" ]; then
rm $OUT_DIRECTORY/* -R
fi

#Kernel part
TARGET_BUILD_VARIANT=user make $1
#/home/olivier/android/working/mediatek/build/tools/mkimage arch/arm/boot/zImage KERNEL > $OUT_DIRECTORY/zImage
cp arch/arm/boot/zImage $OUT_DIRECTORY/

#Modules part
make TARGET_BUILD_VARIANT=user INSTALL_MOD_STRIP=--strip-unneeded INSTALL_MOD_PATH=$OUT_DIRECTORY/system INSTALL_MOD_DIR=$OUT_DIRECTORY/system android_modules_install

# Update device tree
cd $DT
git add --all
git stash;git reset --hard
rm -rf $DT/prebuilt/modules/*
rm -rf $DT/kernel
cp $OUT_DIRECTORY/system/lib/modules/* $DT/prebuilt/modules
cp $OUT_DIRECTORY/zImage $DT/kernel
git add --all;git commit -m "a3: Update prebuilt kernel.";git push -f
