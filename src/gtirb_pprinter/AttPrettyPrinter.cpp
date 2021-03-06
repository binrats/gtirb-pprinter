//===- AttPrettyPrinter.cpp -------------------------------------*- C++ -*-===//
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

#include "AttPrettyPrinter.hpp"
#include "string_utils.hpp"
#include "version.h"
#include <iomanip>

namespace gtirb_pprint {

AttPrettyPrinter::AttPrettyPrinter(gtirb::Context& context_,
                                   gtirb::Module& module_,
                                   const ElfSyntax& syntax_,
                                   const PrintingPolicy& policy_)
    : ElfPrettyPrinter(context_, module_, syntax_, policy_) {
  cs_option(this->csHandle, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
}

void AttPrettyPrinter::printHeader(std::ostream& /*os*/) {}

std::string AttPrettyPrinter::getRegisterName(unsigned int reg) const {
  return std::string{"%"} +
         ascii_str_tolower(PrettyPrinterBase::getRegisterName(reg));
}

void AttPrettyPrinter::printOpRegdirect(std::ostream& os, const cs_insn& inst,
                                        unsigned int reg) {
  if (cs_insn_group(this->csHandle, &inst, CS_GRP_CALL) ||
      cs_insn_group(this->csHandle, &inst, CS_GRP_JUMP))
    os << '*';
  os << getRegisterName(reg);
}

void AttPrettyPrinter::printOpImmediate(
    std::ostream& os, const gtirb::SymbolicExpression* symbolic,
    const cs_insn& inst, uint64_t index) {
  const cs_x86_op& op = inst.detail->x86.operands[index];
  assert(op.type == X86_OP_IMM &&
         "printOpImmediate called without an immediate operand");

  bool is_call = cs_insn_group(this->csHandle, &inst, CS_GRP_CALL);
  bool is_jump = cs_insn_group(this->csHandle, &inst, CS_GRP_JUMP);

  if (!is_call && !is_jump)
    os << '$';

  if (const gtirb::SymAddrConst* s = this->getSymbolicImmediate(symbolic)) {
    this->printSymbolicExpression(os, s, !is_call && !is_jump);
  } else {
    std::ios_base::fmtflags flags = os.flags();
    if (is_call || is_jump)
      os << std::setbase(16) << std::showbase;
    os << op.imm;
    os.flags(flags);
  }
}

void AttPrettyPrinter::printOpIndirect(
    std::ostream& os, const gtirb::SymbolicExpression* symbolic,
    const cs_insn& inst, uint64_t index) {
  const cs_x86& detail = inst.detail->x86;
  const cs_x86_op& op = detail.operands[index];
  assert(op.type == X86_OP_MEM &&
         "printOpIndirect called without a memory operand");

  bool has_segment = op.mem.segment != X86_REG_INVALID;
  bool has_base = op.mem.base != X86_REG_INVALID;
  bool has_index = op.mem.index != X86_REG_INVALID;

  if (cs_insn_group(this->csHandle, &inst, CS_GRP_CALL) ||
      cs_insn_group(this->csHandle, &inst, CS_GRP_JUMP))
    os << '*';
  if (has_segment)
    os << getRegisterName(op.mem.segment) << ':';

  if (const auto* s = std::get_if<gtirb::SymAddrConst>(symbolic)) {
    // Displacement is symbolic.
    printSymbolicExpression(os, s, false);
  } else {
    // Displacement is numeric.
    if (!has_segment && !has_base && !has_index) {
      std::ios_base::fmtflags flags = os.flags();
      os << "0x" << std::hex << op.mem.disp;
      os.flags(flags);
    } else if (op.mem.disp != 0 || has_segment) {
      os << op.mem.disp;
    } else {
      // Print nothing. There is no segment register and the base or index
      // register will be printed, so the zero displacement is implicit.
    }
  }

  // Print base, index, and scale.
  if (has_base || has_index) {
    os << '(';
    if (has_base)
      os << getRegisterName(op.mem.base);
    if (has_index) {
      os << ',' << getRegisterName(op.mem.index);
      if (op.mem.scale != 1)
        os << ',' << op.mem.scale;
    }
    os << ')';
  }
}

const PrintingPolicy& AttPrettyPrinterFactory::defaultPrintingPolicy() const {
  return ElfPrettyPrinter::defaultPrintingPolicy();
}

std::unique_ptr<PrettyPrinterBase>
AttPrettyPrinterFactory::create(gtirb::Context& gtirb_context,
                                gtirb::Module& module,
                                const PrintingPolicy& policy) {
  static const ElfSyntax syntax{};
  return std::make_unique<AttPrettyPrinter>(gtirb_context, module, syntax,
                                            policy);
}

volatile bool AttPrettyPrinter::registered = registerPrinter(
    {"elf"}, {"att"}, std::make_shared<AttPrettyPrinterFactory>());

} // namespace gtirb_pprint
