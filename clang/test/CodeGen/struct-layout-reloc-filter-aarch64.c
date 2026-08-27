// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -I %S/Inputs/struct-layout-reloc-filter \
// RUN:   -fstruct-layout-reloc-file-prefix=%S/Inputs/struct-layout-reloc-filter/one \
// RUN:   -fstruct-layout-reloc-file-prefix=%S/Inputs/struct-layout-reloc-filter/two \
// RUN:   -fstruct-layout-reloc-global-extra-bytes=32 \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=PREFIX
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -I %S/Inputs/struct-layout-reloc-filter \
// RUN:   -fstruct-layout-reloc-config=%S/Inputs/struct-layout-reloc-filter/filter.json \
// RUN:   -fstruct-layout-reloc-global-extra-bytes=32 \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=CONFIG
// RUN: not %clang_cc1 -triple aarch64-unknown-linux-gnu \
// RUN:   -fstruct-layout-reloc \
// RUN:   -fstruct-layout-reloc-config=%S/Inputs/struct-layout-reloc-filter/invalid.json \
// RUN:   -fsyntax-only %s 2>&1 | FileCheck %s --check-prefix=INVALID

#include "one/records.h"
#include "two/records.h"
#include "one-other/records.h"

struct prefix_one global_one = {0, 1};
struct prefix_other global_other = {0, 2};

int load_one(struct prefix_one *p) {
  return p->value;
}

int load_two(struct prefix_two *p) {
  return p->value;
}

int load_other(struct prefix_other *p) {
  return p->value;
}

unsigned long size_one(void) {
  return sizeof(struct prefix_one);
}

unsigned long size_other(void) {
  return sizeof(struct prefix_other);
}

void stack_one(void) {
  struct prefix_one value;
}

void stack_other(void) {
  struct prefix_other value;
}

// PREFIX: %prefix_one.reloc = type { %struct.prefix_one, [32 x i8] }
// PREFIX: @global_one = externally_initialized global %prefix_one.reloc
// PREFIX: @global_other = global %struct.prefix_other

// PREFIX-LABEL: define{{.*}} i32 @load_one(
// PREFIX: call i64 asm sideeffect
// PREFIX-LABEL: define{{.*}} i32 @load_two(
// PREFIX: call i64 asm sideeffect
// PREFIX-LABEL: define{{.*}} i32 @load_other(
// PREFIX-NOT: call i64 asm sideeffect
// PREFIX: getelementptr inbounds %struct.prefix_other
// PREFIX-LABEL: define{{.*}} i64 @size_one()
// PREFIX: call i64 asm sideeffect
// PREFIX-LABEL: define{{.*}} i64 @size_other()
// PREFIX-NOT: call i64 asm sideeffect
// PREFIX: ret i64 8
// PREFIX-LABEL: define{{.*}} void @stack_one()
// PREFIX: call i64 asm sideeffect
// PREFIX: alloca i8, i64 %struct.layout.reloc.value
// PREFIX-LABEL: define{{.*}} void @stack_other()
// PREFIX-NOT: call i64 asm sideeffect
// PREFIX: alloca %struct.prefix_other

// CONFIG: %prefix_one.reloc = type { %struct.prefix_one, [32 x i8] }
// CONFIG: @global_one = externally_initialized global %prefix_one.reloc
// CONFIG: @global_other = global %struct.prefix_other

// CONFIG-LABEL: define{{.*}} i32 @load_one(
// CONFIG: call i64 asm sideeffect
// CONFIG-LABEL: define{{.*}} i32 @load_two(
// CONFIG-NOT: call i64 asm sideeffect
// CONFIG: getelementptr inbounds %struct.prefix_two
// CONFIG-LABEL: define{{.*}} i32 @load_other(
// CONFIG-NOT: call i64 asm sideeffect
// CONFIG: getelementptr inbounds %struct.prefix_other

// The matching definition is in one/records.h. The including source file is
// outside the selected prefix, so this also verifies definition-file lookup.
// one-other starts with the spelling of one but is outside that directory,
// which verifies that prefix matching observes path-component boundaries.
// PREFIX: !llvm.struct.layout.reloc = !{
// PREFIX-DAG: !{!"prefix_one", !"value", i32 8, i32 4, i32 4}
// PREFIX-DAG: !{!"prefix_two", !"value", i32 8, i32 4, i32 4}
// PREFIX-NOT: !{!"prefix_other"

// CONFIG: !llvm.struct.layout.reloc = !{
// CONFIG-DAG: !{!"prefix_one", !"value", i32 8, i32 4, i32 4}
// CONFIG-NOT: !{!"prefix_two"
// CONFIG-NOT: !{!"prefix_other"

// INVALID: invalid struct layout relocation config file
// INVALID-SAME: 'whitelist' must be an array of strings
