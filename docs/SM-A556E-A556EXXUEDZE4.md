# SM-A556E — A556EXXUEDZE4 porting record

Galaxy A55 (Exynos 1480), Brazilian (ZTO) unit, firmware
`A556EXXUEDZE4` / `BP4A.251205.006.A556EXXUEDZE4`. Kernel
`6.1.157-android14-11`, the same KMI as the S24 FE `essi-S721NKSSCDZF3`
reference. Every offset below was derived from this firmware's own kernel
Image; nothing was copied from another device without re-derivation.

## Firmware identity

```text
model:         SM-A556E (device a55x, product a55xnsxx)
AP/PDA:        A556EXXUEDZE4
CP:            A556EXXUEDZE4_CP34304132
display build: BP4A.251205.006.A556EXXUEDZE4
fingerprint:   samsung/a55xnsxx/a55x:16/BP4A.251205.006/A556EXXUEDZE4:user/release-keys
Android:       16, One UI 8.0 (SDK 36), patch 2026-05-05
kernel:        Linux version 6.1.157-android14-11
               (clang 17.0.2 r487747c) #1 SMP PREEMPT Tue May 12 07:25:39 UTC 2026
identity note: no meta-data/fota.zip in this fac ZIP; identity props were read
               from the live device over ADB.
```

## Images

```text
boot.img: 67108864 bytes, boot header v4, kernel at 0x1000
          SHA-256 6ECAFFCBC9BEA05B7FB00BE9521DF503D988DA74E3FA32EB0EAB898FF8B9D9D2
kernel:   38697472 bytes
          SHA-256 0494F519C7B1ABCAE169D2AD598FA5C0B00C36C9D3560D7D9E15F29677593F91
ARM64 Image header: text_offset=0x0 image_size=0x27a0000 flags=0xa
raw BTF:  [0x1876a88, 0x1e2a8c4), single validated candidate
ELF base: 0xffffffc008000000
sboot.bin: 5260080 bytes; "Starting kernel...\n" at 0x18278e
```

## Symbol offsets (base 0xffffffc008000000)

| Macro/use | Symbol or derivation | Offset |
| --- | --- | ---: |
| `CALL_USERMODEHELPER_EXEC_WORK_OFF` | `call_usermodehelper_exec_work` | `0x000d4360` |
| `NOOP_LLSEEK_OFF` | `noop_llseek` | `0x0039ec9c` |
| `COPY_SPLICE_READ_OFF` | `generic_file_splice_read` | `0x003ec8a0` |
| `CONFIGFS_READ_ITER_OFF` | `configfs_read_iter` | `0x0046cce8` |
| `CONFIGFS_BIN_WRITE_ITER_OFF` | `configfs_bin_write_iter` | `0x0046d218` |
| `ASHMEM_IOCTL_OFF` | `ashmem_ioctl` | `0x00d2d4f8` |
| `ASHMEM_COMPAT_IOCTL_OFF` | `compat_ashmem_ioctl` | `0x00d2de30` |
| `ASHMEM_MMAP_OFF` | `ashmem_mmap` | `0x00d2de88` |
| `ASHMEM_OPEN_OFF` | `ashmem_open` | `0x00d2e0a8` |
| `ASHMEM_RELEASE_OFF` | `ashmem_release` | `0x00d2e130` |
| `ASHMEM_SHOW_FDINFO_OFF` | `ashmem_show_fdinfo` | `0x00d2e250` |
| `ANON_PIPE_BUF_OPS_OFF` | `anon_pipe_buf_ops` | `0x0120c610` |
| `ASHMEM_FOPS_OFF` | `ashmem_fops` | `0x013c9b50` |
| `KMALLOC_CACHES_OFF` | `kmalloc_caches` | `0x01792ed8` |
| `SLIDE_NFULNL_LOGGER_NAME_OFF` | `"nfnetlink_log"` string referenced by `nfulnl_logger.name` | `0x016ca0d5` |
| `SYSTEM_UNBOUND_WQ_OFF` | `system_unbound_wq` | `0x022cae60` |
| `SLIDE_NFULNL_LOGGER_OBJECT_OFF` | `nfulnl_logger` object | `0x022d29e0` |
| `__start_ftrace_events` | event array start | `0x0228a940` |
| `__stop_ftrace_events` | event array end | `0x0228cc80` |
| `INIT_TASK_OFF` | `init_task` | `0x022df700` |
| `ROOT_TASK_GROUP_OFF` | `root_task_group` | `0x024f4d40` |
| `SLIDE_RANDOM_TABLE_BOOT_ID_DATA_PTR_OFF` | `.data` slot of `random_table[4]` (`boot_id`) | `0x0241e878` |
| `SELINUX_ENFORCING_OFF` | `selinux_state.enforcing` | `0x025c92f8` |
| `ASHMEM_MISC_FOPS_OFF` | unnamed `miscdevice{name="ashmem"}` + 0x10 | `0x02464430` |
| `SLIDE_SYSCTL_BOOTID_OFF` | `sysctl_bootid` storage | `0x026b0518` |

Device-specific derivations:

- `ashmem_misc` has no symbol on this build. The miscdevice object was located
  by scanning for a qword equal to `&ashmem_fops`: candidate
  `0xffffffc00a464420` has `minor == MISC_DYNAMIC_MINOR` and name `"ashmem"`;
  BTF gives `offsetof(struct miscdevice, fops) == 0x10`.
- A memfd-based ashmem shim exists (`memfd_ashmem_shim_ioctl`), but the full
  native ashmem function set and `ashmem_fops` remain present, so the payload's
  ashmem route is unaffected.
- `selinux_enforcing` does not exist; `SELINUX_ENFORCING_OFF` points at
  `selinux_state.enforcing` (field offset 0 per BTF).
- `random_table[]` stride is `sizeof(struct ctl_table) == 64`; entry 4 is
  `boot_id` with `data == &sysctl_bootid`; its data slot is entry + 8.
- `nfulnl_logger.name` reads `0xffffffc0096ca0d5`.

## Layout values (target BTF)

Identical to `essi-S721NKSSCDZF3`:

```text
sizeof(struct file_operations) = 0x110
  unlocked_ioctl=0x50 compat_ioctl=0x58 mmap=0x60 open=0x70
  release=0x80 splice_read=0xc8 show_fdinfo=0xe0

task_struct size=0x12c0
  usage=0x40 prio=0x84 normal_prio=0x8c sched_task_group=0x348
  pi_lock=0x924 pi_waiters=0x938 pi_top_task=0x948 pi_blocked_on=0x950

sizeof(struct page)=0x40 compound_head=0x08 slab_cache=0x18 page_type=0x30

sizeof(struct rt_mutex_waiter)=0x58
  tree_entry=0x00 pi_tree_entry=0x18 task=0x30 lock=0x38
  wake_state=0x40 prio=0x44 deadline=0x48 ww_ctx=0x50
```

The compact 0x58 waiter layout means this target uses
`COMPACT_RT_MUTEX_WAITER=1` with
`FAKE_WAITER_PI_TREE_ENTRY_OFF=0x18 TASK=0x30 LOCK=0x38 WAKE_STATE=0x40
PRIO=0x44 DEADLINE=0x48 WW_CTX=0x50 LAYOUT_SIZE=0x58`, mirroring the dm3q
compact profile.

## Physical load proof

In `sboot.bin` the handoff sequence sits at file offset `0x3073c`, directly
after printing `"Starting kernel...\n"`:

```text
adrp x8, #0x482000
ldr  w8, [x8, #0xcf8]      ; Image text_offset (== 0)
mov  w9, #-0x80000000
add  x8, x8, x9
blr  x8
```

```c
#define P0_PHYS_OFFSET 0x80000000ULL
#define P0_KERNEL_PHYS_LOAD 0x80000000ULL
```

## Slide data

```text
SLIDE_TRACEFS_EVENT_ID = 106
  live: /sys/kernel/tracing/events/sched/sched_blocked_reason/id -> 106
  offline: zero-based index (__event_sched_blocked_reason -
           __start_ftrace_events)/8 = 86; __TRACE_LAST_TYPE = 20;
           20 + 86 = 106.

SLIDE_TRACEFS_WORKER_CALLER_OFF = 0x000dbc94
  bl schedule inside worker_thread at 0xffffffc0080dbc90;
  caller = following instruction 0xffffffc0080dbc94.

P0 fingerprint: tools/generate_p0_fingerprint.pl kernel 0x1f0000
  targets/a55x-A556EXXUEDZE4/p0_fingerprint.h — 32 rows verified (256 qwords).
```

## Open items recorded before hardware validation

Static frame-depth analysis for `SLIDE_PSELECT_WORD_SHIFT` left an unexplained
`-0x48` gap between the stale waiter address (`SP0 - 0x248`,
`futex_wait_requeue_pi` frame, `rt_waiter` at `sp+0x98`) and
`core_sys_select`'s `stack_fds` (`SP0 - 0x200`). The initial value 0 follows
the same-KMI sibling and must be confirmed empirically via `SLIDE_ONLY` and
the fops diagnostic gates before production use.

## KernelSU module

Built from KernelSU `v3.2.5` (`b0bc817b4e966aa6aa830834eaf6ef765d821d40`)
with `patches/KernelSU-v3.2.5-samsung-kdp-rkp-defex.patch`, DDK image
`ghcr.io/ylarod/ddk-min:android14-6.1-20260313`, release string overridden to
`6.1.157-android14-11`, and:

```text
CONFIG_KSU=m CONFIG_KSU_SAMSUNG_KDP=y CONFIG_KSU_SAMSUNG_RKP=y
CONFIG_KSU_SAMSUNG_DEFEX=y CONFIG_KSU_SAMSUNG_NO_PATCH_TEXT=y
```

The no-patch-text configuration follows the E1S/A56 Exynos EL2 findings:
the generic 6.1 module panics while live-patching text under Samsung/Exynos
EL2.

```text
vermagic: 6.1.157-android14-11 SMP preempt mod_unload modversions aarch64

check_symbol vs recovered fw/vmlinux.elf: exit 0, no missing symbols
audit_module_against_target.py --manual-relocation:
  undefined symbols: 202
  module version entries: 0
  missing from target symbol table: 0
  symbols resolved from kallsyms rather than target exports: 47
  undefined symbols intentionally without module CRC: 202
  target CRC mismatches: 0
```

Published pair:

```text
android14-6.1_kernelsu-a55x-A556EXXUEDZE4-kdp.ko
size: 398368

ksud-a55x-A556EXXUEDZE4-kdp
size: 3608304
```

## Support feed

```json
{
  "payloadId": "a55x-A556EXXUEDZE4",
  "displayName": "Galaxy A55 | Kernel 6.1.157",
  "models": ["SM-A556E"],
  "kernelVersions": ["6.1.157"],
  "requiresFreshP0Session": true
}
```

Only the model verified against live hardware (`SM-A556E`) is listed; other
regional A55 variants should be added only after their builds are checked
against these offsets.

Hardware execution remains a separate validation step. This profile has not
been executed on an SM-A556E device yet.
