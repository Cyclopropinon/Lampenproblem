#pragma once

// ctdi = Compile Time Device Infos

// Source: https://stackoverflow.com/a/66249936
// Modified for convenience
// Get current architecture, detectx nearly every architecture. Coded by Freak
    #if defined(__x86_64__) || defined(_M_X64)
        #define __ARCHITEKTUR__ "x86_64"
    #elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
        #define __ARCHITEKTUR__ "x86_32"
    #elif defined(__ARM_ARCH_2__)
        #define __ARCHITEKTUR__ "ARM2"
    #elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
        #define __ARCHITEKTUR__ "ARM3"
    #elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
        #define __ARCHITEKTUR__ "ARM4T"
    #elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
        #define __ARCHITEKTUR__ "ARM5
    #elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
        #define __ARCHITEKTUR__ "ARM6T2"
    #elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
        #define __ARCHITEKTUR__ "ARM6"
    #elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        #define __ARCHITEKTUR__ "ARM7"
    #elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        #define __ARCHITEKTUR__ "ARM7A"
    #elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
        #define __ARCHITEKTUR__ "ARM7R"
    #elif defined(__ARM_ARCH_7M__)
        #define __ARCHITEKTUR__ "ARM7M"
    #elif defined(__ARM_ARCH_7S__)
        #define __ARCHITEKTUR__ "ARM7S"
    #elif defined(__aarch64__) || defined(_M_ARM64)
        #define __ARCHITEKTUR__ "ARM64"
    #elif defined(mips) || defined(__mips__) || defined(__mips)
        #define __ARCHITEKTUR__ "MIPS"
    #elif defined(__sh__)
        #define __ARCHITEKTUR__ "SUPERH"
    #elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
        #define __ARCHITEKTUR__ "POWERPC"
    #elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
        #define __ARCHITEKTUR__ "POWERPC64"
    #elif defined(__sparc__) || defined(__sparc)
        #define __ARCHITEKTUR__ "SPARC"
    #elif defined(__m68k__)
        #define __ARCHITEKTUR__ "M68K"
    #else
        #define __ARCHITEKTUR__ "UNKNOWN"
    #endif

