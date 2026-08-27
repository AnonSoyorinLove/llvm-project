// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc -fstruct-layout-reloc-global-extra-bytes=32 -emit-llvm -o - %s | FileCheck %s
// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc -fstruct-layout-reloc-global-extra-bytes=32 -O2 -emit-llvm -o - %s | FileCheck %s --check-prefix=OPT

struct sample {
  char pad;
  int value;
  long next;
};

struct sample g = {1, 7, 9};
static struct sample h __attribute__((used, section(".fixed"))) = {2, 8, 10};
static const struct sample optimized = {3, 11, 12};

int read_optimized(void) {
  return optimized.value;
}

// CHECK: .short 5
// CHECK: .short 6
// CHECK: %sample.reloc = type { %struct.sample, [32 x i8] }
// CHECK: @g = externally_initialized global %sample.reloc { %struct.sample { i8 1, i32 7, i64 9 }, [32 x i8] zeroinitializer }
// CHECK: @h = internal externally_initialized global %sample.reloc{{.*}} section ".fixed"

// OPT: @optimized = internal externally_initialized constant %sample.reloc
// OPT: @llvm.compiler.used = appending global [{{[0-9]+}} x ptr] {{.*}}ptr @optimized
// OPT-LABEL: define dso_local i32 @read_optimized
// OPT: call i64 asm sideeffect
// OPT: getelementptr i8, ptr @optimized
// OPT: load i32, ptr
