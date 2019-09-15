//===- Syntax.h -------------------------------------------------*- C++ -*-===//
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
#ifndef GTIRB_PP_SYNTAX_H
#define GTIRB_PP_SYNTAX_H

#include <string>

namespace gtirb_pprint {

class Syntax {
public:
  Syntax() = default;
  virtual ~Syntax();

  // Styles
  virtual const std::string& tab() const;
  virtual const std::string& comment() const = 0;

  // Sections
  virtual const std::string& textSection() const;
  virtual const std::string& dataSection() const;
  virtual const std::string& bssSection() const;

  // Directives
  virtual const std::string& nop() const;
  virtual const std::string& zeroByte() const;

  virtual const std::string& byteData() const = 0;
  virtual const std::string& longData() const = 0;
  virtual const std::string& quadData() const = 0;
  virtual const std::string& wordData() const = 0;

  virtual const std::string& text() const = 0;
  virtual const std::string& data() const = 0;
  virtual const std::string& bss() const = 0;

  virtual const std::string& section() const = 0;
  virtual const std::string& global() const = 0;
  virtual const std::string& align() const = 0;

  // Formatting helpers
  virtual std::string formatSymbolName(const std::string& x) const;
  virtual std::string avoidRegNameConflicts(const std::string& x) const;

  virtual std::string getSizeName(uint64_t x) const;
  virtual std::string getSizeName(const std::string& x) const;
  virtual std::string getSizeSuffix(uint64_t x) const;
  virtual std::string getSizeSuffix(const std::string& x) const;

protected:
  std::string TabStyle{"          "};

  std::string NopDirective{"nop"};
  std::string ZeroByteDirective{".byte 0x00"};

  std::string TextSection{".text"};
  std::string DataSection{".data"};
  std::string BssSection{".bss"};
};

} // namespace gtirb_pprint

#endif /* GTIRB_PP_SYNTAX_H */