//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_XTEA_H_
#define CRYPTO3_XTEA_H_

#include <nil/crypto3/block/detail/xtea/xtea_policy.hpp>

#include <nil/crypto3/block/detail/block_state_preprocessor.hpp>
#include <nil/crypto3/block/detail/stream_endian.hpp>

namespace nil {
    namespace crypto3 {
        namespace block {
            /*!
             * @brief Xtea. A 64-bit cipher popular for its simple implementation.
             * Avoid in new code.
             *
             * @ingroup block
             */
            class xtea {
            protected:
                typedef detail::xtea_policy policy_type;

                constexpr static const std::size_t key_schedule_size = policy_type::key_schedule_size;
                typedef typename policy_type::key_schedule_type key_schedule_type;

            public:

                constexpr static const std::size_t rounds = policy_type::rounds;

                constexpr static const std::size_t word_bits = policy_type::word_bits;
                typedef typename policy_type::word_type word_type;

                constexpr static const std::size_t block_bits = policy_type::block_bits;
                constexpr static const std::size_t block_words = policy_type::block_words;
                typedef typename policy_type::block_type block_type;

                constexpr static const std::size_t key_bits = policy_type::key_bits;
                constexpr static const std::size_t key_words = policy_type::key_words;
                typedef typename policy_type::key_type key_type;

                template<template<typename, typename> class Mode, std::size_t ValueBits, typename Padding>
                struct stream_cipher {
                    typedef block_state_preprocessor<Mode<xtea, Padding>, stream_endian::little_octet_big_bit,
                                                     ValueBits, policy_type::word_bits * 2> type;
                };

                xtea(const key_type &key) {
                    schedule_key(key);
                }

                ~xtea() {
                    key_schedule.fill(0);
                }

                block_type encrypt(const block_type &plaintext) {
                    return encrypt_block(plaintext);
                }

                block_type decrypt(const block_type &ciphertext) {
                    return decrypt_block(ciphertext);
                }

            protected:
                key_schedule_type key_schedule;

                inline block_type encrypt_block(const block_type &plaintext) {
                    block_type out = {0};

                    word_type L, R;
                    load_be(plaintext.data(), L, R);

                    for (size_t r = 0; r != rounds; ++r) {
                        L += (((R << 4) ^ (R >> 5)) + R) ^ key_schedule[2 * r];
                        R += (((L << 4) ^ (L >> 5)) + L) ^ key_schedule[2 * r + 1];
                    }

                    store_be(out.data(), L, R);

                    return out;
                }

                inline block_type decrypt_block(const block_type &ciphertext) {
                    block_type out = {0};

                    word_type L, R;
                    load_be(ciphertext.data(), L, R);

                    for (size_t r = 0; r != rounds; ++r) {
                        R -= (((L << 4) ^ (L >> 5)) + L) ^ key_schedule[63 - 2 * r];
                        L -= (((R << 4) ^ (R >> 5)) + R) ^ key_schedule[62 - 2 * r];
                    }

                    store_be(out.data(), L, R);

                    return out;
                }

                inline void schedule_key(const key_type &key) {
                    std::array<word_type, 4> UK = {0};
                    for (size_t i = 0; i != 4; ++i) {
                        UK[i] = load_be<uint32_t>(key, i);
                    }

                    uint32_t D = 0;
                    for (size_t i = 0; i != key_schedule_size; i += 2) {
                        key_schedule[i] = D + UK[D % 4];
                        D += 0x9E3779B9;
                        key_schedule[i + 1] = D + UK[(D >> 11) % 4];
                    }
                }

            };
        }
    }
}

#endif