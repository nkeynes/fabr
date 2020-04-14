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

/**
 * Special targets: these are predefined targets that can't be handled within
 * normal build rules. They're available at all levels and apply to the current subtree.
 *
 * verify: Checks that every target defined in the system is buildable.
 *  (all recursively required dependencies are available, and the build does not contain cycles)
 *  This implicitly requires executing any targets needed to make this determination.
 * clean: Delete all built files
 * pristine: clean and also remove any cached files used by the build itself (after execution the
 *  build tree will need to be reinitialised)
 *
 * Other default targets are managed by the default rule set.
 */

#ifndef FABR_MODEL_BUILDMODEL_H
#define FABR_MODEL_BUILDMODEL_H

#include <system_error>
#include <string_view>

namespace fabr {

class BuildQueue;
class Path;

class BuildModel {
private:
    /** provides the set of known tags (and their valid combinations) */
    // TagDictionary tags;
    /** provides the set of known build rules */
    // RulesDictionary rules;
    /** provides the set of actual targets */
    // TargetDictionary targets;

public:
    /************* Initialization and parsing *************/
    BuildModel();

    /**
     * Parse in a single build script file
     */
    std::error_code parseBuild(std::string_view file);

    /**
     * Set the given named property. If hard, the property is
     * treated as an underivable input regardless of the model
     * (i.e. its forced by the user).
     */
    void setProperty( std::string_view name, std::string_view value, bool hard=true );

    /**
     * Clear the given property, if previously set.
     * If hard, this will clear a hard set property, otherwise
     * it will only clear a non-hard property.
     */
    void clearProperty( std::string_view name, bool hard=true );

    /**
     * After parsing, resolve all symbolic references (parsing is unordered, so this
     * has to be deferred until after all files have definitely been read. Raise an
     * error if anything can't be resolved.
     * @return true on success, otherwise false.
     */
    bool resolve();

    /**
     * Check the model itself for up-to-dateness, and (re)parse and resolve
     * any new or modified scripts.
     * @return error code if any error occurs.
     */
    void ensureUpToDate();

    /******************** Operation ***********************/

    /**
     * Add the given target to the build queue, along with any recursively
     * required targets. If the target is up to date, adds nothing.
     * Note: if any configuration is required before executing the
     * target, it is added to the queue as well, but is not executed. It is
     * possible for queue execution to fail after configuration as the target
     * is no longer buildable.
     * @return true if the target is at least conditionally buildable, otherwise false.
     */
    bool queueTarget( BuildQueue &queue, std::string_view target );

    /*************** Model cache handling *****************/

    /**
     * Load the model from the given file. Note this expects a binary
     * serialization of the model, not the original scripts.
     */
    void load(const Path &file);

    /**
     * Save the model out to the given file in binary format.
     */
    void save(const Path &file) const;
    /**
     * Save the model back out to the file it was originally
     * read from.
     */
    void save() const;
    /**
     * @return true if the model has changed in any way that would
     * require it to be re-saved.
     */
    bool dirty() const;
};

}

#endif /* !FABR_MODEL_BUILDMODEL_H */
