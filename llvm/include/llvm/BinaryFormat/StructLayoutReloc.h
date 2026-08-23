//===- StructLayoutReloc.h - Struct layout relocation format -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_BINARYFORMAT_STRUCTLAYOUTRELOC_H
#define LLVM_BINARYFORMAT_STRUCTLAYOUTRELOC_H

#include <cstddef>
#include <cstdint>

namespace llvm {
namespace struct_layout_reloc {

inline constexpr uint32_t RecordMagic = 0x53524c31; // SRL1
inline constexpr uint16_t CurrentVersion = 1;

enum class Kind : uint16_t {
  FieldOffsetCode = 1,
  TypeSizeCode = 2,
  FieldOffsetData = 3,
  TypeSizeData = 4,
};

enum class PatchKind : uint16_t {
  AArch64MovwU32 = 1,
  AArch64DataU32 = 2,
  AArch64DataU64 = 3,
};

enum Flags : uint16_t {
  IsStruct = 1U << 0,
};

/// Fixed-size record emitted into .llvm_struct_reloc.
///
/// PatchSite, TypeName, and FieldName are populated through normal ELF
/// relocations. A patcher can therefore resolve them in an ET_REL object
/// without requiring a custom ELF relocation type.
struct RecordV1 {
  uint32_t Magic;
  uint16_t Version;
  uint16_t RelocKind;
  uint16_t Encoding;
  uint16_t Flags;
  uint32_t RecordSize;
  uint64_t PatchSite;
  uint64_t TypeName;
  uint64_t FieldName;
  uint32_t CompiledTypeSize;
  uint32_t CompiledFieldOffset;
  uint32_t CompiledFieldSize;
  uint32_t FieldIndex;
  uint32_t PatchInstructionCount;
  uint32_t Reserved;
};

static_assert(sizeof(RecordV1) == 64, "unexpected struct relocation size");
static_assert(offsetof(RecordV1, PatchSite) == 16,
              "unexpected patch-site offset");

} // namespace struct_layout_reloc
} // namespace llvm

#endif // LLVM_BINARYFORMAT_STRUCTLAYOUTRELOC_H
