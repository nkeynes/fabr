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

#ifndef FABR_SUPPORT_PATH_H
#define FABR_SUPPORT_PATH_H

#include <string>

namespace fabr {

/**
 * Mutable path object (really a thin wrapper around the string).
 * Note these only deal with the 'real' filesystem, and are expected
 * to be in host-native form.
 *
 * Not thread safe.
 */
class Path {
private:
    /**
     * The underlying pathname string. Note this is mutable as some operations
     * may temporarily modify the string before restoring its original state
     * for performance reasons.
     */
    mutable std::string name;

public:
    Path() { }
    Path( std::string_view pathname );

    /**
     * @return the extension of the base filename (if any).
     * If the path has no components or the filename does not
     * have an extension, returns the empty string.
     */
    std::string_view extension() const;

    /**
     * @return the base filename (i.e. last component of the path).
     * If the path has no components (e.g. the empty path) this may
     * return the empty string.
     */
    std::string_view basename() const {
        return back();
    }
    std::string_view back() const;

    /**
     * @return the path name without the final path component (i.e. the
     * dirname). If the pathname does not have any directory components,
     * returns the empty string.
     */
    std::string_view dirname() const;

    /**
     * Append the path component to the end of the path as a new component.
     */
    void push_back( std::string_view component );

    /**
     * Return a new Path with the given component appended.
     */
    Path operator +( std::string_view component ) const {
        Path tmp(*this);
        tmp.push_back(component);
        return tmp;
    }

    /**
     * Remove the last component from the path.
     * If the path has no components, this method has no effect.
     */
    void pop_back();

    /**
     * @return true if the path is an absolute path.
     */
    bool isAbsolute() const;

    /**
     * @return true if the path has any erasable components
     * (i.e., does not consist solely of the root directory/volume).
     */
    bool hasComponents() const;

    /**
     * @return true if the path represents a root directory - either
     * "/" (Unix) or with a volume prefix e.g. "C:\\" (Windows).
     */
    bool isRoot() const;

    /**
     * @return true if the path represents the empty path.
     */
    bool isEmpty() const {
        return name.empty();
    }

    /**
     * @return the path as a string.
     */
    const std::string &str() const {
        return name;
    }

    /***************** Filesystem enquiry *****************/

    /**
     * @return true if the current path exists.
     * Note: If the path is a symlink, returns true if the target of the
     * link exists.
     */
    bool exists() const;
    /**
     * @return true if the given path exists, relative to the current path.
     *
     */
    bool exists(std::string_view filename) const;

    /**
     * @return true if the current path exists and is an ordinary file.
     * Note: If the path is a symlink, returns true if the target of the
     * link is an ordinary file.
     */
    bool isFile() const;

    /**
     * @return true if the given path exists relative to the current path,
     * and is an ordinary file.
     */
    bool isFile(std::string_view filename) const;

    /**
     * @return true if the current path exists and is a directory.
     * Note: If the path is a symlink, returns true if the target of the
     * link is an ordinary file.
     */
    bool isDirectory() const;
    /**
     * @return true if the given path exists relative to the current path,
     * and is a directory.
     */
    bool isDirectory(std::string_view filename) const;

    /**
     * Character used to separate components in pathnames.
     */
    static const int Separator;

    /**
     * Character used to separate paths in path lists.
     */
    static const int ListSeparator;

    /**
     * return a path representing the current working directory.
     */
    static Path getCurrentDir();
};


}

#endif /* !FABR_SUPPORT_PATH_H */
