// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=RELOC
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu \
// RUN:   -emit-llvm -o - %s | FileCheck %s --check-prefix=PLAIN

struct sample {
  char pad;
  int value;
  long next;
};

void local_object(void) {
  struct sample s;
  s.value = 1;
}

void local_array(unsigned long index) {
  struct sample values[3];
  values[index].value = 7;
}

unsigned long array_size(void) {
  return sizeof(struct sample[3]);
}

int pointer_add(struct sample *p, unsigned long index) {
  return (p + index)->value;
}

void pointer_increment(struct sample **p) {
  ++*p;
}

long pointer_difference(struct sample *p, struct sample *q) {
  return p - q;
}

void aggregate_copy(void) {
  struct sample a;
  struct sample b;
  a.value = 1;
  b = a;
}

void zero_initializer(void) {
  struct sample s = {0};
}

void nonzero_initializer(void) {
  struct sample s = {0, 7, 9};
}

void array_initializer(void) {
  struct sample values[2] = {{0, 7, 9}, {1, 8, 10}};
}

// RELOC-LABEL: define dso_local void @local_object()
// RELOC: %struct.layout.reloc.value = call i64 asm sideeffect
// RELOC: %s = alloca i8, i64 %struct.layout.reloc.value, align 8
// RELOC: %{{.*}} = getelementptr i8, ptr %s, i64 %struct.field.offset

// RELOC-LABEL: define dso_local void @local_array(i64 noundef %index)
// RELOC: %struct.layout.reloc.object.size = mul nuw i64 %struct.layout.reloc.value, 3
// RELOC: %values = alloca i8, i64 %struct.layout.reloc.object.size, align 8
// RELOC: %struct.layout.reloc.stride = mul i64 %{{.*}}, %struct.layout.reloc.value{{.*}}
// RELOC: %struct.layout.reloc.arrayidx = getelementptr inbounds i8, ptr %values, i64 %struct.layout.reloc.stride

// RELOC-LABEL: define dso_local i32 @pointer_add
// RELOC: %struct.layout.reloc.value = call i64 asm sideeffect
// RELOC: %reloc.index = mul nsw i64 %{{.*}}, %struct.layout.reloc.value
// RELOC: %add.ptr = getelementptr inbounds i8, ptr %{{.*}}, i64 %reloc.index

// RELOC-LABEL: define dso_local void @pointer_increment
// RELOC: %struct.layout.reloc.value = call i64 asm sideeffect
// RELOC: %reloc.inc.offset = mul i64 1, %struct.layout.reloc.value
// RELOC: %incdec.ptr = getelementptr inbounds i8, ptr %{{.*}}, i64 %reloc.inc.offset

// RELOC-LABEL: define dso_local i64 @pointer_difference
// RELOC: %struct.layout.reloc.value = call i64 asm sideeffect
// RELOC: %sub.ptr.div = sdiv exact i64 %sub.ptr.sub, %struct.layout.reloc.value

// RELOC-LABEL: define dso_local void @aggregate_copy()
// RELOC: %struct.layout.reloc.value = call i64 asm sideeffect
// RELOC: call void @llvm.memcpy.p0.p0.i64(ptr align 8 %{{.*}}, ptr align 8 %{{.*}}, i64 %struct.layout.reloc.value{{.*}}, i1 false)

// RELOC-LABEL: define dso_local void @zero_initializer()
// RELOC: %{{.*}} = alloca i8, i64 %struct.layout.reloc.value, align 8
// RELOC: %struct.layout.reloc.value{{.*}} = call i64 asm sideeffect
// RELOC: call void @llvm.memset.p0.i64(ptr align 8 %{{.*}}, i8 0, i64 %struct.layout.reloc.value{{.*}}, i1 false)

// PLAIN-LABEL: define dso_local void @local_object()
// PLAIN: %s = alloca %struct.sample, align 8
// PLAIN-NOT: asm sideeffect
// PLAIN-LABEL: define dso_local void @local_array
// PLAIN: %values = alloca [3 x %struct.sample], align 8
// PLAIN-NOT: asm sideeffect
// PLAIN-LABEL: define dso_local i64 @pointer_difference
// PLAIN: %sub.ptr.div = sdiv exact i64 %sub.ptr.sub, 16
