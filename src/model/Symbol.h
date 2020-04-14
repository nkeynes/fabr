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

#ifndef FABR_MODEL_SYMBOL_H
#define FABR_MODEL_SYMBOL_H

#include <stdint.h>
#include <string.h>

#include <map>
#include <set>
#include <string>
#include <string_view>

namespace fabr {

/**
 * A Symbol is basically a lightweight pooled string. Symbol itself is never
 * used directly (it's just a data container), everything goes through SymbolRef.
 */
class Symbol {
protected:
    uint32_t length;
    char bytes[];

    /* If there is an existing symbol with the given string contents, return
     * that symbol. Otherwise contruct a new symbol and return it.
     */
    static Symbol *get( const char *str, size_t len );

    friend class SymbolRef;
};


class SymbolRef {
private:
    Symbol *sym;

    SymbolRef( Symbol *s ) : sym(s) { }

public:
    /**
     * Construct the empty (null) SymbolRef.
     */
    SymbolRef() : sym(nullptr) { }
    SymbolRef( const SymbolRef &ref ) : sym(ref.sym) { }

    static SymbolRef get( const char *str ) {
        return Symbol::get(str, ::strlen(str));
    }
    static SymbolRef get( const char *str, size_t len ) {
        return Symbol::get(str, len);
    }
    static SymbolRef get( const char *str, const char *end) {
        return Symbol::get(str, end-str);
    }
    static SymbolRef get( const std::string &str ) {
        return Symbol::get(str.data(), str.length());
    }
    static SymbolRef get( const std::string_view &str ) {
        return Symbol::get(str.data(), str.length());
    }

    SymbolRef &operator=(const SymbolRef &ref) {
        sym = ref.sym;
        return *this;
    }
    bool operator ==(const SymbolRef &ref) const {
        return sym == ref.sym;
    }
    bool operator !=(const SymbolRef &ref) const {
        return sym != ref.sym;
    }

    explicit operator bool() const {
        return sym != nullptr;
    }
    bool isNull() const {
        return sym != nullptr;
    }

    const char *data() const {
        return sym == nullptr ? nullptr : sym->bytes;
    }
    const uint32_t length() const {
        return sym == nullptr ? 0 : sym->length;
    }

    std::string str() {
        return std::string(sym->bytes, sym->length);
    }
    std::string_view toStringView() {
        return std::string_view(sym->bytes, sym->length);
    }

    friend struct std::less<SymbolRef>;
};

/* TODO: may want to make these use hashtables or similar in future */
template<class V>
class SymbolMap : public std::map<SymbolRef,V> {
};

typedef std::set<SymbolRef> SymbolSet;

/* Mapping of property to value */
typedef std::map<SymbolRef,SymbolRef> PropertySet;

}

namespace std {
template<>
struct less<fabr::SymbolRef> {
    constexpr bool operator()( const fabr::SymbolRef & lhs, const fabr::SymbolRef & rhs ) const {
        return lhs.sym < rhs.sym;
    }
};
}


#endif /* !FABR_MODEL_SYMBOL_H */
