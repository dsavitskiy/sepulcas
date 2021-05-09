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

#include "storage.h"
#include <any>

namespace cosmica
{

class sepulca;

/**
 * Sepulca object.
 */
class sepulca
{
public:
    // Disable copy and move of sepulca objects.
    sepulca(const sepulca &) = delete;
    sepulca(sepulca &&) = delete;

    /**
     * Returns sepulca's unique ID.
     */
    const sepulca_id &get_id() const {
        return m_sid;
    }

    /**
     * Erases the sepulca from the associated storage.
     * This sepulca object continues to be valid and can be committed
     * to the storage again.
     */
    void erase() {
        m_stor.erase(*this);
    }

    /**
     * Commits the current sepulca state to the associated storage.
     */
    void commit() {
        m_stor.commit(*this);
    }

    /**
     * Checks if the sepulca has the given attribute.
     */
    bool has_attr(const std::string &name) const noexcept {
        return m_attrs.contains(name);
    }

    /**
     * Returns a value of a given attribute.
     * Throws an expection if the attribute is not found.
     */
    std::string get_attr(const std::string &name) const
    {
        if (auto i = m_attrs.find(name); i != m_attrs.end()) {
            return i->second;
        } else {
            throw std::runtime_error("Attribute '" + name + "' not found");
        }
    }

    /**
     * Sets a value for a given attribute.
     *
     * If the attribute does not exist, creates a new one.
     *
     * Any changes to sepulca attributes are written to the associated
     * storage only after commit() call.
     */
    void set_attr(const std::string &name, const std::string &value)
    {
        m_attrs[name] = value;
    }

    /**
     * Deletes a given attribute.
     *
     * Throws an expection if the attribute is not found.
     *
     * Any changes to sepulca attributes are written to the associated
     * storage only after commit() call.
     */
    void delete_attr(const std::string &name)
    {
        if (auto i = m_attrs.find(name); i != m_attrs.end()) {
            m_attrs.erase(i);
        } else {
            throw std::runtime_error("Attribute '" + name + "' not found");
        }
    }

    /**
     * Returns a container of all attributes.
     */
    const attributes &get_attrs() const noexcept {
        return m_attrs;
    }

    /**
     * Returns client data associate with this sepulca object.
     * This data is not written to the associated storage.
     */
    std::any get_transient_data() const {
        return m_transient_data;
    }

    /**
     * Sets a client-defined data for this sepulca object.
     * This function returns previously storage data.
     * This data is not written to the associated storage.
     */
    std::any set_transient_data(std::any data) {
        return std::exchange(m_transient_data, std::move(data));
    }

private:
    // Every sepulca storage class must be declared friend here
    // in order to be able create sepulcas.
    friend class file_storage;

    /**
     * Sepulca object costructor.
     * Clients can create sepulca objects only by storage interface.
     */
    sepulca(storage &stor, sepulca_id &&sid, attributes &&attrs) :
        m_stor(stor),
        m_sid(sid),
        m_attrs(attrs)
    {
    }

    storage &m_stor;
    const sepulca_id m_sid;
    attributes m_attrs;
    std::any m_transient_data;
};

}
