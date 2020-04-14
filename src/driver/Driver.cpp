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

#include "driver/Constants.h"
#include "driver/Driver.h"
#include "driver/Options.h"

#include "model/BuildModel.h"

#include "support/Path.h"

namespace fabr {

bool findRoots( BuildModel &model ) {
    /* Walk up the directory tree from the current directory */
    Path path = Path::getCurrentDir();
    Path topSourceDir;
    do {
        if( path.isFile(BUILD_CACHEDMODEL) ) {
            /* Done, load the serialized model */
            model.load(path + BUILD_CACHEDMODEL);
            return true;
        }
        if( path.isFile(BUILD_FILENAME) ) {
            topSourceDir = path;
        }

    } while(!path.isRoot());
}


ExitCode Driver::run(int argc, char *argv[]) {
    Options options;

    ExitCode status = options.parse(argc, argv);
    if( status != ExitCode::EXITCODE_OK ) {
        return status;
    }

    /* Locate the top of the source and build trees, and initialize the
     * build model. There's a few cases:
     *   a) we're in an existing build directory - just import the cached model
     *   b) we're in a non-build directory and user supplied the path to
     *      source root - start a new build model with the source path.
     *   c) we're in the source tree - locate top of tree, and setup a new
     *      build model for an in-tree build.
     */
    BuildModel model;

    /* Check all build script files for up-to-date ness, and refresh the model
     * with any that are new or modified. Note we have to check everything even
     * in a limited build because we allow non-local changes to rules.
     */
    model.ensureUpToDate();

    /* Generate the build queue from the requested targets.
     * If targets are contradictory, the result will be as-if
     * they had been processed sequentially (e.g. all,clean is
     * equivalent to clean).
     */

    /* Process the queue */


    /* Run any post-build actions */
}

}
