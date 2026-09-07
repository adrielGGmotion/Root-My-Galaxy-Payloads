# a55x-A556EXXUEDZE4

Target profile for Samsung Galaxy A55 5G `SM-A556E` on the
ZTO firmware `A556EXXUEDZE4`.

```text
build: BP4A.251205.006.A556EXXUEDZE4
fingerprint: samsung/a55xnsxx/essi:16/BP4A.251205.006/A556EXXUEDZE4:user/release-keys
kernel: 6.1.157-android14-11
page size: 4096
image base: 0xffffffc008000000
```

`target.h` and `p0_fingerprint.h` were generated from the exact raw Image;
live ADB properties matched this profile on 2026-09-07.
The profile uses the shared Android 14 / 6.1 physical-P0 route, compact
`rt_mutex_waiter` layout, MTE-aware KernelSnitch matching, and a fresh
same-process P0 session. The legacy inverse-slide fingerprint mode is not
enabled.

The payload and KernelSU pair are statically built and audited. Bounded live
runs reached the collision/bruteforce stage but stopped at the initial pipe
page gate, so no root or module-load result was recorded. Do not treat this
profile as hardware validated until a clean-boot run passes the remaining
gates.
