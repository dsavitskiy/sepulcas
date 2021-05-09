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

#include "file_storage.h"
#include <iostream>

static void print(const cosmica::sepulca &s, size_t indent = 0)
{
    std::string si(indent * 4, ' ');
    std::cout << si << "sepulca " << s.get_id() << ": "
        << s.get_attrs().size() << " attribute(s)" << std::endl;

    for (const auto &[k, v] : s.get_attrs()) {
        std::cout << si << si << k << " = " << v << std::endl;
    }
}

static void print_all(cosmica::storage &stor)
{
    std::cout << "storage contents:" << std::endl;
    stor.enumerate([](auto s) {
        print(*s, 1);
        return true;
    });
    std::cout << std::endl;
}

static int list_storage(const std::filesystem::path &path)
{
    std::cout << "list sepulca storage " << path << std::endl;
	cosmica::file_storage stor(path);
	print_all(stor);
	return 0;
}

static int create_sepulca(const std::filesystem::path &path, int argc, char *kv[])
{
    assert(argc % 2 == 0);

    std::cout << "create sepulca in storage " << path << std::endl;
	cosmica::file_storage stor(path);

	cosmica::attributes attrs;
	for (int i = 0; i < argc; i += 2) {
	    attrs.emplace(std::make_pair(kv[0], kv[1]));
	    ++kv;
	    ++kv;
    }

	auto s = stor.create(std::move(attrs));
	print(*s);

	return 0;
}

static int erase_sepulca(const std::filesystem::path &path,
                         cosmica::sepulca_id sid)
{
    std::cout << "erase sepulca from storage " << path
        << ": '" << sid << "'" << std::endl;

    cosmica::file_storage stor(path);
    auto s = stor.get(sid);
    std::cout << "this sepulca will be erased:" << std::endl;
    print(*s);
    s->erase();

	return 0;
}

static int print_sepulca(const std::filesystem::path &path,
                         cosmica::sepulca_id sid)
{
    std::cout << "print sepulca from storage " << path
        << ": '" << sid << "'" << std::endl;

    cosmica::file_storage stor(path);
    auto s = stor.get(sid);
    print(*s);

	return 0;
}

static int check_sepulca(const std::filesystem::path &path,
                         cosmica::sepulca_id sid)
{
    std::cout << "check if sepulca '" << sid << "' exists in storage "
        << path << std::endl;

    cosmica::file_storage stor(path);
    if (stor.exists(sid)) {
        std::cout << "it exists" << std::endl;
    } else {
        std::cout << "it does not exist" << std::endl;
    }

	return 0;
}

static int test_lock()
{
    std::string pid = "[" + std::to_string(getpid()) + "] ";

    cosmica::file_lock fl("lock.txt");

    std::cout << "testing file lock: " << fl.get_lock_file_path() << std::endl;
    std::cout << pid << "locking" << std::endl;
    fl.lock();
    std::cout << pid << "sleeping 10 sec" << std::endl;
    sleep(10);
    std::cout << pid << "unlocking" << std::endl;
    fl.unlock();
    std::cout << pid << "unlocked" << std::endl;

    return 0;
}

static int usage()
{
    std::cout << "usage:\n"
        << "  lock                            test file lock\n"
        << "  list <dir>                      list sepulcas in a storage\n"
        << "  create <dir> [<key> <value>]... create a sepulca in a storage\n"
        << "  erase <dir> <id>                erase a sepulca\n"
        << "  print <dir> <id>                print a sepulca\n"
        << "  check <dir> <id>                check if a sepulca exists\n";
    return 1;
}

int main(int argc, char *argv[])
{
    try {
        if (argc < 2) {
            return usage();
        }

        argc -= 2;
        ++argv;
        const char *cmd = *argv;
        ++argv;

        if (strcmp(cmd, "lock") == 0) {
            return test_lock();
        }

        if (strcmp(cmd, "list") == 0) {
            if (argc != 1) {
                return usage();
            }
            return list_storage(argv[0]);
        }

        if (strcmp(cmd, "add") == 0) {
            if (argc < 1) {
                return usage();
            }

            const std::filesystem::path path = *argv++;
            --argc;
            if (argc % 2 != 0) {
                return usage();
            }

            return create_sepulca(path, argc, argv);
        }

        if (strcmp(cmd, "erase") == 0) {
            if (argc != 2) {
                return usage();
            }

            const std::filesystem::path path = *argv++;
            const cosmica::sepulca_id sid = *argv++;

            return erase_sepulca(path, sid);
        }

        if (strcmp(cmd, "print") == 0) {
            if (argc != 2) {
                return usage();
            }

            const std::filesystem::path path = *argv++;
            const cosmica::sepulca_id sid = *argv++;

            return print_sepulca(path, sid);
        }

        if (strcmp(cmd, "check") == 0) {
            if (argc != 2) {
                return usage();
            }

            const std::filesystem::path path = *argv++;
            const cosmica::sepulca_id sid = *argv++;

            return check_sepulca(path, sid);
        }

        return usage();
    } catch (const std::exception &err) {
        std::cerr << "Error: " << err.what() << std::endl;
    	return 2;
    }
}
