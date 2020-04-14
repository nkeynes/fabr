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

#ifndef FABR_DRIVER_EXITCODE_H
#define FABR_DRIVER_EXITCODE_H

namespace fabr {

/**
 * Definition for possible exit codes from the builder.
 */
enum class ExitCode {
    EXITCODE_OK = 0,      /* Success */
    EXITCODE_USER = 1,    /* Command-line option error */
    EXITCODE_NOBUILD = 2, /* No build files found */
    EXITCODE_BADBUILD = 3,/* Invalid build files */
    EXITCODE_NOTARGET = 4,/* Requested target not in build */
};

}

#endif /* !FABR_DRIVER_EXITCODE_H */
