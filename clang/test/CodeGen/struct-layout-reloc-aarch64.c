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

extern struct sample global_sample;

void inline_asm_immediate(void) {
  __asm__ volatile(".quad %c0" : : "i"(&global_sample.next));
}

static __attribute__((always_inline)) inline void
inline_asm_immediate_helper(long *pointer) {
  __asm__ volatile(".quad %c0" : : "i"(pointer));
}

void inline_asm_immediate_call(void) {
  inline_asm_immediate_helper(&global_sample.next);
}

long load_global_next(void) {
  return global_sample.next;
}

// IR: call i64 asm sideeffect
// IR-SAME: movz ${0:x}, #4
// IR: getelementptr i8, ptr {{.*}}, i64 %struct.field.offset
// IR: load i32, ptr {{.*}}, align 1
// IR: store i32 {{.*}}, ptr {{.*}}, align 1
// IR-LABEL: define dso_local void @inline_asm_immediate()
// IR: call void asm sideeffect ".quad ${0:c}", "i"(ptr getelementptr inbounds (%struct.sample, ptr @global_sample, i32 0, i32 2))
// IR-LABEL: define dso_local void @inline_asm_immediate_call()
// IR: store ptr getelementptr inbounds (%struct.sample, ptr @global_sample, i32 0, i32 2)
// IR: call void asm sideeffect ".quad ${0:c}", "i"(ptr %{{.*}})
// IR-LABEL: define dso_local i64 @load_global_next()
// IR: call i64 asm sideeffect
// IR: !llvm.struct.layout.reloc = !{![[VALUE:[0-9]+]], ![[VALUE]], ![[ADDR:[0-9]+]], ![[ADDR]]}
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
// ASM: .ascii "int"
// ASM: .section .llvm_struct_reloc,"",@progbits
// ASM: .word 1397902385
// ASM: .hword 2
// ASM: .hword 1
// ASM: .hword 1
// ASM: .hword 1
// ASM: .word 72
// ASM: .word 16
// ASM: .word 4
// ASM: .word 4
// ASM: .word 1
// ASM: .word 2
// ASM: .hword 1
