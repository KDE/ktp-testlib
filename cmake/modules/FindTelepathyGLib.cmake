# Try to find the GLib binding of the Telepathy library
# TELEPATHY_GLIB_FOUND - system has telepathy-glib
# TELEPATHY_GLIB_INCLUDE_DIR - the telepathy-glib include directory
# TELEPATHY_GLIB_LIBRARIES - Link these to use telepathy-glib

# Copyright (c) 2008, Allen Winter <winter@kde.org>
# Copyright (c) 2009, Andre Moreira Magalhaes <andrunko@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

set(TELEPATHY_GLIB_FIND_REQUIRED ${TelepathyGLib_FIND_REQUIRED})
if(TELEPATHY_GLIB_INCLUDE_DIR AND TELEPATHY_GLIB_LIBRARIES)
  # Already in cache, be silent
  set(TELEPATHY_GLIB_FIND_QUIETLY TRUE)
endif(TELEPATHY_GLIB_INCLUDE_DIR AND TELEPATHY_GLIB_LIBRARIES)

find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    if (TelepathyGLib_FIND_VERSION_EXACT)
        pkg_check_modules(PC_TELEPATHY_GLIB QUIET telepathy-glib=${TelepathyGLib_FIND_VERSION})
    else (TelepathyGLib_FIND_VERSION_EXACT)
        pkg_check_modules(PC_TELEPATHY_GLIB QUIET telepathy-glib>=${TelepathyGLib_FIND_VERSION})
    endif (TelepathyGLib_FIND_VERSION_EXACT)
endif(PKG_CONFIG_FOUND)

find_path(TELEPATHY_GLIB_INCLUDE_DIR
          NAMES telepathy-glib/gtypes.h
          HINTS
          ${PC_TELEPATHY_GLIB_INCLUDEDIR}
          ${PC_TELEPATHY_GLIB_INCLUDE_DIRS}
)

find_library(TELEPATHY_GLIB_LIBRARIES
             NAMES telepathy-glib
             HINTS
             ${PC_TELEPATHY_GLIB_LIBDIR}
             ${PC_TELEPATHY_GLIB_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TELEPATHY_GLIB DEFAULT_MSG
                                  TELEPATHY_GLIB_LIBRARIES TELEPATHY_GLIB_INCLUDE_DIR)
