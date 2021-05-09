#
# This file is part of the Sepulca distribution.
#
# Copyright (C) 2021 Dmitry Savitskiy.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.17)
project(sepulcas)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
        -std=c++20
        -Wall
        -Wextra
        -Werror
        -Wpedantic
#        -Wno-unused-parameter
#        -Wno-unused-function
    )

set(SOURCE_FILES
        file_storage.h
        file_lock.h
        sepulca_id.h
        sepulca.h
        storage.h
        main.cpp
        )

add_executable(sepulcas ${SOURCE_FILES})
