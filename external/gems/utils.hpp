#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/crypto.hpp>
#include <math.h>

namespace utils {

    using eosio::asset;
    using eosio::symbol;
    using eosio::extended_symbol;
    using eosio::extended_asset;
    using eosio::name;
    using eosio::symbol_code;

    using std::string;
    using std::vector;

    struct OraclizedAsset {
        extended_asset tokens;
        double value;
        double ratioed;
    };


    /**
     * ## STATIC `asset_to_double`
     *
     * Convert asset to double
     *
     * ### params
     *
     * - `{asset} quantity` - quantity
     *
     * ### returns
     *
     * - `{double}` - amount
     *
     * ### example
     *
     * ```c++
     * const asset quantity = asset{10000, symbol{"EOS", 4}};
     * const double amount = utils::asset_to_double( quantity );
     * // => 1.0
     * ```
     */
    static double asset_to_double( const asset quantity )
    {
        if ( quantity.amount == 0 ) return 0.0;
        return quantity.amount / pow(10, quantity.symbol.precision());
    }

    /**
     * ## STATIC `double_to_asset`
     *
     * Convert double to asset
     *
     * ### params
     *
     * - `{double} amount` - amount
     * - `{symbol} symbol` - symbol
     *
     * ### returns
     *
     * - `{asset}` - token
     *
     * ### example
     *
     * ```c++
     * const double amount = 1.0;
     * const symbol sym = symbol{"EOS", 4};
     * const asset token = utils::double_to_asset( amount, sym );
     * // => "1.0000 EOS"
     * ```
     */
    static asset double_to_asset( const double amount, const symbol sym )
    {
        return asset{ static_cast<int64_t>( amount * pow(10, sym.precision())), sym };
    }

    /**
     * ## STATIC `sort_tokens`
     *
     * Returns sorted token assets, used to handle return values from pairs sorted in this order
     *
     * ### params
     *
     * - `{asset} a` - token A
     * - `{asset} b` - token B
     *
     * ### returns
     *
     * - `{pair<asset, asset>}` - sorted tokens
     *
     * ### example
     *
     * ```c++
     * // Inputs
     * const asset a = asset{10000, symbol{"USDT", 4}};
     * const asset b = asset{10000, symbol{"EOS", 4}};
     *
     * // Sort
     * const auto[ token0, token1 ] = utils::sort_tokens( a, b );
     * // token0 => "1.0000 EOS"
     * // token1 => "1.0000 USDT"
     * ```
     */
    static std::pair<asset, asset> sort_tokens( const asset a, const asset b )
    {
        eosio::check(a.symbol != b.symbol, "utils: IDENTICAL_ASSETS");
        return a.symbol < b.symbol ? std::pair<asset, asset>{a, b} : std::pair<asset, asset>{b, a};
    }

    /**
     * ## STATIC `split`
     *
     * Split string into tokens
     *
     * ### params
     *
     * - `{string} str` - string to split
     * - `{string} delim` - delimiter (ex: ",")
     *
     * ### returns
     *
     * - `{vector<string>}` - tokenized strings
     *
     * ### example
     *
     * ```c++
     * const auto[ token0, token1 ] = utils::split( "foo,bar", "," );
     * // token0 => "foo"
     * // token1 => "bar"
     * ```
     */
    static vector<string> split( const string str, const string delim )
    {
        vector<string> tokens;
        if ( str.size() == 0 ) return tokens;

        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(delim, prev);
            if (pos == string::npos) pos = str.length();
            string token = str.substr(prev, pos-prev);
            if (token.length() > 0) tokens.push_back(token);
            prev = pos + delim.length();
        }
        while (pos < str.length() && prev < str.length());
        return tokens;
    }


    /**
     * ## STATIC `parse_name`
     *
     * Parse string for account name. Return default name if invalid. Caller can check validity with name::value.
     *
     * ### params
     *
     * - `{string} str` - string to parse
     *
     * ### returns
     *
     * - `{name}` - name
     *
     * ### example
     *
     * ```c++
     * const name contract = utils::parse_name( "tethertether" );
     * // contract => "tethertether"_n
     * ```
     */
    static name parse_name(const string& str) {

        if(str.length() == 0 || str.length() > 12) return {};
        int i=0;
        for(const auto c: str) {
            if((c >= 'a' && c <= 'z') || ( c >= '0' && c <= '5') || c == '.') {
                if(i == 0 && ( c >= '0' && c <= '5') ) return {};   //can't start with a digit
                if(i == 11 && c == '.') return {};                  //can't end with a .
            }
            else return {};
            i++;
        }
        return name{str};
    }
    /**
     * ## STATIC `parse_symbol_code`
     *
     * Parse string for symbol_code. Return default symbol_code if invalid. Caller can check validity with symbol_code::is_valid().
     *
     * ### params
     *
     * - `{string} str` - string to parse
     *
     * ### returns
     *
     * - `{symbol_code}` - symbol_code
     *
     * ### example
     *
     * ```c++
     * const symbol_code symcode = utils::parse_symbol_code( "USDT" );
     * // symcode => symbol_code{"USDT"}
     * ```
     */
    static symbol_code parse_symbol_code(const string& str) {
        if (str.size() > 7) return {};
        for (const auto c: str ) {
            if ( c < 'A' || c > 'Z') return {};
        }
        const symbol_code sym_code = symbol_code{ str };

        return sym_code.is_valid() ? sym_code : symbol_code{};
    }

    /**
     * ## STATIC `parse_symbol`
     *
     * Parse string for symbol. Return default symbol if invalid. Caller can check validity with symbol::is_valid().
     *
     * ### params
     *
     * - `{string} str` - string to parse
     *
     * ### returns
     *
     * - `{symbol}` - symbol
     *
     * ### example
     *
     * ```c++
     * const symbol sym = utils::parse_symbol( "4,USDT" );
     * // sym => symbol{"USDT",4}
     * ```
     */
    static symbol parse_symbol(const string& str) {

        auto tokens = utils::split(str, ",");
        if (tokens.size()!=2) return {};

        int precision = 0;
        for (const auto c: tokens[0]){
            if(c < '0' || c > '9') return {};
            precision = precision * 10 + (c-'0');
        }
        if (precision < 0 || precision > 16) return {};

        const symbol_code symcode = parse_symbol_code(tokens[1]);
        if(!symcode.is_valid()) return {};

        return symbol{symcode, static_cast<uint8_t>(precision)};
    }

    /**
     * ## STATIC `parse_asset`
     *
     * Parse string for asset. Symbol precision is calculated based on the number of decimal digits.
     * Return default asset if invalid. Caller can check validity with asset::is_valid().
     *
     * ### params
     *
     * - `{string} str` - string to parse
     *
     * ### returns
     *
     * - `{asset}` - asset
     *
     * ### example
     *
     * ```c++
     * const asset out = utils::parse_asset( "-1.0000 USDT" );
     * // out => asset{-10000, symbol{"USDT",4}}
     * ```
     */
    static asset parse_asset(const string& str) {
        auto tokens = utils::split(str, " ");
        if(tokens.size()!=2) return {};

        auto sym_code = parse_symbol_code(tokens[1]);
        if(!sym_code.is_valid()) return {};

        int64_t amount = 0;
        uint8_t precision = 0, digits=0;
        bool seen_dot = false, neg = false;
        for (const auto c: tokens[0] ) {
            if(c=='-') {
                if(digits>0) return {};
                neg = true;
                continue;
            }
            if(c=='.') {
                if(seen_dot || digits==0) return {};
                seen_dot = true;
                continue;
            }
            if( c < '0' || c > '9') return {};
            digits++;
            if(seen_dot) precision++;
            amount = amount * 10 + (c-'0');
        }
        if(precision > 16 || (seen_dot && precision==0)) return {};

        asset out = asset {neg ? -amount : amount, symbol{sym_code, precision}};

        return out.is_valid() ? out : asset {};
    }

    /**
     * ## STATIC `parse_extended_symbol`
     *
     * Parse string for extended_symbol.
     * Return default asset if invalid. Caller can check validity with symbol::is_valid().
     *
     * ### params
     *
     * - `{string} str` - string to parse
     *
     * ### returns
     *
     * - `{extended_symbol}` - extended symbol
     *
     * ### example
     *
     * ```c++
     * const extended_symbol ext_sym = utils::parse_extended_symbol( "4,USDT@tethertether" );
     * // ext_sym => extended_symbol{symbol{"USDT",4}, "tethertether"_n}
     * ```
     */
    static extended_symbol parse_extended_symbol( const string& str)
    {
        auto ext_tokens = utils::split(str, "@");
        if ( ext_tokens.size() != 2 ) return {};

        const name contract = parse_name(ext_tokens[1]);
        const symbol sym = parse_symbol(ext_tokens[0]);

        return extended_symbol {sym, contract};
    }

    /**
     * ## STATIC `parse_extended_asset`
     *
     * Parse string for extended_asset. Symbol precision is calculated based on the number of decimal digits.
     * Return default asset if invalid. Caller can check validity with quantity::is_valid().
     *
     * ### params
     *
     * - `{string} str` - string to parse
     *
     * ### returns
     *
     * - `{extended_asset}` - extended asset
     *
     * ### example
     *
     * ```c++
     * const extended_asset ext_out = utils::parse_extended_asset( "-1.0000 USDT@tethertether" );
     * // ext_sym => extended_symbol{asset{-10000, symbol{"USDT",4}}, "tethertether"_n}
     * ```
     */
    static extended_asset parse_extended_asset( const string& str)
    {
        auto ext_tokens = utils::split(str, "@");

        if (ext_tokens.size()!=2 || ext_tokens[1].length()==0 || ext_tokens[1].length()>13)
            return {};

        const name contract = parse_name(ext_tokens[1]);
        const asset quantity = parse_asset(ext_tokens[0]);
        if(!quantity.is_valid()) return {};

        return extended_asset {quantity, contract};
    }

    /**
     * ## STATIC `get_balance`
     *
     * Get account balance without failing to avoid assert errors for accounts with unopened balance.
     * eosio.token::get_balance alternative
     *
     * ### params
     *
     * - `{extended_symbol} ext_sym` - extended symbol to query
     * - `{name} owner` - account to query balance for
     *
     * ### returns
     *
     * - `{extended_asset}` - account balance, `extended_asset{}` if no opened account
     *
     * ### example
     *
     * ```c++
     * const extended_symbol ext_sym { symbol{"USDT", 4}, "tethertether"_n };
     * const asset balance = utils::get_balance(ext_sym).quantity;
     * eosio::check(balance.is_valid(), "Balance not opened");

     * ```
     */
    static extended_asset get_balance( const extended_symbol ext_sym, const name owner )
    {
        //eosio.token accounts table - private in eosio.token contract
        struct [[eosio::table]] account {
            asset    balance;
            uint64_t primary_key()const { return balance.symbol.code().raw(); }
        };
        typedef eosio::multi_index< "accounts"_n, account > accounts_table;

        accounts_table _accounts( ext_sym.get_contract(), owner.value );
        auto it = _accounts.find( ext_sym.get_symbol().code().raw() );
        if(it == _accounts.end()) return { 0, ext_sym };
        eosio::check( ext_sym.get_symbol() == it->balance.symbol, "utils: extended symbol mismatch balance");

        return { it->balance.amount, ext_sym };
    }

    /**
     * ## STATIC `get_supply`
     *
     * Get token supply without failing to avoid assert errors for non-existing tokens.
     * eosio.token::get_supply alternative
     *
     * ### params
     *
     * - `{extended_symbol} ext_sym` - extended symbol to query
     *
     * ### returns
     *
     * - `{asset}` - token supply, `asset{}` if token doesn't exist
     *
     * ### example
     *
     * ```c++
     * const extended_symbol ext_sym { symbol{"USDT", 4}, "tethertether"_n };
     * const asset supply = utils::get_supply(ext_sym);
     * eosio::check(balance.is_valid(), "Token doesn't exist on this contract");
     * ```
     */
    static asset get_supply( const extended_symbol ext_sym)
    {
        struct [[eosio::table]] currency_stats {
            asset    supply;
            asset    max_supply;
            name     issuer;

            uint64_t primary_key()const { return supply.symbol.code().raw(); }
        };
        typedef eosio::multi_index< "stat"_n, currency_stats > stats;

        stats _stat(ext_sym.get_contract(), ext_sym.get_symbol().code().raw() );
        const auto it = _stat.begin();

        return it != _stat.end() ? it->supply : asset {};
    }

    /**
     * ## STATIC `get_hashed_nonce`
     *
     * Basic get hashed nonce
     *
     * ### params
     *
     * - `{uint64_t} nonce` - nonce
     *
     * ### returns
     *
     * - `{uint64_t}` - hashed nonce number
     *
     * ### example
     *
     * ```c++
     * const uint64_t rnd = utils::get_hashed_nonce(12345);
     * ```
     */
    static uint64_t get_hashed_nonce( uint64_t nonce ){

        const auto sha = eosio::sha256( (const char*) &nonce, sizeof( nonce )); // generate hash for uniform distribution
        const uint64_t rnd = *((uint64_t *) &sha );             // take 8 first bytes

        return rnd;
    }

};