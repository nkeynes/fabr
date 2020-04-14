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

#include "support/Buffer.h"

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

namespace fabr {

std::unique_ptr<Buffer> Buffer::getBuffer(size_t size) {
    /* Allocate the Buffer object and the actual data in one chunk */
    size_t total = sizeof(Buffer) + size;
    void *data = operator new(total);
    std::unique_ptr<Buffer>(new (data) Buffer(((char *)data)+sizeof(Buffer), size));
}

std::unique_ptr<Buffer> Buffer::getZeroBuffer(size_t size) {
    std::unique_ptr<Buffer> buf = getBuffer(size);
    memset(buf->data(), 0, buf->length);
    return buf;
}

}
