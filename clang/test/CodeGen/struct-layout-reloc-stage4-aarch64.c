// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc -fstruct-layout-reloc-global-reserve=32 -emit-llvm -o - %s | FileCheck %s

struct sample {
  char pad;
  int value;
  long next;
};

struct sample g = {1, 7, 9};
static struct sample h __attribute__((used, section(".fixed"))) = {2, 8, 10};

// CHECK: .short 5
// CHECK: .short 6
// CHECK: %sample.reloc = type { %struct.sample, [16 x i8] }
// CHECK: @g = global %sample.reloc { %struct.sample { i8 1, i32 7, i64 9 }, [16 x i8] zeroinitializer }
// CHECK: @h = internal global %sample.reloc{{.*}} section ".fixed"
