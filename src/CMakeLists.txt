# MaNGOS is a full featured server for World of Warcraft, supporting
# the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
#
# Copyright (C) 2005-2025 MaNGOS <https://www.getmangos.eu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Build the mangos shared library
add_subdirectory(shared)

add_subdirectory(genrev)

# Needs to link against mangos_world.lib
if(BUILD_MANGOSD)
    # Build the mangos game library
    add_subdirectory(game)
endif()

if(BUILD_MANGOSD)
    # Build the mangos world server
    add_subdirectory(mangosd)

    # Build the selected modules
    add_subdirectory(modules)
endif()

# Build the mangos realm authentication server
if(BUILD_REALMD)
    add_subdirectory(realmd)
endif()

# If we want the tools for map/vmap/mmap extraction
if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if (BUILD_MANGOSD OR BUILD_REALMD)
    if(WIN32)
        get_filename_component(MYSQL_LIB_DIR ${MySQL_LIBRARIES} DIRECTORY)
        get_filename_component(MYSQL_LIB_DLL ${MySQL_LIBRARIES} NAME)
        STRING(REPLACE ".lib" ".dll" MYSQL_LIB_DLL ${MYSQL_LIB_DLL})
        install(FILES ${MYSQL_LIB_DIR}/${MYSQL_LIB_DLL} DESTINATION ${BIN_DIR})
    endif()
endif()
