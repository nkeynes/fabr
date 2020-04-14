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

#ifndef FABR_SUPPORT_FILE_H
#define FABR_SUPPORT_FILE_H

#include <system_error>
#include <string_view>
#include <memory>

namespace fabr {

class Buffer;

/**
 * Thin wrapper around a file descriptor to support unique_ptr-like
 * semantics, and map error codes to exceptions.
 */
class File {
    int fd;

public:
    /**
     * Construct a file on the given fd, taking ownership of the file
     * descriptor.
     */
    File( int fd = -1 ) : fd(fd) { }
    File( File &&f ) : fd(f.fd) {
        f.fd = -1;
    }
    File( const File &f ) = delete;

    /**
     * Destroy the File and close the underlying file descriptor.
     */
    ~File();

    bool operator==( const File &f ) const {
        return fd == f.fd;
    }
    bool operator!=( const File &f ) const {
        return fd != f.fd;
    }
    File &operator=( File &&f ) {
        fd = f.fd;
        f.fd = -1;
        return *this;
    }

    /**
     * Test if the file descriptor is valid (not -1)
     */
    bool isValid() const {
        return fd != -1;
    }
    explicit operator bool() const {
        return isValid();
    }

    /**
     * Return the file descriptor and remove it from the File.
     */
    int take() {
        int tmp = fd;
        fd = -1;
        return tmp;
    }

    /********************* File operations ********************/

    /**
     * Read (unbuffered) length bytes from the file, returning the
     * number of bytes actually read (possibly 0).
     * @throws system_error if the read fails.
     */
    size_t read( char *buf, size_t length );

    /**
     * Write (unbuffered) length bytes to the file, returning the
     * number of bytes actually written (possibly 0).
     * @throws system_error if the read fails.
     */
    size_t write( char *buf, size_t length );

    /**
     * Seek to the given offset in the file.
     * @param offset offset in bytes from the start of the file.
     * @throws system_error if the seek fails.
     */
    off_t seek( off_t offset );

    /**
     * Return the size of the file in bytes.
     * (Note this is an uncached stat)
     * @throws system_error if the operation fails.
     */
    size_t size();

    /**
     * Return a Buffer containing the contents of the file.
     * @throws system_error if the operation fails.
     */
    std::unique_ptr<Buffer> getBuffer();

    /**
     * Convenience function to get the file buffer from a filename.
     * @throws system_error if the operation fails.
     */
    static std::unique_ptr<Buffer> getBuffer(const char *filename) {
        return getForRead(filename).getBuffer();
    }
    static std::unique_ptr<Buffer> getBuffer(const std::string &filename) {
        return getForRead(filename).getBuffer();
    }
    static std::unique_ptr<Buffer> getBuffer(std::string_view filename) {
        return getForRead(filename).getBuffer();
    }

    /********************** File open ***********************/

    /**
     * Open an existing file for reading.
     * @throws system_error if the operation fails.
     */
    static File getForRead(const char *filename);
    static File getForRead(const std::string &filename) {
        return getForRead(filename.c_str());
    }
    static File getForRead(std::string_view filename) {
        return getForRead(std::string(filename).c_str());
    }

    /**
     * Create a new file with the given filename for writing, with mode 0666. If the
     * file already exists, the call will fail.
     * @throws system_error if the operation fails.
     */
    static File create(const char *filename);
    static File create(const std::string &filename) {
        return create(filename.c_str());
    }
    static File create(std::string_view filename) {
        return create(std::string(filename).c_str());
    }
};


}

#endif /* !FABR_SUPPORT_FILE_H */
