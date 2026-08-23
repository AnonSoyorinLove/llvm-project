// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=IR
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=PLAIN
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -O2 -S -o - %s | FileCheck %s --check-prefix=ASM

struct sample {
  char pad;
  int value;
  long next;
};

int load_value(struct sample *p) {
  return p->value;
}

void store_value(struct sample *p, int value) {
  p->value = value;
}

long *address_next(struct sample *p) {
  return &p->next;
}

// IR: call i64 asm sideeffect
// IR-SAME: movz ${0:x}, #4
// IR: getelementptr i8, ptr {{.*}}, i64 %struct.field.offset
// IR: load i32, ptr {{.*}}, align 1
// IR: store i32 {{.*}}, ptr {{.*}}, align 1
// IR: !llvm.struct.layout.reloc = !{![[VALUE:[0-9]+]], ![[VALUE]], ![[ADDR:[0-9]+]]}
// IR: ![[VALUE]] = !{!"sample", !"value", i32 16, i32 4, i32 4}
// IR: ![[ADDR]] = !{!"sample", !"next", i32 16, i32 8, i32 8}

// PLAIN-NOT: asm sideeffect
// PLAIN: getelementptr inbounds %struct.sample, ptr {{.*}}, i32 0, i32 1

// ASM: mov x[[OFFSET:[0-9]+]], #4
// ASM-NEXT: movk x[[OFFSET]], #0, lsl #16
// ASM: .section .llvm_struct_reloc.str,"MS",@progbits,1
// ASM: .ascii "sample"
// ASM: .byte 0
// ASM: .ascii "value"
// ASM: .section .llvm_struct_reloc,"",@progbits
// ASM: .word 1397902385
// ASM: .hword 1
// ASM: .hword 1
// ASM: .hword 1
// ASM: .hword 1
// ASM: .word 64
// ASM: .word 16
// ASM: .word 4
// ASM: .word 4
