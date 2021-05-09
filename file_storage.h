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

#include "sepulca.h"
#include "file_lock.h"
#include <fstream>
#include <iostream>
#include <filesystem>

#define SEPULCA_SIG "Sepulca v1"

namespace cosmica
{

class file_storage : public storage
{
public:
    /**
     * Opens sepulca file storage for the given path.
     */
    explicit file_storage(const std::filesystem::path &dir) :
        m_dir(dir)
    {
        auto s = std::filesystem::status(m_dir);
        if (s.type() == std::filesystem::file_type::not_found) {
            std::cout << "File storage '" << dir.string()
                << "' not found, creating new."
                << std::endl;

            std::filesystem::create_directories(m_dir);
        } else if (s.type() != std::filesystem::file_type::directory) {
            throw std::runtime_error("File storage '" + dir.string() +
                "' not a directory");
        }

        m_lock = std::make_unique<file_lock>(dir / "lock.txt");
    }

    virtual ~file_storage() override = default;

    virtual sepulca_ptr create(attributes attrs = {}) override
    {
        std::lock_guard guard(*m_lock);

        sepulca_id_generator gen;
        sepulca_id sid;
        do {
            sid = gen.new_id();
        } while (do_check_exists(sid));

        auto s = new_sepulca(std::move(sid), std::move(attrs));
        do_serialize(*s);
        return s;
    }

    virtual sepulca_ptr get(const sepulca_id &sid) const override
    {
        std::lock_guard guard(*m_lock);

        auto s = do_deserialize(get_cell_path(sid));
        if (!s) {
            throw std::runtime_error("Sepulca '" + sid + "' not found");
        }
        return s;
    }

    virtual bool exists(const sepulca_id &sid) const override
    {
        std::lock_guard guard(*m_lock);

        return do_check_exists(sid);
    }

    virtual void enumerate(std::function<bool(sepulca_ptr)> cb) const override
    {
        std::lock_guard guard(*m_lock);

        for(const auto &dir_ent : std::filesystem::directory_iterator(m_dir)) {
            if (dir_ent.path() == m_lock->get_lock_file_path()) {
                continue;
            }

            auto s = do_deserialize(dir_ent.path());
            if (s) {
                if (!cb(std::move(s))) {
                    break;
                }
            }
        }
    }

protected:
    virtual void erase(sepulca &s) override
    {
        std::lock_guard guard(*m_lock);

        if (!do_check_exists(s.get_id())) {
            throw std::runtime_error("Sepulca '" + s.get_id() +
                "' has been already destroyed");
        }

        std::filesystem::remove(get_cell_path(s));
    }

    virtual void commit(sepulca &s) override
    {
        std::lock_guard guard(*m_lock);

        do_serialize(s);
    }

private:
    bool do_check_exists(const sepulca_id &sid) const {
        return std::filesystem::is_regular_file(get_cell_path(sid));
    }

    void do_serialize(sepulca &s)
    {
        std::ofstream ofs(get_cell_path(s), std::ofstream::trunc);

        ofs << SEPULCA_SIG << "\n" << s.get_id() << "\n";
        for (const auto &[k, v] : s.get_attrs()) {
            ofs << k << "\n" << v << "\n";
        }
    }

    sepulca_ptr do_deserialize(const std::filesystem::path &p) const
    {
        std::ifstream ifs(p);

        std::string sig, sid;
        std::getline(ifs, sig);
        if (sig != SEPULCA_SIG) {
            std::cerr << "Invalid Sepulca file signature: " << p << std::endl;
            return {};
        }

        std::getline(ifs, sid);
        if (sid.empty()) {
            std::cerr << "Invalid Sepulca identifier in file: " << p
                << std::endl;
            return {};
        }

        attributes attrs;
        while (ifs) {
            std::string k, v;
            std::getline(ifs, k);
            if (!k.empty()) {
                std::getline(ifs, v);
                attrs.emplace(std::pair(std::move(k), std::move(v)));
            } else {
                break;
            }
        }

        return new_sepulca(std::move(sid), std::move(attrs));
    }

    std::filesystem::path get_cell_path(const sepulca_id &sid) const {
        return (m_dir / sid).replace_extension("txt");
    }

    std::filesystem::path get_cell_path(const sepulca &s) const {
        return get_cell_path(s.get_id());
    }

    sepulca_ptr new_sepulca(sepulca_id &&sid, attributes &&attrs) const
    {
        return sepulca_ptr(new sepulca(const_cast<file_storage&>(*this),
                                       std::move(sid),
                                       std::move(attrs)));
    }

    const std::filesystem::path m_dir;
    mutable std::unique_ptr<file_lock> m_lock;
};

}
