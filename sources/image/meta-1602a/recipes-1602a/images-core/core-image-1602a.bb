inherit core-image
CORE_IMAGE_EXTRA_INSTALL += "kernel-module-1602a"
CORE_IMAGE_EXTRA_INSTALL += "utility-1602a"
CORE_IMAGE_EXTRA_INSTALL += "openssh"

inherit extrausers
PASSWD = "\$5\$2WoxjAdaC2\$l4aj6Is.EWkD72Vt.byhM5qRtF9HcCM/5YpbxpmvNB5"
EXTRA_USERS_PARAMS = "usermod -p '${PASSWD}' root;"
