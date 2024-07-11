#!/usr/bin/env python3

import argparse
import os
import shutil
import subprocess
import tempfile

from contextlib import contextmanager

USAGE = "Create a bootable disk image with an installed kernel, using GRUB as the bootloader. Note that only users in the sudoers file can run this program!"


def parse_args():
    parser = argparse.ArgumentParser(description=USAGE)
    parser.add_argument("--kernel-image", help="The kernel image file", required=True)
    parser.add_argument(
        "--disk-image",
        help="The bootable disk image file to be created. "
        "Warning: this will overwrite an existing file!",
        required=True,
    )
    return parser.parse_args()


def create_disk_image(disk_image):
    print("Creating disk image...")
    if os.path.exists(disk_image):
        os.remove(disk_image)
    # The layout for the resulting image is:
    # 1MB - GRUB bootloader
    # 7MB - / (contains GRUB and the FENIX kernel)
    image_size = "8M"
    subprocess.run(
        [
            "dd",
            "if=/dev/zero",
            "of={}".format(disk_image),
            "bs={}".format(image_size),
            "count=1",
            "status=noxfer",
        ],
        check=True,
    )


def get_loop_device():
    return subprocess.run(
        ["sudo", "losetup", "--find"], check=True, stdout=subprocess.PIPE, encoding="utf-8"
    ).stdout.strip()


def init_disk_image(disk_image_file):
    create_disk_image(disk_image_file)
    print("Creating root partition...")
    subprocess.run(
        ["sudo", "fdisk", disk_image_file],
        input="n\np\n1\n\n\na\nw\n",
        encoding="ascii",
    )


@contextmanager
def attach_loop_devices(disk_image_file):
    offset = 1024 * 1024  # 1MB

    bootloader_dev = get_loop_device()
    subprocess.run(["sudo", "losetup", bootloader_dev, disk_image_file], check=True)

    root_dev = get_loop_device()
    subprocess.run(
        ["sudo", "losetup", root_dev, disk_image_file, "-o", str(offset)], check=True
    )

    try:
        print(
            "GRUB loop device {} attached to {}".format(bootloader_dev, disk_image_file)
        )
        print(
            "root loop device {} attached to {} with offset {}".format(
                root_dev, disk_image_file, offset
            )
        )
        yield {"bootloader": bootloader_dev, "root": root_dev}
    finally:
        print("Dettaching GRUB loop device {}".format(bootloader_dev))
        subprocess.run(["sudo", "losetup", "-d", bootloader_dev], check=False)
        print("Dettaching root loop device {}".format(root_dev))
        subprocess.run(["sudo", "losetup", "-d", root_dev], check=False)


def format_disk(device):
    print("Formatting disk {}...".format(device))
    subprocess.run(["sudo", "mkfs", "-t", "ext2", device], check=True)


def mount_loop_dev(device, mount_point):
    print("Mounting {} on {}...".format(device, mount_point))
    uid = os.getuid()
    gid = os.getegid()
    subprocess.run(["sudo", "mount", "-o", "loop", device, mount_point], check=True)


def unmount_disk(mount_point):
    print("Unmounting {}...".format(mount_point))
    subprocess.run(["sudo", "umount", mount_point], check=False)


def install_grub(device, root_dir):
    print("Installing GRUB...")
    subprocess.run(
        [
            "sudo",
            "grub-install",
            "--root-directory={}".format(root_dir),
            "--target=i386-pc",
            "--modules=normal ext2 part_msdos",
            device,
        ],
        check=True,
    )


def sudo_cp(src, dst):
    subprocess.run(["sudo", "cp", src, dst])


def configure_grub(boot_mount_point, kernel_image_name):
    print("Configuring GRUB...")
    grub_dir = os.path.join(boot_mount_point, "grub")
    grub_config_path = os.path.join(grub_dir, "grub.cfg")
    kernel_image_path = os.path.join("/boot", kernel_image_name)
    with tempfile.TemporaryDirectory() as tmp:
        tmp_grub_config = os.path.join(tmp, "grub.cfg")
        with open(tmp_grub_config, "w") as f:
            f.write('menuentry "FENIX" {\n')
            f.write("    multiboot {}\n".format(kernel_image_path))
            f.write(
                """}
timeout=0"""
            )
        sudo_cp(tmp_grub_config, grub_config_path)


def main():
    params = parse_args()
    subprocess.run(["sudo", "true"], check=True)
    with tempfile.TemporaryDirectory() as mount_point:
        init_disk_image(params.disk_image)
        with attach_loop_devices(params.disk_image) as devices:
            format_disk(devices["root"])
            mount_loop_dev(devices["root"], mount_point)
            try:
                kernel_image_name = os.path.basename(params.kernel_image)
                install_grub(devices["bootloader"], mount_point)

                print("Copying kernel to the disk...")
                boot_mount_point = os.path.join(mount_point, "boot")
                sudo_cp(params.kernel_image, boot_mount_point)

                configure_grub(boot_mount_point, kernel_image_name)
            finally:
                unmount_disk(mount_point)
    print("DONE")


if __name__ == "__main__":
    main()
