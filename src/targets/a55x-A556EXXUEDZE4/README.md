# a55x-A556EXXUEDZE4

Target profile for the Galaxy A55 5G:

```text
model: SM-A556E
device: a55x
firmware: A556EXXUEDZE4
kernel: 6.1.157-android14-11
build: #1 SMP PREEMPT Tue May 12 07:25:39 UTC 2026
page size: 4096
```

All offsets were recovered from the exact UEDZE4 kernel (vmlinux symbols,
BTF struct layouts, and disassembly) and cross-checked against the shipped
artifact `artifacts/a55x-A556EXXUEDZE4/cve-2026-43499-app.so`:

- the five P0-profile image-offset constants (nfulnl logger object, boot_id
  ctl_table data pointer, init task, root task group, sysctl bootid) plus the
  ashmem/root/selinux constants all match the artifact's mov/movk materialization;
- the 32-row P0 fingerprint table is byte-identical to the artifact;
- configfs read/write iter resolve to `configfs_read_iter` (0x46cce8) and
  `configfs_bin_write_iter` (0x46d218 = read_iter + 0x530) as in the artifact.

The physical P0 offset and virtual kernel base are handled separately at
runtime; the virtual base is recovered from the live `ashmem_misc.fops`
pointer and is never treated as a firmware constant.

Generate the 32-row fingerprint table from the raw kernel Image with:

```sh
tools/generate_p0_fingerprint.pl Image 0x1f0000 p0_fingerprint.h
```

Build with Android NDK r29:

```sh
make TARGET=a55x-A556EXXUEDZE4 ANDROID_NDK_HOME=/path/to/android-ndk
```

The release payload is capped at the artifact's `APP_RELEASE_SIZE` (104128
bytes); rebuilds are semantically (not byte) identical because the artifact
was produced with clang 22.0.1 / LLD 21.0.0.
