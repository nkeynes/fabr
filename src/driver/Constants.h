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

#ifndef FABR_DRIVER_CONSTANTS_H
#define FABR_DRIVER_CONSTANTS_H

#define PACKAGE_NAME "fabr"

#define PACKAGE_VERSION "0.1"

/**
 * Build source filename.
 */
#define BUILD_FILENAME "BUILD"

/**
 * Additional user properties, read from source root).
 */
#define BUILD_USERFILE "build.properties"

/**
 * Directory to hold the build cache (under the build root)
 */
#define BUILD_CACHEDIR ".build"

/**
 * Main model image file under the build root)
 */
#define BUILD_CACHEDMODEL ".build/model"

#endif /* !FABR_DRIVER_CONSTANTS_H */
