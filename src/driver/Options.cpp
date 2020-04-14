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

#include "driver/Options.h"

#include <getopt.h>
#include <unistd.h>
#include <iostream>

namespace fabr {

static const char shortOptions[] = "h";
static const struct option longOptions[] = {
    { const_cast<char *>("help"), no_argument, nullptr, 'h' },
    { nullptr, 0, nullptr, 0 }
};

void Options::printUsage() {
    std::cerr
            << "Usage: fabr [options] [targets]\n"
            << "  fabr <target-list>\n"
            << "  fabr init <source-dir>\n"
            << "  fabr run <target> [arguments]\n\n"
            << "Options:\n"
            << "  -D<property>=<value>  Set the given property.\n"
            << "  -n                    Dry-run only.\n"
            << "  -U<property>          Unset the given property.\n";
}

void Options::printHeader() {
    std::cerr << "builder ";
}

ExitCode Options::parse(int argc, char *argv[]) {
    int opt;
    while( (opt = getopt_long(argc, argv, shortOptions, longOptions, nullptr)) != -1 ) {
        switch(opt) {
        case 'h':
            printUsage();
            return ExitCode::EXITCODE_OK;

        }
    }

}

}
