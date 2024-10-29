#ifndef SYSTEM_DETECTION_H
#define SYSTEM_DETECTION_H

// Benutzt Boost, um __OS__, __PLATTFORM__, __KOMPILER__ und __ARCHITEKTUR__ zu definieren.

// Include Boost Predef library to access the predefined macros
#include <boost/predef.h>

// OS Detection
#if BOOST_OS_AIX
    #define __OS__ "AIX"
#elif BOOST_OS_AMIGAOS
    #define __OS__ "AmigaOS"
#elif BOOST_OS_BEOS
    #define __OS__ "BeOS"
#elif BOOST_OS_BSD
    #define __OS__ "BSD"
#elif BOOST_OS_CYGWIN
    #define __OS__ "Cygwin"
#elif BOOST_OS_HAIKU
    #define __OS__ "Haiku"
#elif BOOST_OS_HPUX
    #define __OS__ "HP-UX"
#elif BOOST_OS_IOS
    #define __OS__ "iOS"
#elif BOOST_OS_IRIX
    #define __OS__ "IRIX"
#elif BOOST_OS_LINUX
    #define __OS__ "Linux"
#elif BOOST_OS_MACOS
    #define __OS__ "macOS"
#elif BOOST_OS_OS400
    #define __OS__ "OS/400"
#elif BOOST_OS_QNX
    #define __OS__ "QNX"
#elif BOOST_OS_SOLARIS
    #define __OS__ "Solaris"
#elif BOOST_OS_UNIX
    #define __OS__ "Unix"
#elif BOOST_OS_SVR4
    #define __OS__ "SVR4"
#elif BOOST_OS_VMS
    #define __OS__ "VMS"
#elif BOOST_OS_WINDOWS
    #define __OS__ "Windows"
#elif BOOST_OS_BSD_BSDI
    #define __OS__ "BSD/BSDi"
#elif BOOST_OS_BSD_DRAGONFLY
    #define __OS__ "DragonFly BSD"
#elif BOOST_OS_BSD_FREE
    #define __OS__ "FreeBSD"
#elif BOOST_OS_BSD_NET
    #define __OS__ "NetBSD"
#elif BOOST_OS_BSD_OPEN
    #define __OS__ "OpenBSD"
#else
    #define __OS__ "Unknown OS"
#endif

// Platform Detection
#if BOOST_PLAT_ANDROID
    #define __PLATTFORM__ "Android"
#elif BOOST_PLAT_CLOUDABI
    #define __PLATTFORM__ "CloudABI"
#elif BOOST_PLAT_IOS_DEVICE
    #define __PLATTFORM__ "iOS Device"
#elif BOOST_PLAT_IOS_SIMULATOR
    #define __PLATTFORM__ "iOS Simulator"
#elif BOOST_PLAT_MINGW
    #define __PLATTFORM__ "MinGW"
#elif BOOST_PLAT_MINGW32
    #define __PLATTFORM__ "MinGW32"
#elif BOOST_PLAT_MINGW64
    #define __PLATTFORM__ "MinGW64"
#elif BOOST_PLAT_WINDOWS_DESKTOP
    #define __PLATTFORM__ "Windows Desktop"
#elif BOOST_PLAT_WINDOWS_PHONE
    #define __PLATTFORM__ "Windows Phone"
#elif BOOST_PLAT_WINDOWS_RUNTIME
    #define __PLATTFORM__ "Windows Runtime"
#elif BOOST_PLAT_WINDOWS_SERVER
    #define __PLATTFORM__ "Windows Server"
#elif BOOST_PLAT_WINDOWS_STORE
    #define __PLATTFORM__ "Windows Store"
#elif BOOST_PLAT_WINDOWS_SYSTEM
    #define __PLATTFORM__ "Windows System"
#elif BOOST_PLAT_WINDOWS_UWP
    #define __PLATTFORM__ "Windows UWP"
#else
    #define __PLATTFORM__ "Unknown Platform"
#endif

// Compiler Detection
#if BOOST_COMP_BORLAND
    #define __KOMPILER__ "Borland"
#elif BOOST_COMP_CLANG
    #define __KOMPILER__ "Clang"
#elif BOOST_COMP_COMO
    #define __KOMPILER__ "Comeau"
#elif BOOST_COMP_DEC
    #define __KOMPILER__ "DEC"
#elif BOOST_COMP_DIAB
    #define __KOMPILER__ "Diab"
#elif BOOST_COMP_DMC
    #define __KOMPILER__ "Digital Mars"
#elif BOOST_COMP_SYSC
    #define __KOMPILER__ "SCO"
#elif BOOST_COMP_EDG
    #define __KOMPILER__ "EDG"
#elif BOOST_COMP_PATH
    #define __KOMPILER__ "PathScale"
#elif BOOST_COMP_GNUC
    #define __KOMPILER__ "GCC"
#elif BOOST_COMP_GCCXML
    #define __KOMPILER__ "GCCXML"
#elif BOOST_COMP_GHS
    #define __KOMPILER__ "Green Hills"
#elif BOOST_COMP_HPACC
    #define __KOMPILER__ "HP/UX"
#elif BOOST_COMP_IAR
    #define __KOMPILER__ "IAR"
#elif BOOST_COMP_IBM
    #define __KOMPILER__ "IBM"
#elif BOOST_COMP_INTEL
    #define __KOMPILER__ "Intel"
#elif BOOST_COMP_KCC
    #define __KOMPILER__ "Kai"
#elif BOOST_COMP_LLVM
    #define __KOMPILER__ "LLVM"
#elif BOOST_COMP_HIGHC
    #define __KOMPILER__ "MetaWare HighC"
#elif BOOST_COMP_MWERKS
    #define __KOMPILER__ "Metrowerks CodeWarrior"
#elif BOOST_COMP_MRI
    #define __KOMPILER__ "MRI"
#elif BOOST_COMP_MPW
    #define __KOMPILER__ "MPW"
#elif BOOST_COMP_NVCC
    #define __KOMPILER__ "NVIDIA CUDA"
#elif BOOST_COMP_PALM
    #define __KOMPILER__ "Palm"
#elif BOOST_COMP_PGI
    #define __KOMPILER__ "PGI"
#elif BOOST_COMP_SGI
    #define __KOMPILER__ "SGI"
#elif BOOST_COMP_SUNPRO
    #define __KOMPILER__ "SunPro"
#elif BOOST_COMP_TENDRA
    #define __KOMPILER__ "TenDRA"
#elif BOOST_COMP_MSVC
    #define __KOMPILER__ "MSVC"
#elif BOOST_COMP_WATCOM
    #define __KOMPILER__ "Watcom"
#else
    #define __KOMPILER__ "Unknown Compiler"
#endif

// Architecture Detection
#if BOOST_ARCH_ALPHA
    #define __ARCHITEKTUR__ "Alpha"
#elif BOOST_ARCH_ARM
    #define __ARCHITEKTUR__ "ARM"
#elif BOOST_ARCH_BLACKFIN
    #define __ARCHITEKTUR__ "Blackfin"
#elif BOOST_ARCH_CONVEX
    #define __ARCHITEKTUR__ "Convex"
#elif BOOST_ARCH_E2K
    #define __ARCHITEKTUR__ "Elbrus 2000"
#elif BOOST_ARCH_IA64
    #define __ARCHITEKTUR__ "IA-64"
#elif BOOST_ARCH_M68K
    #define __ARCHITEKTUR__ "Motorola 68K"
#elif BOOST_ARCH_MIPS
    #define __ARCHITEKTUR__ "MIPS"
#elif BOOST_ARCH_PARISC
    #define __ARCHITEKTUR__ "PA-RISC"
#elif BOOST_ARCH_PPC
    #define __ARCHITEKTUR__ "PowerPC"
#elif BOOST_ARCH_PPC_64
    #define __ARCHITEKTUR__ "PowerPC 64"
#elif BOOST_ARCH_PTX
    #define __ARCHITEKTUR__ "PTX"
#elif BOOST_ARCH_PYRAMID
    #define __ARCHITEKTUR__ "Pyramid"
#elif BOOST_ARCH_RISCV
    #define __ARCHITEKTUR__ "RISC-V"
#elif BOOST_ARCH_RS6000
    #define __ARCHITEKTUR__ "RS/6000"
#elif BOOST_ARCH_SPARC
    #define __ARCHITEKTUR__ "SPARC"
#elif BOOST_ARCH_SH
    #define __ARCHITEKTUR__ "SuperH"
#elif BOOST_ARCH_SYS370
    #define __ARCHITEKTUR__ "System/370"
#elif BOOST_ARCH_SYS390
    #define __ARCHITEKTUR__ "System/390"
#elif BOOST_ARCH_X86
    #define __ARCHITEKTUR__ "x86"
#elif BOOST_ARCH_Z
    #define __ARCHITEKTUR__ "z/Architecture"
#elif BOOST_ARCH_X86_32
    #define __ARCHITEKTUR__ "x86_32"
#elif BOOST_ARCH_X86_64
    #define __ARCHITEKTUR__ "x86_64"
#else
    #define __ARCHITEKTUR__ "Unknown Architecture"
#endif

#endif // SYSTEM_DETECTION_H
