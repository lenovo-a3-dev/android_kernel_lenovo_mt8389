#!/bin/bash

set -e

KERNEL_DIR=/home/olivier/android/working
export TARGET_PRODUCT=a3000_row_call
RAMDISK_DIRECTORY=/home/olivier/a3-kernel/ramdisk/$TARGET_PRODUCT

rm -rf $KERNEL_DIR/out/target/product/$TARGET_PRODUCT/kernel
rm -rf $KERNEL_DIR/out/target/product/$TARGET_PRODUCT/boot.img
$KERNEL_DIR/mediatek/build/tools/mkimage $KERNEL_DIR/kernel/out/arch/arm/boot/zImage KERNEL > $KERNEL_DIR/out/target/product/$TARGET_PRODUCT/kernel

/home/olivier/a3-kernel/mtk-tools/repack-MTK.pl -boot $KERNEL_DIR/out/target/product/$TARGET_PRODUCT/kernel $RAMDISK_DIRECTORY $KERNEL_DIR/out/target/product/$TARGET_PRODUCT/boot.img
