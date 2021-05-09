/******************************************************************************
 *
 * This file is part of the Sepulca distribution.
 *
 * Copyright (C) 2021 Dmitry Savitskiy.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ******************************************************************************/

#pragma once

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

namespace cosmica
{

/**
 * Storage-wide unique Sepulca unidetifier.
 */
using sepulca_id = std::string;

/**
 * Generator of unique (random) Sepulca IDs.
 */
class sepulca_id_generator
{
public:
    /**
     * Create a new identifier.
     */
    sepulca_id new_id() const
    {
        std::ostringstream oss;
        oss << "{";
        for (int i = 0; i < 8; ++i) {
            if (i > 0 && i % 2 == 0) {
                oss << "-";
            }
            oss << std::hex
                << std::setw(2)
                << std::setfill('0')
                << static_cast<uint32_t>(random_num());
        }
        oss << "}";
        return oss.str();
    }

private:
    static uint8_t random_num()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        return dis(gen);
    }
};

}
