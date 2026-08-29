// RUN: %clang_cc1 -triple aarch64-unknown-linux-gnu -fstruct-layout-reloc \
// RUN:   -fstruct-layout-reloc-global-extra-bytes=16 -emit-llvm -o - %s \
// RUN:   | FileCheck %s

struct kernel_param {
  const char *name;
  void *mod;
  const void *ops;
  unsigned short perm;
  signed char level;
  unsigned char flags;
  void *arg;
};

static struct kernel_param first
    __attribute__((used, section("__param"))) = {};
static struct kernel_param second
    __attribute__((used, section("__param"))) = {};

struct ordinary {
  int value;
};

static struct ordinary ordinary_object __attribute__((used)) = {1};

// __param objects must retain sizeof(struct kernel_param) as their stride.
// CHECK-NOT: %kernel_param.reloc = type
// Ordinary relocatable globals continue to receive the configured reserve.
// CHECK: %ordinary.reloc = type { %struct.ordinary, [16 x i8] }
// CHECK: @first = internal externally_initialized global %struct.kernel_param zeroinitializer, section "__param"
// CHECK: @second = internal externally_initialized global %struct.kernel_param zeroinitializer, section "__param"
// CHECK: @ordinary_object = internal externally_initialized global %ordinary.reloc
