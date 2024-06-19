#pragma once

#ifndef _ENABLEBIGINTS_
    #line 17 "Lampenproblem.cpp"
    #warning "Is [Boost](https://www.boost.org/) installed? If yes, is it correctly modified to support bigints? Compiling without bigints..."
#endif

#ifdef _DISABLELIBFLINT_
    #line 35 "Lampenproblem.cpp"
    #warning "[FLINT](https://flintlib.org/) not found! Compiling without FLINT..."
#endif

#ifdef _DISABLELIBFLINTXX_
    #line 35 "Lampenproblem.cpp"
    #warning "[FLINTxx](https://github.com/flintlib/flintxx) not found! Compiling without FLINTxx..."
#endif
