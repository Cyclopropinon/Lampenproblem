#pragma once

#ifndef _ENABLEBIGINTS_
    #warning "Is [Boost](https://www.boost.org/) installed? If yes, is it correctly modified to support bigints? Compiling without bigints..."
#endif

#ifdef _DISABLELIBFLINT_
    #warning "[FLINT](https://flintlib.org/) not found! Compiling without FLINT..."
#endif

#ifdef _DISABLELIBFLINTXX_
    #warning "[FLINTxx](https://github.com/flintlib/flintxx) not found! Compiling without FLINTxx..."
#endif
