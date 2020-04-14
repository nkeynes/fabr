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

#ifndef FABR_DRIVER_OPTIONS_H
#define FABR_DRIVER_OPTIONS_H

#include <string>
#include <vector>

#include "driver/ExitCode.h"

namespace fabr {

/**
 * Manages command-line options for the build tool.
 */
class Options {
private:
    std::vector<std::string> targets;

    std::string sourceRoot;
    std::string buildRoot;

    void printHeader();
    void printUsage();

public:
    Options() { }

    /**
     * Parse command-line options.
     */
    ExitCode parse( int argc, char *argv[] );

    const std::string &getSourceRoot() const {
        return sourceRoot;
    }
    const std::string &getBuildRoot() const {
        return buildRoot;
    }

    const std::vector<std::string> &getTargets() const {
        return targets;
    }
};

}
#endif /* FABR_DRIVER_OPTIONS_H */
