#!/bin/sh
while [ -z "$(resetprop sys.boot_completed)" ]; do
	sleep 5
done
/data/adb/modules/tda/tda
