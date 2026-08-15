// RUN: %clang -### -target aarch64-linux-gnu -fstruct-layout-reloc -c %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=ENABLE
// RUN: %clang -### -target aarch64-linux-gnu -fstruct-layout-reloc \
// RUN:   -fno-struct-layout-reloc -c %s 2>&1 \
// RUN:   | FileCheck %s --check-prefix=DISABLE

// ENABLE: "-cc1"
// ENABLE: "-fstruct-layout-reloc"

// DISABLE: "-cc1"
// DISABLE-NOT: "-fstruct-layout-reloc"
