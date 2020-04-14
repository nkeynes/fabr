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

#include "support/Path.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <system_error>

namespace fabr {

#ifdef WIN32
const int Path::Separator = '\\';
#else
const int Path::Separator = '/';
#endif


static bool isUnixRoot(const std::string &str) {
    return str.size() == 1 && str[0] == Path::Separator;
}

/**
 * @return true if the string is a volume prefix (e.g. "C:")
 */
static bool isVolumePrefix(const std::string &str) {
    return str.size() == 2 && str[1] == ':' && ::isalpha(str[0]);
}

/**
 * @return true if the string is a volume root directory (e.g. "C:\\")
 */
static bool isVolumeRoot(const std::string &str) {
    return str.size() == 3 && str[2] == Path::Separator &&
           str[1] == ':' &&  ::isalpha(str[0]);
}

/**
 * @return true if the string starts with an absolute unix path (e.g. "/asdf");
 */
static bool hasUnixRoot(const std::string &str) {
    return str.size() > 0 && str[0] == Path::Separator;
}

/**
 * @return true if the string starts with a volume prefix (e.g. "C:asdf")
 */
static bool hasVolumePrefix(const std::string &str) {
    return str.size() >= 2 && str[1] == ':' && ::isalpha(str[0]);
}

/**
 * @return true if the string starts with a volume root dir (e.g. "C:\asdf")
 */
static bool hasVolumeRoot(const std::string &str) {
    return str.size() >= 3 && str[2] == Path::Separator &&
           str[1] == ':' && ::isalpha(str[0]);
}

/**
 * @return true if a separating slash needs to be added after the
 * given name, before appending a new component.
 *
 * This is true if the name doesn't already end in a slash,
 * is not a volume name (e.g. "C:"), and is not empty.
 */
static bool needSlashBeforeAppend(const std::string &name) {
    return !name.empty() &&
           name[name.size()-1] != Path::Separator &&
           !isVolumePrefix(name);
}

/**
 * Append the string str to the given target string, normalising and adjusting
 * slashes as needed.
 */
static void appendNormalisedPath( std::string &target, std::string_view str ) {
    if( str.empty() )
        return;

    target.reserve(target.size() + str.size());

    if( needSlashBeforeAppend(target) )
        target.push_back(Path::Separator);

    /* Convert all slashes to host-native slashes */
    for( int i=0; i<str.size(); i++ ) {
        if( str[i] == '/' || str[i] == '\\' ) {
            if( i != 0 || target.empty() ) {
                target.push_back(Path::Separator);
            }
        } else {
            target.push_back(str[i]);
        }
    }
    /* Remove trailing slash if present, and not required.
     * If it's a root path, we need to leave it alone.
     */
    if( target.size() > 1 && /* Leave "/" unaffected */
            target[target.size()-1] == Path::Separator &&
            !isVolumeRoot(target) ) {
        target.pop_back();
    }
}

Path::Path( std::string_view pathname ) {
    appendNormalisedPath(name, pathname);
}


std::string_view Path::back() const {
    size_t idx = name.find_last_of(Path::Separator);
    if( idx != std::string::npos ) {  /* "a/b/c" => "c", "/" => "" */
        return std::string_view(name.data() + idx + 1, name.size()-idx-1);
    } else if( hasVolumePrefix(name) ) { /* "C:foo" => "foo" */
        return std::string_view(name.data() + 2, name.size()-2);
    } else {  /* "foo" => "foo", "" => "" */
        return std::string_view(name);
    }
}

std::string_view Path::dirname() const {
    size_t idx = name.find_last_of(Path::Separator);
    if( idx != std::string::npos ) {
        if( idx == 0 || (idx == 2 && hasVolumeRoot(name)) ) {
            return std::string_view(name.data(), idx+1); /* "/foo" => "/" */
        } else {
            return std::string_view(name.data(), idx);  /* "/foo/bar => "/foo" */
        }
    } else if( hasVolumePrefix(name) ) {
        return std::string_view(name.data(), 2); /* "C:foo" => "C:" */
    } else {
        return "";   /* "foo" => "" */
    }
}

void Path::pop_back() {
    size_t idx = name.find_last_of(Path::Separator);
    if( idx != std::string::npos ) { /* a/b/c" => "a/b" */
        if( idx == 0 || (idx == 2 && hasVolumeRoot(name)) ) {
            name.erase(idx+1); /* "/foo" => "/", "c:\\bar" => "c:\\" */
        } else {
            name.erase(idx); /* "/foo/bar" => "/foo" */
        }
    } else if( hasVolumePrefix(name) ) {
        name.erase(2);  /* "c:foo" => "c:" */
    } else {
        name.erase();   /* "foo" => "" */
    }
}

void Path::push_back(std::string_view component) {
    appendNormalisedPath(name, component);
}

bool Path::hasComponents() const {
    return !name.empty() &&
           !isUnixRoot(name) &&
           !isVolumeRoot(name) &&
           !isVolumePrefix(name);
}

bool Path::isAbsolute() const {
    return hasUnixRoot(name) || hasVolumeRoot(name);
}

bool Path::isRoot() const {
    return isUnixRoot(name) || isVolumeRoot(name);
}

/**
 * Handle failure of a path syscall. This is to distinguish between
 * errors representing non-existence of a pathname from those
 * representing some actual system error.
 * @return false if errno is due to pathname non-existence.
 * @throws system_error if errno has any other error code.
 */
static bool handlePathError(int err) {
    switch(err) {
    /* Note: EACCES is debatable, but from the point of view of
     * the user of this function, we may as well treat it as !exists().
     */
    case EACCES:
    case ELOOP:
    case ENAMETOOLONG:
    case ENOENT:
    case ENOTDIR:
        return false;
    default:
        /* Otherwise we have an unexpected IO error, so go ahead
         * and throw it.
         */
        throw std::system_error(errno, std::system_category());
    }
}

bool Path::exists( ) const {
    int status = ::access(name.c_str(), F_OK);
    if( status == -1 ) {
        return handlePathError(errno);
    } else {
        return true;
    }
}

bool Path::isFile() const {
    struct stat st;
    int status = ::stat(name.c_str(), &st);
    if( status == -1 ) {
        return handlePathError(errno);
    } else {
        return (st.st_mode & S_IFREG) != 0;
    }
}

bool Path::isDirectory() const {
    struct stat st;
    int status = ::stat(name.c_str(), &st);
    if( status == -1 ) {
        return handlePathError(errno);
    } else {
        return (st.st_mode & S_IFDIR) != 0;
    }
}

bool Path::exists(std::string_view pathname) const {
    size_t idx = name.size();
    try {
        appendNormalisedPath(name, pathname);
        bool result = exists();
        name.erase(idx);
        return result;
    } catch (...) {
        name.erase(idx);
        std::rethrow_exception(std::current_exception());
    }
}

bool Path::isFile(std::string_view pathname) const {
    size_t idx = name.size();
    try {
        appendNormalisedPath(name, pathname);
        bool result = isFile();
        name.erase(idx);
        return result;
    } catch (...) {
        name.erase(idx);
        std::rethrow_exception(std::current_exception());
    }
}

bool Path::isDirectory(std::string_view pathname) const {
    size_t idx = name.size();
    try {
        appendNormalisedPath(name, pathname);
        bool result = isDirectory();
        name.erase(idx);
        return result;
    } catch (...) {
        name.erase(idx);
        std::rethrow_exception(std::current_exception());
    }
}


Path Path::getCurrentDir() {
    char buf[PATH_MAX];
    if( ::getcwd(buf, sizeof(buf)) == nullptr ) {
        throw std::system_error(errno, std::system_category());
    }
    return Path(buf);
}

}
