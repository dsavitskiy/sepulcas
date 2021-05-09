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

#include "sepulca_id.h"
#include <memory>
#include <map>

namespace cosmica
{

class sepulca;
using sepulca_ptr = std::unique_ptr<sepulca>;

/**
 * Sepulca attributes.
 */
using attributes = std::map<std::string, std::string>;

/**
 * Sepulca storage abstract class.
 */
class storage
{
public:
    storage() = default;
    virtual ~storage() = default;

    // Disable copy and move of storage objects.
    storage(const storage &) = delete;
    storage(storage &&) = delete;

    /**
     * Creates a sepulca with an optional list of initial attributes,
     * assigning a new unique identifier.
     * The new sepulca is committed to the storage immediately after creation.
     */
    virtual sepulca_ptr create(attributes attrs = {}) = 0;

    /**
     * Loads a sepulca with a given identifier from the storage.
     */
    virtual sepulca_ptr get(const sepulca_id &sid) const = 0;

    /**
     * Checks if a sepulca with a given identifier exists in the storage.
     */
    virtual bool exists(const sepulca_id &sid) const = 0;

    /**
     * Enumerate all sepulcas.
     */
    virtual void enumerate(std::function<bool(sepulca_ptr)> cb) const = 0;

protected:
    friend class sepulca;

    /**
     * Erase a sepulca.
     */
    virtual void erase(sepulca &s) = 0;

    /**
     * Commit a sepulca.
     */
    virtual void commit(sepulca &s) = 0;
};

}
