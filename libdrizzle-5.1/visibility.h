/* libdrizzle
 * Copyright (C) 2013 Drizzle Developers Group
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 *
 */

#pragma once

/**
 *
 * DRIZZLE_API is used for the public API symbols. It either DLL imports or
 * DLL exports (or does nothing for static build).
 *
 */

#if defined(BUILDING_LIBDRIZZLE)
# if defined(HAVE_VISIBILITY) && HAVE_VISIBILITY
#  define DRIZZLE_API __attribute__ ((visibility("default")))
# elif defined (__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#  define DRIZZLE_API __global
# elif defined(_MSC_VER)
#  define DRIZZLE_API extern __declspec(dllexport) 
# else
#  define DRIZZLE_API
# endif /* defined(HAVE_VISIBILITY) */
#else  /* defined(BUILDING_LIBDRIZZLE) */
# if defined(_MSC_VER)
#  define DRIZZLE_API extern __declspec(dllimport) 
# else
#  define DRIZZLE_API
# endif /* defined(_MSC_VER) */
#endif /* defined(BUILDING_LIBDRIZZLE) */
