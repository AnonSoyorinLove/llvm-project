// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=IR
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=PLAIN
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -S -o - %s | FileCheck %s --check-prefix=DATA

struct sample {
  char pad;
  int value;
  long next;
};

typedef __SIZE_TYPE__ size_t;

unsigned long field_offset(void) {
  return __builtin_offsetof(struct sample, value);
}

unsigned long type_size(void) {
  return sizeof(struct sample);
}

size_t type_size_t(void) {
  return sizeof(struct sample);
}

unsigned long global_offset =
    __builtin_offsetof(struct sample, value);
unsigned long global_size = sizeof(struct sample);
const unsigned long global_const_size = sizeof(struct sample);
unsigned global_offset32 = __builtin_offsetof(struct sample, value);
unsigned global_size32 = sizeof(struct sample);

// IR-LABEL: define dso_local i64 @field_offset()
// IR: call i64 asm sideeffect
// IR-SAME: movz ${0:x}, #4
// IR: ret i64 %struct.layout.reloc.value
// IR-LABEL: define dso_local i64 @type_size()
// IR: call i64 asm sideeffect
// IR-SAME: movz ${0:x}, #16
// IR: ret i64 %struct.layout.reloc.value
// IR: !llvm.struct.layout.reloc = !{![[FIELD:[0-9]+]], ![[SIZE:[0-9]+]], ![[SIZE]], ![[FIELD]], ![[SIZE]], ![[SIZE]], ![[FIELD]], ![[SIZE]]}
// IR: ![[FIELD]] = !{!"sample", !"value", i32 16, i32 4, i32 4}
// IR: ![[SIZE]] = !{!"sample", !"<type-size>", i32 16, i32 0, i32 0}

// PLAIN-LABEL: define dso_local i64 @field_offset()
// PLAIN: ret i64 4
// PLAIN-LABEL: define dso_local i64 @type_size()
// PLAIN: ret i64 16
// PLAIN-NOT: asm sideeffect

// DATA: .section .llvm_struct_reloc.str,"MS",@progbits,1
// DATA: .ascii "sample"
// DATA: .ascii "value"
// DATA: .section .llvm_struct_reloc,"",@progbits
// DATA: .hword 3
// DATA: .hword 3
// DATA: .xword global_offset
// DATA: .hword 4
// DATA: .hword 3
// DATA: .xword global_size
// DATA: .xword global_const_size
// DATA: .hword 3
// DATA: .hword 2
// DATA: .xword global_offset32
// DATA: .hword 4
// DATA: .hword 2
// DATA: .xword global_size32
