#!/bin/sh

set -e

load_lcd1602a() {
    module="lcd1602a"
    device="lcd1602a"
    mode="664"

    if grep -q '^staff:' /etc/group; then
        group="staff"
    else
        group="wheel"
    fi

    if [ -f $module.ko ]; then
        insmod $module.ko || exit 1
    else
        insmod /lib/modules/$(uname -r)/extra/$module.ko || exit 1
    fi

    major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)

    rm -f /dev/${device}
    mknod /dev/${device} c $major 0
    chgrp $group /dev/${device}
    chmod $mode  /dev/${device}
}

unloadload_lcd1602a() {
    module="lcd1602a"
    device="lcd1602a"

    rmmod $module || exit 1

    rm -f /dev/${device} /dev/${device}
}

case "$1" in
  start)
     echo -n "Loading lcd1602a module..."
     load_lcd1602a
     echo "."
     ;;
  stop)
     echo -n "Unloading lcd1602a module..."
     unloadload_lcd1602a
     echo "."
     ;;
  force-reload|restart)
     echo -n "Reloading lcd1602a module..."
     unloadload_lcd1602a
     load_lcd1602a
     echo "."
     ;;
  *)
     echo "Usage: $0 {start|stop|restart|force-reload}"
     exit 1
esac

exit 0