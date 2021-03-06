//===- Export.hpp -----------------------------------------------*- C++ -*-===//
//
//  Copyright (C) 2018 GrammaTech, Inc.
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
#ifndef GTIRB_PP_EXPORT_H
#define GTIRB_PP_EXPORT_H

///
/// \define DEBLOAT_PrettyPrinter_EXPORTS
///
/// Defined by the build system (CMake or SCons).
/// This should only be defined by the build system which generates the GT-IRB
/// library.  Users of the library should NOT define this.
///

///
/// \define DEBLOAT_PRETTYPRINTER_EXPORT_API
///
/// This controls the visibility of exported symbols (i.e. classes) in Windows
/// DLL's and Linux Shared Objects.
///

/// @cond INTERNAL
#ifndef __has_declspec_attribute
#define __has_declspec_attribute(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif
/// @endcond

#if defined(_MSC_VER) || __has_declspec_attribute(dllexport)
#if defined DEBLOAT_debloatPrettyPrinter_EXPORTS
#define DEBLOAT_PRETTYPRINTER_EXPORT_API _declspec(dllexport)
#else
#define DEBLOAT_PRETTYPRINTER_EXPORT_API _declspec(dllimport)
#endif
#elif defined(__GNUC__) || __has_attribute(visibility)
#define DEBLOAT_PRETTYPRINTER_EXPORT_API __attribute__((visibility("default")))
#else
#define DEBLOAT_PRETTYPRINTER_EXPORT_API
#endif

#endif /* GTIRB_PP_EXPORT_H */
