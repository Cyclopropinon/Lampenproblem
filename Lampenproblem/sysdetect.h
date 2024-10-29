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
    #define __KOMPILER_V__ BOOST_COMP_BORLAND_DETECTION
#elif BOOST_COMP_CLANG
    #define __KOMPILER__ "Clang"
    #define __KOMPILER_V__ BOOST_COMP_CLANG_DETECTION
#elif BOOST_COMP_COMO
    #define __KOMPILER__ "Comeau"
    #define __KOMPILER_V__ BOOST_COMP_COMO_DETECTION
#elif BOOST_COMP_DEC
    #define __KOMPILER__ "DEC"
    #define __KOMPILER_V__ BOOST_COMP_DEC_DETECTION
#elif BOOST_COMP_DIAB
    #define __KOMPILER__ "Diab"
    #define __KOMPILER_V__ BOOST_COMP_DIAB_DETECTION
#elif BOOST_COMP_DMC
    #define __KOMPILER__ "Digital Mars"
    #define __KOMPILER_V__ BOOST_COMP_DMC_DETECTION
#elif BOOST_COMP_SYSC
    #define __KOMPILER__ "SCO"
    #define __KOMPILER_V__ BOOST_COMP_SYSC_DETECTION
#elif BOOST_COMP_EDG
    #define __KOMPILER__ "EDG"
    #define __KOMPILER_V__ BOOST_COMP_EDG_DETECTION
#elif BOOST_COMP_PATH
    #define __KOMPILER__ "PathScale"
    #define __KOMPILER_V__ BOOST_COMP_PATH_DETECTION
#elif BOOST_COMP_GNUC
    #define __KOMPILER__ "GCC"
    #define __KOMPILER_V__ BOOST_COMP_GNUC_DETECTION
#elif BOOST_COMP_GCCXML
    #define __KOMPILER__ "GCCXML"
    #define __KOMPILER_V__ BOOST_COMP_GCCXML_DETECTION
#elif BOOST_COMP_GHS
    #define __KOMPILER__ "Green Hills"
    #define __KOMPILER_V__ BOOST_COMP_GHS_DETECTION
#elif BOOST_COMP_HPACC
    #define __KOMPILER__ "HP/UX"
    #define __KOMPILER_V__ BOOST_COMP_HPACC_DETECTION
#elif BOOST_COMP_IAR
    #define __KOMPILER__ "IAR"
    #define __KOMPILER_V__ BOOST_COMP_IAR_DETECTION
#elif BOOST_COMP_IBM
    #define __KOMPILER__ "IBM"
    #define __KOMPILER_V__ BOOST_COMP_IBM_DETECTION
#elif BOOST_COMP_INTEL
    #define __KOMPILER__ "Intel"
    #define __KOMPILER_V__ BOOST_COMP_INTEL_DETECTION
#elif BOOST_COMP_KCC
    #define __KOMPILER__ "Kai"
    #define __KOMPILER_V__ BOOST_COMP_KCC_DETECTION
#elif BOOST_COMP_LLVM
    #define __KOMPILER__ "LLVM"
    #define __KOMPILER_V__ BOOST_COMP_LLVM_DETECTION
#elif BOOST_COMP_HIGHC
    #define __KOMPILER__ "MetaWare HighC"
    #define __KOMPILER_V__ BOOST_COMP_HIGHC_DETECTION
#elif BOOST_COMP_MWERKS
    #define __KOMPILER__ "Metrowerks CodeWarrior"
    #define __KOMPILER_V__ BOOST_COMP_MWERKS_DETECTION
#elif BOOST_COMP_MRI
    #define __KOMPILER__ "MRI"
    #define __KOMPILER_V__ BOOST_COMP_MRI_DETECTION
#elif BOOST_COMP_MPW
    #define __KOMPILER__ "MPW"
    #define __KOMPILER_V__ BOOST_COMP_MPW_DETECTION
#elif BOOST_COMP_NVCC
    #define __KOMPILER__ "NVIDIA CUDA"
    #define __KOMPILER_V__ BOOST_COMP_NVCC_DETECTION
#elif BOOST_COMP_PALM
    #define __KOMPILER__ "Palm"
    #define __KOMPILER_V__ BOOST_COMP_PALM_DETECTION
#elif BOOST_COMP_PGI
    #define __KOMPILER__ "PGI"
    #define __KOMPILER_V__ BOOST_COMP_PGI_DETECTION
#elif BOOST_COMP_SGI
    #define __KOMPILER__ "SGI"
    #define __KOMPILER_V__ BOOST_COMP_SGI_DETECTION
#elif BOOST_COMP_SUNPRO
    #define __KOMPILER__ "SunPro"
    #define __KOMPILER_V__ BOOST_COMP_SUNPRO_DETECTION
#elif BOOST_COMP_TENDRA
    #define __KOMPILER__ "TenDRA"
    #define __KOMPILER_V__ BOOST_COMP_TENDRA_DETECTION
#elif BOOST_COMP_MSVC
    #define __KOMPILER__ "MSVC"
    #define __KOMPILER_V__ BOOST_COMP_MSVC_DETECTION
#elif BOOST_COMP_WATCOM
    #define __KOMPILER__ "Watcom"
    #define __KOMPILER_V__ BOOST_COMP_WATCOM_DETECTION
#else
    #define __KOMPILER__ "Unknown Compiler"
    #define __KOMPILER_V__ 0
#endif


// Compiler Detection and Version List
#define ADD_COMPILER(name, version) name " " STR(version)

#define COMPILER_IF(cond, name, version) \
    cond ? ADD_COMPILER(name, version) :

#define __COMPILER_LIST__ \
    COMPILER_IF(BOOST_COMP_BORLAND, "Borland", BOOST_COMP_BORLAND_DETECTION) \
    COMPILER_IF(BOOST_COMP_CLANG, "Clang", BOOST_COMP_CLANG_DETECTION) \
    COMPILER_IF(BOOST_COMP_COMO, "Comeau", BOOST_COMP_COMO_DETECTION) \
    COMPILER_IF(BOOST_COMP_DEC, "DEC", BOOST_COMP_DEC_DETECTION) \
    COMPILER_IF(BOOST_COMP_DIAB, "Diab", BOOST_COMP_DIAB_DETECTION) \
    COMPILER_IF(BOOST_COMP_DMC, "Digital Mars", BOOST_COMP_DMC_DETECTION) \
    COMPILER_IF(BOOST_COMP_SYSC, "SCO", BOOST_COMP_SYSC_DETECTION) \
    COMPILER_IF(BOOST_COMP_EDG, "EDG", BOOST_COMP_EDG_DETECTION) \
    COMPILER_IF(BOOST_COMP_PATH, "PathScale", BOOST_COMP_PATH_DETECTION) \
    COMPILER_IF(BOOST_COMP_GNUC, "GCC", BOOST_COMP_GNUC_DETECTION) \
    COMPILER_IF(BOOST_COMP_GCCXML, "GCCXML", BOOST_COMP_GCCXML_DETECTION) \
    COMPILER_IF(BOOST_COMP_GHS, "Green Hills", BOOST_COMP_GHS_DETECTION) \
    COMPILER_IF(BOOST_COMP_HPACC, "HP/UX", BOOST_COMP_HPACC_DETECTION) \
    COMPILER_IF(BOOST_COMP_IAR, "IAR", BOOST_COMP_IAR_DETECTION) \
    COMPILER_IF(BOOST_COMP_IBM, "IBM", BOOST_COMP_IBM_DETECTION) \
    COMPILER_IF(BOOST_COMP_INTEL, "Intel", BOOST_COMP_INTEL_DETECTION) \
    COMPILER_IF(BOOST_COMP_KCC, "Kai", BOOST_COMP_KCC_DETECTION) \
    COMPILER_IF(BOOST_COMP_LLVM, "LLVM", BOOST_COMP_LLVM_DETECTION) \
    COMPILER_IF(BOOST_COMP_HIGHC, "MetaWare HighC", BOOST_COMP_HIGHC_DETECTION) \
    COMPILER_IF(BOOST_COMP_MWERKS, "Metrowerks CodeWarrior", BOOST_COMP_MWERKS_DETECTION) \
    COMPILER_IF(BOOST_COMP_MRI, "MRI", BOOST_COMP_MRI_DETECTION) \
    COMPILER_IF(BOOST_COMP_MPW, "MPW", BOOST_COMP_MPW_DETECTION) \
    COMPILER_IF(BOOST_COMP_NVCC, "NVIDIA CUDA", BOOST_COMP_NVCC_DETECTION) \
    COMPILER_IF(BOOST_COMP_PALM, "Palm", BOOST_COMP_PALM_DETECTION) \
    COMPILER_IF(BOOST_COMP_PGI, "PGI", BOOST_COMP_PGI_DETECTION) \
    COMPILER_IF(BOOST_COMP_SGI, "SGI", BOOST_COMP_SGI_DETECTION) \
    COMPILER_IF(BOOST_COMP_SUNPRO, "SunPro", BOOST_COMP_SUNPRO_DETECTION) \
    COMPILER_IF(BOOST_COMP_TENDRA, "TenDRA", BOOST_COMP_TENDRA_DETECTION) \
    COMPILER_IF(BOOST_COMP_MSVC, "MSVC", BOOST_COMP_MSVC_DETECTION) \
    COMPILER_IF(BOOST_COMP_WATCOM, "Watcom", BOOST_COMP_WATCOM_DETECTION) \
    ""


// Architecture Detection and Version
#if BOOST_ARCH_ALPHA
    #define __ARCHITEKTUR__ "Alpha"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_ALPHA
#elif BOOST_ARCH_ARM
    #define __ARCHITEKTUR__ "ARM"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_ARM
#elif BOOST_ARCH_BLACKFIN
    #define __ARCHITEKTUR__ "Blackfin"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_BLACKFIN
#elif BOOST_ARCH_CONVEX
    #define __ARCHITEKTUR__ "Convex"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_CONVEX
#elif BOOST_ARCH_E2K
    #define __ARCHITEKTUR__ "Elbrus 2000"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_E2K
#elif BOOST_ARCH_IA64
    #define __ARCHITEKTUR__ "IA-64"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_IA64
#elif BOOST_ARCH_M68K
    #define __ARCHITEKTUR__ "Motorola 68K"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_M68K
#elif BOOST_ARCH_MIPS
    #define __ARCHITEKTUR__ "MIPS"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_MIPS
#elif BOOST_ARCH_PARISC
    #define __ARCHITEKTUR__ "PA-RISC"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PARISC
#elif BOOST_ARCH_PPC
    #define __ARCHITEKTUR__ "PowerPC"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PPC
#elif BOOST_ARCH_PPC_64
    #define __ARCHITEKTUR__ "PowerPC 64"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PPC_64
#elif BOOST_ARCH_PTX
    #define __ARCHITEKTUR__ "PTX"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PTX
#elif BOOST_ARCH_PYRAMID
    #define __ARCHITEKTUR__ "Pyramid"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_PYRAMID
#elif BOOST_ARCH_RISCV
    #define __ARCHITEKTUR__ "RISC-V"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_RISCV
#elif BOOST_ARCH_RS6000
    #define __ARCHITEKTUR__ "RS/6000"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_RS6000
#elif BOOST_ARCH_SPARC
    #define __ARCHITEKTUR__ "SPARC"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SPARC
#elif BOOST_ARCH_SH
    #define __ARCHITEKTUR__ "SuperH"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SH
#elif BOOST_ARCH_SYS370
    #define __ARCHITEKTUR__ "System/370"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SYS370
#elif BOOST_ARCH_SYS390
    #define __ARCHITEKTUR__ "System/390"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_SYS390
#elif BOOST_ARCH_X86
    #define __ARCHITEKTUR__ "x86"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_X86
#elif BOOST_ARCH_Z
    #define __ARCHITEKTUR__ "z/Architecture"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_Z
#elif BOOST_ARCH_X86_32
    #define __ARCHITEKTUR__ "x86_32"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_X86_32
#elif BOOST_ARCH_X86_64
    #define __ARCHITEKTUR__ "x86_64"
    #define __ARCHITEKTUR_V__ BOOST_ARCH_X86_64
#else
    #define __ARCHITEKTUR__ "Unknown Architecture"
    #define __ARCHITEKTUR_V__ 0
#endif

#endif // SYSTEM_DETECTION_H
