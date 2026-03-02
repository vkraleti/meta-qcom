#!/bin/sh
# Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
#
# SPDX-License-Identifier: BSD-3-Clause-Clear
set -e

DEV=/dev/disk/by-partlabel/persist
[ -b "$DEV" ] || exit 0

# Check if a filesystem exists
if [ -z "$(/sbin/blkid -o value -s TYPE "$DEV" 2>/dev/null || true)" ]; then
    echo "$DEV: no filesystem found, creating ext4"
    /sbin/mkfs.ext4 -F "$DEV"
    exit 0
fi

# Run a read-only fsck check to detect corruptions
/sbin/e2fsck -n "$DEV" >/dev/null 2>&1
err=$?
if [ "$err" -gt 4 ]; then
    echo "$DEV: filesystem corrupted (e2fsck error=$err), recreating"
    /sbin/mkfs.ext4 -F "$DEV"
    exit 0
else
    echo "$DEV: filesystem is OK"
fi
