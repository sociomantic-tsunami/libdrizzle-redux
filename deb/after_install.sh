#!/bin/bash
#
# Script which is run after install to ensure binary compatibility between
# this and the previous version of libdrizzle-redux

if [ -h /usr/lib/libdrizzle-redux.so.9 ]; then
    echo "Removing outdated symlink"
    sudo rm /usr/lib/libdrizzle-redux.so.10 /usr/lib/libdrizzle-redux.so.9 2>/dev/null
    if [ -e "/usr/lib/libdrizzle-redux.so.9" ]; then
        echo "Couldn't remove symlink '/usr/lib/libdrizzle-redux.so.9'"
              "Please remove it manually"
    fi;
fi;
