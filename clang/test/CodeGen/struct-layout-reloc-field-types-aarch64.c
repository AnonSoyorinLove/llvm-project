// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=IR
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -S -o - %s | FileCheck %s --check-prefix=ASM
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -fsanitize=local-bounds -O1 -emit-llvm -o - %s | \
// RUN:   FileCheck %s --check-prefix=BOUNDS

struct nested {
  int value;
};

struct sample {
  char pad;
  struct nested child;
  int values[5];
};

unsigned long array_field_size(struct sample *p) {
  return sizeof(p->values);
}

int nested_field_load(struct sample *p) {
  return p->child.value;
}

int array_field_load(struct sample *p, unsigned long index) {
  return p->values[index];
}

unsigned long global_array_field_size =
    sizeof(((struct sample *)0)->values);

// IR-LABEL: define dso_local i64 @array_field_size
// IR: call i64 asm sideeffect
// IR-SAME: movz ${0:x}, #20
// IR: ret i64 %struct.layout.reloc.value
// IR-LABEL: define dso_local i32 @nested_field_load
// IR: call i64 asm sideeffect
// IR: getelementptr i8

// BOUNDS-LABEL: define dso_local i32 @array_field_load
// BOUNDS-NOT: getelementptr inbounds [5 x i32]
// BOUNDS: getelementptr inbounds i8
// BOUNDS-NOT: icmp ult i64 %{{.*}}, 5
// BOUNDS: ret i32
// IR: call i64 asm sideeffect
// IR: getelementptr i8

// ASM: .ascii "sample"
// ASM: .ascii "values"
// ASM: .ascii "basic:int"
// ASM: .section .llvm_struct_reloc,"",@progbits
// ASM: .word 1397902385
// ASM-NEXT: .hword 2
// ASM-NEXT: .hword 8
// ASM-NEXT: .hword 3
// ASM-NEXT: .hword 1
// ASM-NEXT: .word 72
// ASM: .xword global_array_field_size
// ASM: .word 20
// ASM: .hword 4

// ASM-LABEL: array_field_size:
// ASM: .ascii "sample"
// ASM: .ascii "values"
// ASM: .ascii "basic:int"
// ASM: .section .llvm_struct_reloc,"",@progbits
// ASM: .word 1397902385
// ASM-NEXT: .hword 2
// ASM-NEXT: .hword 7
// ASM-NEXT: .hword 1
// ASM-NEXT: .hword 1
// ASM-NEXT: .word 72
// ASM: .word 20
// ASM: .hword 4

// ASM: .ascii "sample"
// ASM: .ascii "child"
// ASM: .ascii "nested"
// ASM: .hword 2

// ASM: .ascii "nested"
// ASM: .ascii "value"
// ASM: .ascii "int"
// ASM: .hword 1
