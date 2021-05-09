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

#include <mutex>
#include <filesystem>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

namespace cosmica
{

/**
 * A system-wide mutex implemented via file locking mechanism.
 * This class is compatible with std::lock_guard.
 */
class file_lock
{
public:
    /**
     * Creates a file lock object for the given file.
     */
    explicit file_lock(std::filesystem::path path) :
        m_path(std::move(path))
    {
        m_fd = open(m_path.c_str(), O_CREAT | O_RDONLY, 0777);
        if (m_fd == -1) {
            throw std::runtime_error("Failed to open lock file '" +
                m_path.string() + "': " + strerror(errno));
        }
    }

    /**
     * Releaes the lock and destroys the lock object.
     */
    ~file_lock()
    {
        std::lock_guard lock(m_mutex);

        if (m_fd != -1) {
            close(m_fd);
            m_fd = -1;
        }
    }

    /**
     * Acquires the file lock.
     * This function does nothing if the lock already acquired.
     */
    void lock()
    {
        std::lock_guard lock(m_mutex);

        if (m_fd == -1) {
            return;
        }

        if (flock(m_fd, LOCK_EX) != 0) {
            throw std::runtime_error("Failed to lock file '" +
                m_path.string() + "': " + strerror(errno));
        }
    }

    /**
     * Releases the file lock.
     */
    void unlock()
    {
        std::lock_guard lock(m_mutex);

        if (m_fd == -1) {
            return;
        }

        if (flock(m_fd, LOCK_UN) != 0) {
            throw std::runtime_error("Failed to unlock file '" +
                m_path.string() + "': " + strerror(errno));
        }
    }

    /**
     * Returns lock's file path.
     */
    const auto &get_lock_file_path() const {
        return m_path;
    }

private:
    std::filesystem::path m_path;
    std::mutex m_mutex;
    int m_fd = -1;
};

}
