#!/usr/bin/env python3

import argparse
import subprocess

from contextlib import contextmanager

USAGE = "Start a qemu virtual machine with the specified disk image."


def parse_args():
    parser = argparse.ArgumentParser(description=USAGE)
    parser.add_argument(
        "--disk-image",
        help="The bootable disk image file to be stated. ",
        required=True,
    )
    return parser.parse_args()


def main():
    params = parse_args()
    subprocess.run(
        [
            "qemu-system-x86_64",
            "-m", "16",
            "-display", "gtk,zoom-to-fit=on",
            "-smp",
            "1,cores=1,cpus=1",  # expose a single CPU to the guest
            "-hda",
            params.disk_image,
        ],
        check=True,
    )


if __name__ == "__main__":
    main()
