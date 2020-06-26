//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_XTEA_FUNCTIONS_CPP_HPP
#define CRYPTO3_XTEA_FUNCTIONS_CPP_HPP

#include <nil/crypto3/detail/basic_functions.hpp>

namespace nil {
    namespace crypto3 {
        namespace block {
            namespace detail {
                template<std::size_t WordBits>
                struct xtea_functions : public ::nil::crypto3::detail::basic_functions<WordBits> {
                    typedef typename ::nil::crypto3::detail::basic_functions<WordBits>::word_type word_type;
                };
            }    // namespace detail
        }        // namespace block
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MISTY1_FUNCTIONS_CPP_HPP
