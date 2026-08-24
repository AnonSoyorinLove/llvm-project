// RUN: %clang -### -target aarch64-linux-gnu -fstruct-layout-reloc -fstruct-layout-reloc-file-prefix=/kernel/include -fstruct-layout-reloc-file-prefix=/vendor/include -fstruct-layout-reloc-config=struct-reloc.json -c %s 2>&1 | FileCheck %s
// RUN: not %clang -target aarch64-linux-gnu \
// RUN:   -fstruct-layout-reloc-file-prefix= -fsyntax-only %s 2>&1 | \
// RUN:   FileCheck %s --check-prefix=EMPTY-PREFIX
// RUN: %clang -target aarch64-linux-gnu -fstruct-layout-reloc \
// RUN:   -fstruct-layout-reloc-config=%S/../CodeGen/Inputs/struct-layout-reloc-filter/filter.json \
// RUN:   -MD -MF %t.d -c %s -o %t.o
// RUN: FileCheck %s --check-prefix=DEPFILE --input-file=%t.d

// CHECK: "-cc1"
// CHECK-SAME: "-fstruct-layout-reloc"
// CHECK-SAME: "-fstruct-layout-reloc-file-prefix=/kernel/include"
// CHECK-SAME: "-fstruct-layout-reloc-file-prefix=/vendor/include"
// CHECK-SAME: "-fstruct-layout-reloc-config=struct-reloc.json"

// EMPTY-PREFIX: error: struct layout relocation file prefix cannot be empty

// DEPFILE: filter.json
