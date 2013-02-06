PATH=/media/android/build/aosp/out/host/linux-x86/bin/:$PATH
TESTOUTDIR=tests
TESTIMAGES=testimages
TEST1="01_simple_unpack"
TEST2="02_simple_repack"
CM_GROUPER=grouper-cm10.1.img
TOOL=bootimg-tools
ZIMAGE=zImage
RAMDISK=ramdisk

echo "Cleaning Previous Test Results"
rm -rfv $TESTOUTDIR
mkdir -p $TESTOUTDIR

echo "TEST 1 unpack simple ramdisk.cpio.gz and kernel"
mkdir -v $TESTOUTDIR/$TEST1
echo "$TOOL unpack -i $TESTIMAGES/$CM_GROUPER -o $TESTOUTDIR/$TEST1 -k $ZIMAGE -r $RAMDISK -x"
$TOOL unpack -i $TESTIMAGES/$CM_GROUPER -o $TESTOUTDIR/$TEST1 -k $ZIMAGE -r $RAMDISK -x

echo "$TOOL pack -o $TESTOUTDIR/$TEST2/$CM_GROUPER -k $TESTOUTDIR/$TEST1/$ZIMAGE -r $TESTOUTDIR/$TEST1/$RAMDISK"
$TOOL pack -o $TESTOUTDIR/$TEST2/$CM_GROUPER -k $TESTOUTDIR/$TEST1/$ZIMAGE -r $TESTOUTDIR/$TEST1/$RAMDISK

