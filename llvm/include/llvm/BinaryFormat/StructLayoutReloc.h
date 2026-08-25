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
inline constexpr uint16_t CurrentVersion = 2;

enum class Kind : uint16_t {
  FieldOffsetCode = 1,
  TypeSizeCode = 2,
  FieldOffsetData = 3,
  TypeSizeData = 4,
  GlobalObjectLayout = 5,
  GlobalInitField = 6,
  FieldSizeCode = 7,
  FieldSizeData = 8,
};

enum class PatchKind : uint16_t {
  None = 0,
  AArch64MovwU32 = 1,
  AArch64DataU32 = 2,
  AArch64DataU64 = 3,
};

enum Flags : uint16_t {
  IsStruct = 1U << 0,
};

enum class FieldTypeKind : uint16_t {
  None = 0,
  Basic = 1,
  Struct = 2,
  Union = 3,
  Array = 4,
  Pointer = 5,
  Enum = 6,
  Function = 7,
  Other = 8,
};

/// Fixed-size record emitted into .llvm_struct_reloc. Array bounds are
/// intentionally omitted from FieldTypeName so a target array length may
/// differ while its element type remains compatible.
///
/// PatchSite, TypeName, FieldName, and FieldTypeName are populated through
/// normal ELF relocations. A patcher can therefore resolve them in an ET_REL
/// object without requiring a custom ELF relocation type.
struct Record {
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
  uint16_t FieldType;
  uint16_t Reserved;
  uint64_t FieldTypeName;
};

static_assert(sizeof(Record) == 72, "unexpected struct relocation size");
static_assert(offsetof(Record, PatchSite) == 16,
              "unexpected patch-site offset");
static_assert(offsetof(Record, FieldTypeName) == 64,
              "unexpected field-type-name offset");

} // namespace struct_layout_reloc
} // namespace llvm

#endif // LLVM_BINARYFORMAT_STRUCTLAYOUTRELOC_H
