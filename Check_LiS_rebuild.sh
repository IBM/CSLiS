#!/bin/bash
#
# Copyright 2022 - IBM Inc. All rights reserved
# SPDX-License-Identifier: LGPL-2.1
#
# Test of streams driver exists
#
ls -l /lib/modules/`uname -r`/misc | grep streams.ko >/dev/null 2>&1
if [ $? = 0 ]
then
  echo " LiS is up to date"
else
  echo " LiS needs to be built"
  cd /usr/src/LiS
  make uninstall
  make very-clean
  ./buildLiS
fi
