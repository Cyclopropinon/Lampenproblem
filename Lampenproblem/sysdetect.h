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

// Compiler Detection and Version
#if BOOST_COMP_BORLAND
    #define __KOMPILER__ "Borland"
    #define __KOMPILER_V__ BOOST_COMP_BORLAND_VERSION
#elif BOOST_COMP_CLANG
    #define __KOMPILER__ "Clang"
    #define __KOMPILER_V__ BOOST_COMP_CLANG_VERSION
#elif BOOST_COMP_COMO
    #define __KOMPILER__ "Comeau"
    #define __KOMPILER_V__ BOOST_COMP_COMO_VERSION
#elif BOOST_COMP_DEC
    #define __KOMPILER__ "DEC"
    #define __KOMPILER_V__ BOOST_COMP_DEC_VERSION
#elif BOOST_COMP_DIAB
    #define __KOMPILER__ "Diab"
    #define __KOMPILER_V__ BOOST_COMP_DIAB_VERSION
#elif BOOST_COMP_DMC
    #define __KOMPILER__ "Digital Mars"
    #define __KOMPILER_V__ BOOST_COMP_DMC_VERSION
#elif BOOST_COMP_SYSC
    #define __KOMPILER__ "SCO"
    #define __KOMPILER_V__ BOOST_COMP_SYSC_VERSION
#elif BOOST_COMP_EDG
    #define __KOMPILER__ "EDG"
    #define __KOMPILER_V__ BOOST_COMP_EDG_VERSION
#elif BOOST_COMP_PATH
    #define __KOMPILER__ "PathScale"
    #define __KOMPILER_V__ BOOST_COMP_PATH_VERSION
#elif BOOST_COMP_GNUC
    #define __KOMPILER__ "GCC"
    #define __KOMPILER_V__ BOOST_COMP_GNUC_VERSION
#elif BOOST_COMP_GCCXML
    #define __KOMPILER__ "GCCXML"
    #define __KOMPILER_V__ BOOST_COMP_GCCXML_VERSION
#elif BOOST_COMP_GHS
    #define __KOMPILER__ "Green Hills"
    #define __KOMPILER_V__ BOOST_COMP_GHS_VERSION
#elif BOOST_COMP_HPACC
    #define __KOMPILER__ "HP/UX"
    #define __KOMPILER_V__ BOOST_COMP_HPACC_VERSION
#elif BOOST_COMP_IAR
    #define __KOMPILER__ "IAR"
    #define __KOMPILER_V__ BOOST_COMP_IAR_VERSION
#elif BOOST_COMP_IBM
    #define __KOMPILER__ "IBM"
    #define __KOMPILER_V__ BOOST_COMP_IBM_VERSION
#elif BOOST_COMP_INTEL
    #define __KOMPILER__ "Intel"
    #define __KOMPILER_V__ BOOST_COMP_INTEL_VERSION
#elif BOOST_COMP_KCC
    #define __KOMPILER__ "Kai"
    #define __KOMPILER_V__ BOOST_COMP_KCC_VERSION
#elif BOOST_COMP_LLVM
    #define __KOMPILER__ "LLVM"
    #define __KOMPILER_V__ BOOST_COMP_LLVM_VERSION
#elif BOOST_COMP_HIGHC
    #define __KOMPILER__ "MetaWare HighC"
    #define __KOMPILER_V__ BOOST_COMP_HIGHC_VERSION
#elif BOOST_COMP_MWERKS
    #define __KOMPILER__ "Metrowerks CodeWarrior"
    #define __KOMPILER_V__ BOOST_COMP_MWERKS_VERSION
#elif BOOST_COMP_MRI
    #define __KOMPILER__ "MRI"
    #define __KOMPILER_V__ BOOST_COMP_MRI_VERSION
#elif BOOST_COMP_MPW
    #define __KOMPILER__ "MPW"
    #define __KOMPILER_V__ BOOST_COMP_MPW_VERSION
#elif BOOST_COMP_NVCC
    #define __KOMPILER__ "NVIDIA CUDA"
    #define __KOMPILER_V__ BOOST_COMP_NVCC_VERSION
#elif BOOST_COMP_PALM
    #define __KOMPILER__ "Palm"
    #define __KOMPILER_V__ BOOST_COMP_PALM_VERSION
#elif BOOST_COMP_PGI
    #define __KOMPILER__ "PGI"
    #define __KOMPILER_V__ BOOST_COMP_PGI_VERSION
#elif BOOST_COMP_SGI
    #define __KOMPILER__ "SGI"
    #define __KOMPILER_V__ BOOST_COMP_SGI_VERSION
#elif BOOST_COMP_SUNPRO
    #define __KOMPILER__ "SunPro"
    #define __KOMPILER_V__ BOOST_COMP_SUNPRO_VERSION
#elif BOOST_COMP_TENDRA
    #define __KOMPILER__ "TenDRA"
    #define __KOMPILER_V__ BOOST_COMP_TENDRA_VERSION
#elif BOOST_COMP_MSVC
    #define __KOMPILER__ "MSVC"
    #define __KOMPILER_V__ BOOST_COMP_MSVC_VERSION
#elif BOOST_COMP_WATCOM
    #define __KOMPILER__ "Watcom"
    #define __KOMPILER_V__ BOOST_COMP_WATCOM_VERSION
#else
    #define __KOMPILER__ "Unknown Compiler"
    #define __KOMPILER_V__ 0
#endif

// Architecture Detection and Version
#if BOOST_ARCH_ALPHA
    #define __ARCHITEKTUR__ "Alpha"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_ALPHA_VERSION
#elif BOOST_ARCH_ARM
    #define __ARCHITEKTUR__ "ARM"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_ARM_VERSION
#elif BOOST_ARCH_BLACKFIN
    #define __ARCHITEKTUR__ "Blackfin"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_BLACKFIN_VERSION
#elif BOOST_ARCH_CONVEX
    #define __ARCHITEKTUR__ "Convex"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_CONVEX_VERSION
#elif BOOST_ARCH_E2K
    #define __ARCHITEKTUR__ "Elbrus 2000"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_E2K_VERSION
#elif BOOST_ARCH_IA64
    #define __ARCHITEKTUR__ "IA-64"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_IA64_VERSION
#elif BOOST_ARCH_M68K
    #define __ARCHITEKTUR__ "Motorola 68K"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_M68K_VERSION
#elif BOOST_ARCH_MIPS
    #define __ARCHITEKTUR__ "MIPS"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_MIPS_VERSION
#elif BOOST_ARCH_PARISC
    #define __ARCHITEKTUR__ "PA-RISC"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PARISC_VERSION
#elif BOOST_ARCH_PPC
    #define __ARCHITEKTUR__ "PowerPC"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PPC_VERSION
#elif BOOST_ARCH_PPC_64
    #define __ARCHITEKTUR__ "PowerPC 64"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PPC_64_VERSION
#elif BOOST_ARCH_PTX
    #define __ARCHITEKTUR__ "PTX"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PTX_VERSION
#elif BOOST_ARCH_PYRAMID
    #define __ARCHITEKTUR__ "Pyramid"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PYRAMID_VERSION
#elif BOOST_ARCH_RISCV
    #define __ARCHITEKTUR__ "RISC-V"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_RISCV_VERSION
#elif BOOST_ARCH_RS6000
    #define __ARCHITEKTUR__ "RS/6000"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_RS6000_VERSION
#elif BOOST_ARCH_SPARC
    #define __ARCHITEKTUR__ "SPARC"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SPARC_VERSION
#elif BOOST_ARCH_SH
    #define __ARCHITEKTUR__ "SuperH"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SH_VERSION
#elif BOOST_ARCH_SYS370
    #define __ARCHITEKTUR__ "System/370"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SYS370_VERSION
#elif BOOST_ARCH_SYS390
    #define __ARCHITEKTUR__ "System/390"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SYS390_VERSION
#elif BOOST_ARCH_X86
    #define __ARCHITEKTUR__ "x86"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_X86_VERSION
#elif BOOST_ARCH_Z
    #define __ARCHITEKTUR__ "z/Architecture"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_Z_VERSION
#elif BOOST_ARCH_X86_32
    #define __ARCHITEKTUR__ "x86_32"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_X86_32_VERSION
#elif BOOST_ARCH_X86_64
    #define __ARCHITEKTUR__ "x86_64"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_X86_64_VERSION
#else
    #define __ARCHITEKTUR__ "Unknown Architecture"
    #define __ARCHITEKTUR_V__ 0
#endif

#endif // SYSTEM_DETECTION_H
