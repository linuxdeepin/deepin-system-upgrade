#!/bin/bash

cleanup() {
	cd "$cwd"
	fusermount -uzq "$cwd"/iso
	rm -rf "$cwd"/squashfs-root
}

term_handler() {
	if [ -n "$child" ]; then
		kill -TERM "$child" 2>/dev/null
	fi
	cleanup
}

trap term_handler SIGTERM

cwd="$(pwd)"
cleanup
fuseiso -p "$1" "$cwd"/iso -o nonempty
state=0

if [ -f "$cwd"/iso/live/filesystem.squashfs ]; then
	if [ -d "$cwd"/squashfs-root ]; then
		rm -rf "$cwd"/squashfs-root
	fi
	unsquashfs "$cwd"/iso/live/filesystem.squashfs -ef etc/os-version &>"$cwd"/unsquashfs.log

	if [ $# -gt 1 ]; then
		if [ $2 = "-c" ]; then
			cd "$cwd"/iso
			sha256sum -c sha256sum.txt &
			child=$!
			wait "$child"
			state=$?
		else
			echo -n $(grep -Po "(?<=$2=).+$" "$cwd"/squashfs-root/etc/os-version)
		fi
	else
		cat "$cwd"/squashfs-root/etc/os-version
	fi
	cd "$cwd"
else
	echo "Not a valid Deepin/UOS iso"
	state=1
fi

findmnt "$cwd"/iso >/dev/null
if [ $? -eq 0 ]; then
	cleanup
fi
exit $state
