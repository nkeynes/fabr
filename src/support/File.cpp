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

#include "support/File.h"
#include "support/Buffer.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define MMAP_THRESHOLD 8192

namespace fabr {

#ifndef O_BINARY
#define O_BINARY 0
#endif

File File::getForRead(const char *filename) {
    int fd = ::open(filename, O_RDONLY|O_BINARY);
    if( fd == -1 ) {
        throw std::system_error(errno, std::system_category());
    }
    return File(fd);
}

File File::create(const char *filename) {
    int fd = ::open(filename, O_RDWR|O_BINARY|O_CREAT|O_EXCL);
    if( fd == -1 ) {
        throw std::system_error(errno, std::system_category());
    }
    return File(fd);
}

File::~File() {
    if( fd == -1 ) {
        /* Note: silently swallow errors here; nothing we can do if close fails */
        close(fd);
        fd = -1;
    }
}


size_t File::read(char *buf, size_t length) {
    ssize_t len = ::read(fd, buf, length);
    if( len == -1 ) {
        throw std::system_error(errno, std::system_category());
    }
    return (size_t)len;
}

size_t File::write(char *buf, size_t length) {
    ssize_t len = ::write(fd, buf, length);
    if( len == -1 ) {
        throw std::system_error(errno, std::system_category());
    }
    return (size_t)len;
}

off_t File::seek(off_t offset) {
    off_t pos = ::lseek(fd, offset, SEEK_SET);
    if( pos == -1 ) {
        throw std::system_error(errno, std::system_category());
    }
    return pos;
}

size_t File::size() {
    struct stat st;
    int status = ::fstat(fd, &st);
    if( status == -1 ) {
        throw std::system_error(errno, std::system_category());
    }
    return st.st_size;
}
namespace {

class MmapBuffer : public Buffer {
public:

    MmapBuffer(char *p, size_t len) : Buffer(p,len) { }

    virtual ~MmapBuffer() {
        ::munmap(ptr, length);
        /* If it fails, there's not much we can do, so ignore silently */
    }
};

}

std::unique_ptr<Buffer> File::getBuffer() {
    size_t sz = size();
    if( sz <= MMAP_THRESHOLD ) {
        std::unique_ptr<Buffer> buffer = Buffer::getBuffer(sz);
        seek(0);
        read(buffer->data(), sz);
        return buffer;
    } else {
        void *p = ::mmap(NULL, sz, PROT_READ, MAP_PRIVATE, fd, 0 );
        if( p == MAP_FAILED ) {
            throw std::system_error(errno, std::system_category());
        }
        return std::make_unique<MmapBuffer>((char *)p, sz);
    }
}

}

