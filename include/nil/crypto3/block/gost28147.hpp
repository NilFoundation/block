//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_GOST_28147_89_H_
#define CRYPTO3_GOST_28147_89_H_

#include <nil/crypto3/block/detail/gost_28147_89/gost_28147_89_policy.hpp>
#include <nil/crypto3/block/detail/gost_28147_89/gost_28147_89_parameters.hpp>

#include <nil/crypto3/block/detail/stream_endian.hpp>
#include <nil/crypto3/block/detail/block_state_preprocessor.hpp>

namespace nil {
    namespace crypto3 {
        namespace block {
            struct crypto_pro_params : public detail::gost_28147_89_parameters {
            private:
                struct col_selector {
                    col_selector(std::size_t row) : row(row) {

                    }

                    byte_type operator[](std::size_t col) const {
                        const byte_type x = parameters[4 * (col % 16) + row];
                        const byte_type y = parameters[4 * (col / 16) + row];
                        return (x >> 4) | (y << 4);
                    }

                private:
                    std::size_t row;
                };

            public:
                col_selector operator[](std::size_t row) const {
                    return {row};
                }

                // GostR3411-94-CryptoProParamSet (OID 1.2.643.2.2.31.1)
                constexpr static const parameters_type parameters = {
                        0xA5, 0x74, 0x77, 0xD1, 0x4F, 0xFA, 0x66, 0xE3, 0x54, 0xC7, 0x42, 0x4A, 0x60, 0xEC, 0xB4, 0x19,
                        0x82, 0x90, 0x9D, 0x75, 0x1D, 0x4F, 0xC9, 0x0B, 0x3B, 0x12, 0x2F, 0x54, 0x79, 0x08, 0xA0, 0xAF,
                        0xD1, 0x3E, 0x1A, 0x38, 0xC7, 0xB1, 0x81, 0xC6, 0xE6, 0x56, 0x05, 0x87, 0x03, 0x25, 0xEB, 0xFE,
                        0x9C, 0x6D, 0xF8, 0x6D, 0x2E, 0xAB, 0xDE, 0x20, 0xBA, 0x89, 0x3C, 0x92, 0xF8, 0xD3, 0x53, 0xBC
                };
            };

            /**
             * Default GOST parameters are the ones given in GOST R 34.11 for
             * testing purposes; these sboxes are also used by Crypto++, and,
             * at least according to Wikipedia, the Central Bank of Russian
             * Federation
             */
            struct cbr_params : public detail::gost_28147_89_parameters {
            private:
                struct col_selector {
                    col_selector(std::size_t row) : row(row) {

                    }

                    byte_type operator[](std::size_t col) const {
                        const byte_type x = parameters[4 * (col % 16) + row];
                        const byte_type y = parameters[4 * (col / 16) + row];
                        return (x >> 4) | (y << 4);
                    }

                private:
                    std::size_t row;
                };

            public:
                col_selector operator[](std::size_t row) const {
                    return {row};
                }

                // GostR3411_94_TestParamSet (OID 1.2.643.2.2.31.0)
                constexpr static const parameters_type parameters = {
                        0x4E, 0x57, 0x64, 0xD1, 0xAB, 0x8D, 0xCB, 0xBF, 0x94, 0x1A, 0x7A, 0x4D, 0x2C, 0xD1, 0x10, 0x10,
                        0xD6, 0xA0, 0x57, 0x35, 0x8D, 0x38, 0xF2, 0xF7, 0x0F, 0x49, 0xD1, 0x5A, 0xEA, 0x2F, 0x8D, 0x94,
                        0x62, 0xEE, 0x43, 0x09, 0xB3, 0xF4, 0xA6, 0xA2, 0x18, 0xC6, 0x98, 0xE3, 0xC1, 0x7C, 0xE5, 0x7E,
                        0x70, 0x6B, 0x09, 0x66, 0xF7, 0x02, 0x3C, 0x8B, 0x55, 0x95, 0xBF, 0x28, 0x39, 0xB3, 0x2E, 0xCC
                };
            };

            /*!
             * @brief GOST-28147-89 (Magma). 64-bit Russian cipher. Possible
             * security issues. Avoid unless compatibility is needed.
             *
             * @tparam ParamsType GOST 28147-89 block cipher uses a set of 4 bit Sboxes, however
             * the standard does not actually define these Sboxes; they are considered
             * a local configuration issue. Several different sets are used.
             *
             * @ingroup block
             */
            template<typename ParamsType = cbr_params>
            class gost_28147_89 {
            protected:
                typedef detail::gost_28147_89_policy<ParamsType> policy_type;

                typedef ParamsType parameters_type;

                constexpr static const std::size_t key_schedule_size = policy_type::key_schedule_size;
                typedef typename policy_type::key_schedule_type key_schedule_type;

                constexpr static const std::size_t expanded_substitution_size = policy_type::expanded_substitution_size;
                typedef typename policy_type::expanded_substitution_type expanded_substitution_type;

            public:

                constexpr static const std::size_t word_bits = policy_type::word_bits;
                typedef typename policy_type::word_type word_type;

                constexpr static const std::size_t block_bits = policy_type::block_bits;
                constexpr static const std::size_t block_words = policy_type::block_words;
                typedef typename policy_type::block_type block_type;

                constexpr static const std::size_t key_bits = policy_type::key_bits;
                constexpr static const std::size_t key_words = policy_type::key_words;
                typedef typename policy_type::key_type key_type;

                template<template<typename, typename> class Mode,
                                                      typename StateAccumulator, std::size_t ValueBits,
                                                      typename Padding>
                struct stream_cipher {
                    typedef block_state_preprocessor<Mode<gost_28147_89<ParamsType>, Padding>, StateAccumulator,
                                                     stream_endian::little_octet_big_bit, ValueBits,
                                                     policy_type::word_bits * 2> type_;
#ifdef CRYPTO3_HASH_NO_HIDE_INTERNAL_TYPES
                    typedef type_ type;
#else
                    struct type : type_ {
                    };
#endif
                };

                gost_28147_89(const key_type &key, const expanded_substitution_type &exp = expanded_substitution_type())
                        : expanded_substitution(exp) {
                    for (size_t i = 0; i != 256; ++i) {
                        expanded_substitution[i] = policy_type::template rotl<11, uint32_t>(params[0][i]);
                        expanded_substitution[i + 256] = policy_type::template rotl<19, uint32_t>(params[1][i]);
                        expanded_substitution[i + 512] = policy_type::template rotl<27, uint32_t>(params[2][i]);
                        expanded_substitution[i + 768] = policy_type::template rotl<3, uint32_t>(params[3][i]);
                    }

                    schedule_key(key);
                }

                virtual ~gost_28147_89() {
                    key_schedule.fill(0);
                }

                block_type encrypt(const block_type &plaintext) {
                    return encrypt_block(plaintext);
                }

                block_type decrypt(const block_type &ciphertext) {
                    return decrypt_block(ciphertext);
                }

            protected:
                parameters_type params;
                expanded_substitution_type expanded_substitution;
                key_schedule_type key_schedule;

                inline void schedule_key(const key_type &key) {
                    for (size_t i = 0; i != key_schedule_size; ++i) {
                        key_schedule[i] = load_le<uint32_t>(key, i);
                    }
                }

                inline block_type encrypt_block(const block_type &plaintext) {
                    block_type out = {0};

                    word_type N1 = load_le<uint32_t>(plaintext.data(), 0);
                    word_type N2 = load_le<uint32_t>(plaintext.data(), 1);

                    for (size_t j = 0; j != 3; ++j) {
                        GOST_2ROUND(N1, N2, 0, 1);
                        GOST_2ROUND(N1, N2, 2, 3);
                        GOST_2ROUND(N1, N2, 4, 5);
                        GOST_2ROUND(N1, N2, 6, 7);
                    }

                    GOST_2ROUND(N1, N2, 7, 6);
                    GOST_2ROUND(N1, N2, 5, 4);
                    GOST_2ROUND(N1, N2, 3, 2);
                    GOST_2ROUND(N1, N2, 1, 0);

                    store_le(out.data(), N2, N1);

                    return out;
                }

                inline block_type decrypt_block(const block_type &ciphertext) {
                    block_type out = {0};

                    word_type N1 = load_le<uint32_t>(ciphertext.data(), 0);
                    word_type N2 = load_le<uint32_t>(ciphertext.data(), 1);

                    GOST_2ROUND(N1, N2, 0, 1);
                    GOST_2ROUND(N1, N2, 2, 3);
                    GOST_2ROUND(N1, N2, 4, 5);
                    GOST_2ROUND(N1, N2, 6, 7);

                    for (size_t j = 0; j != 3; ++j) {
                        GOST_2ROUND(N1, N2, 7, 6);
                        GOST_2ROUND(N1, N2, 5, 4);
                        GOST_2ROUND(N1, N2, 3, 2);
                        GOST_2ROUND(N1, N2, 1, 0);
                    }

                    store_le(out.data(), N2, N1);

                    return out;
                }
            };
        }
    }
}
#endif