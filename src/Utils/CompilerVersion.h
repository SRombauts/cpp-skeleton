/**
 * @file    CompilerVersion.h
 * @ingroup Utils
 * @brief   Get the version of the C++ compiler used.
 *
 * Copyright (c) 2017 Sebastien Rombauts (sebastien.rombauts@gmail.com)
*/
#pragma once

#include <string>


namespace Utils {

/**
 * @brief   Get the version of the C++ compiler used.
 * @ingroup Utils
 *
 * Works for the 3 main compilers GCC, Clang and Microsoft Visual Studio
*/
class CompilerVersion
{
////////////////////////////////////////////////////////////////////////////////
// Méthodes Publiques
////////////////////////////////////////////////////////////////////////////////
public:
    /// Extract informations for the main compilers
    CompilerVersion() :
#ifdef  _MSC_VER // _WIN32
        mbIsValid      (true),
        mName          ("MSVC"),
        mVersion       (std::to_string(_MSC_VER)),
        mVersionFull   (std::to_string(_MSC_FULL_VER)),
        mVersionInt    (_MSC_VER)
#else
#ifdef __GNUC__
        mbIsValid      (true),
#ifdef __clang__
        mName          ("clang"),
        mVersion       (Formatter() << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__),
        mVersionFull   (__clang_version__),
        mVersionInt    (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
#else
        mName          ("GCC"),
        mVersion       (Formatter() << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__),
        mVersionFull   (mVersion),
        mVersionInt    (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif
#else
        mbIsValid      (false),
        mVersionInt    (0)
#endif
#endif
    {}

    /// True if using a known compiler
    inline bool isValid() const {
        return mbIsValid;
    }

    /// Compiler's name (GCC, Clang or MSVC)
    inline const std::string& getName() const {
        return mName;
    }

    /// Short version string (for instance "3.8.0" or "1900")
    inline const std::string& getVersion() const {
        return mVersion;
    }

    /// Full version string (for instance "3.8.0 (tags/RELEASE_380/final)" or "150020706")
    inline const std::string& getVersionFull() const {
        return mVersionFull;
    }

    /// Integer version (for instance 30800 or 1900)
    inline int getVersionInt() const {
        return mVersionInt;
    }

    /// Compose description with compiler's name and full version string
    inline std::string formatDescription() const {
        return Formatter() << mName << " " << mVersionFull;
    }

////////////////////////////////////////////////////////////////////////////////
// Variables Membres Privées
////////////////////////////////////////////////////////////////////////////////
private:
   bool           mbIsValid;     ///< Validity flag
   std::string    mName;         ///< Compiler's name (GCC, Clang or MSVC)
   std::string    mVersion;      ///< Short version string (for instance "3.8.0" or "1900")
   std::string    mVersionFull;  ///< Full version string (for instance "3.8.0 (tags/RELEASE_380/final)" or "150020706")
   unsigned int   mVersionInt;   ///< Integer version (for instance 30800 or 1900)
};

} // namespace Utils

