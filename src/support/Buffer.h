/*
 * Copyright (c) 2020 Nathan Keynes
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FABR_SUPPORT_BUFFER_H_
#define FABR_SUPPORT_BUFFER_H_

#include <sys/types.h>
#include <string_view>
#include <memory>

namespace fabr {

/**
 * Generic byte buffer.
 */
class Buffer {
protected:
    char *ptr;
    size_t length;

    Buffer(char *ptr, size_t length) : ptr(ptr), length(length) {}

public:
    virtual ~Buffer() { }

    char *data() const {
        return ptr;
    }
    size_t size() const {
        return length;
    }
    char *end() const {
        return ptr + length;
    }

    /**
     * @return a new, uninitialised buffer of the given size.
     */
    static std::unique_ptr<Buffer> getBuffer( size_t size );

    /**
     * @return a new, zero-initialised buffer of the given size.
     */
    static std::unique_ptr<Buffer> getZeroBuffer( size_t size );
};

}

#endif /* !FABR_SUPPORT_BUFFER_H_ */
