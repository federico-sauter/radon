#!/bin/bash
pids=$( ps -ef | grep qemu-system-x86_64 | grep -v grep | awk '{ print $2 }' | xargs )
if [ -n "$pids" ]; then
	kill $pids
fi
