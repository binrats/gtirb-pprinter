//===- ElfPrinter.cpp -------------------------------------------*- C++ -*-===//
//
//  Copyright (C) 2019 GrammaTech, Inc.
//
//  This code is licensed under the MIT license. See the LICENSE file in the
//  project root for license terms.
//
//  This project is sponsored by the Office of Naval Research, One Liberty
//  Center, 875 N. Randolph Street, Arlington, VA 22203 under contract #
//  N68335-17-C-0700.  The content of the information does not necessarily
//  reflect the position or policy of the Government and no official
//  endorsement should be inferred.
//
//===----------------------------------------------------------------------===//
#include "ElfPrinter.h"

#include <elf.h>

namespace gtirb_pprint {
///
/// Print a comment that automatically scopes.
///
class BlockAreaComment {
public:
  BlockAreaComment(std::ostream& ss, std::string m = std::string{},
                   std::function<void()> f = []() {})
      : ofs{ss}, message{std::move(m)}, func{std::move(f)} {
    ofs << '\n';

    if (!message.empty()) {
      ofs << "# BEGIN - " << this->message << '\n';
    }

    func();
  }

  ~BlockAreaComment() {
    func();

    if (!message.empty()) {
      ofs << "# END   - " << this->message << '\n';
    }

    ofs << '\n';
  }

  std::ostream& ofs;
  const std::string message;
  std::function<void()> func;
};

ElfPrettyPrinter::ElfPrettyPrinter(gtirb::Context& context_, gtirb::IR& ir_,
                                   const string_range& keep_funcs,
                                   DebugStyle dbg_)
    : PrettyPrinterBase(context_, ir_, dbg_) {

  if (this->ir.modules()
          .begin()
          ->getAuxData<
              std::map<gtirb::Offset,
                       std::vector<std::tuple<std::string, std::vector<int64_t>,
                                              gtirb::UUID>>>>(
              "cfiDirectives")) {
    m_skip_sects.insert(".eh_frame");
  }

  for (const auto& [k, v] : m_syntax)
    syntax[k] = v;
  for (const auto name : keep_funcs)
    m_skip_funcs.erase(name);
  for (const auto name : m_skip_sects)
    skip_sects.insert(name);
  for (const auto name : m_skip_funcs)
    skip_funcs.insert(name);
  for (const auto name : m_skip_data)
    skip_data.insert(name);
}

void ElfPrettyPrinter::printSectionHeaderDirective(
    std::ostream& os, const gtirb::Section& section) {
  std::string sectionName = section.getName();
  os << syntax[Asm::Directive::Section] << ' ' << sectionName;
}

void ElfPrettyPrinter::printSectionProperties(std::ostream& os,
                                              const gtirb::Section& section) {
  const auto* elfSectionProperties =
      this->ir.modules()
          .begin()
          ->getAuxData<std::map<gtirb::UUID, std::tuple<uint64_t, uint64_t>>>(
              "elfSectionProperties");
  if (!elfSectionProperties)
    return;
  const auto sectionProperties = elfSectionProperties->find(section.getUUID());
  if (sectionProperties == elfSectionProperties->end())
    return;
  uint64_t type = std::get<0>(sectionProperties->second);
  uint64_t flags = std::get<1>(sectionProperties->second);
  os << " ,\"";
  if (flags & SHF_WRITE)
    os << "w";
  if (flags & SHF_ALLOC)
    os << "a";
  if (flags & SHF_EXECINSTR)
    os << "x";
  os << "\"";
  if (type == SHT_PROGBITS)
    os << ",@progbits";
  if (type == SHT_NOBITS)
    os << ",@nobits";
}

void ElfPrettyPrinter::printSectionFooterDirective(
    std::ostream& /* os */, const gtirb::Section& /* section */) {}

void ElfPrettyPrinter::printFunctionHeader(std::ostream& os, gtirb::Addr addr) {
  const std::string& name = this->getFunctionName(addr);

  if (!name.empty()) {
    const BlockAreaComment bac(os, "Function Header",
                               [this, &os]() { printBar(os, false); });
    printAlignment(os, addr);
    os << syntax[Asm::Directive::Global] << ' ' << name << '\n';
    os << ".type" << ' ' << name << ", @function\n";
    os << name << ":\n";
  }
}

void ElfPrettyPrinter::printByte(std::ostream& os, std::byte byte) {
  os << syntax[Asm::Directive::Byte] << " 0x" << std::hex
     << static_cast<uint32_t>(byte) << std::dec << '\n';
}

void ElfPrettyPrinter::printFooter(std::ostream& /* os */){};

} // namespace gtirb_pprint
