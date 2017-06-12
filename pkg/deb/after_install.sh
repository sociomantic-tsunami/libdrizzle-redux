#!/bin/bash
#
# Script which is run after install to ensure binary compatibility between
# this and the previous version of libdrizzle-redux

sudo ln -sf /usr/lib/libdrizzle-redux.so.10 /usr/lib/libdrizzle-redux.so.9
