//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020 Nikita Kaskov <nbering@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_BLOCK_TWOFISH_HPP
#define CRYPTO3_BLOCK_TWOFISH_HPP

#include <boost/endian/arithmetic.hpp>
#include <boost/endian/conversion.hpp>

#include <nil/crypto3/block/detail/twofish/twofish_policy.hpp>

#include <nil/crypto3/block/detail/block_stream_processor.hpp>
#include <nil/crypto3/block/detail/cipher_modes.hpp>

namespace nil {
    namespace crypto3 {
        namespace block {
            /*!
             * @brief Twofish. An AES contender. Somewhat complicated key setup
             * and a "kitchen sink" design.
             *
             * @ingroup block
             *
             * @tparam KeyBits Block cipher key bits. Available values are: 128, 192, 256.
             */
            template<std::size_t KeyBits>
            class twofish {
            protected:
                typedef detail::twofish_policy<KeyBits> policy_type;

                constexpr static const std::size_t key_schedule_size = policy_type::key_schedule_size;
                typedef typename policy_type::key_schedule_type key_schedule_type;

                constexpr static const std::size_t expanded_substitution_size = policy_type::expanded_substitution_size;
                typedef typename policy_type::expanded_substitution_type expanded_substitution_type;

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

                template<class Mode, typename StateAccumulator, std::size_t ValueBits>
                struct stream_processor {
                    struct params_type {

                        constexpr static const std::size_t value_bits = ValueBits;
                        constexpr static const std::size_t length_bits = policy_type::word_bits * 2;
                    };

                    typedef block_stream_processor<Mode, StateAccumulator, params_type> type;
                };

                typedef typename stream_endian::little_octet_big_bit endian_type;

                twofish(const key_type &key) {
                    schedule_key(key);
                }

                ~twofish() {
                    round_key.fill(0);
                }

                inline block_type encrypt(const block_type &plaintext) const {
                    return encrypt_block(plaintext);
                }

                inline block_type decrypt(const block_type &ciphertext) const {
                    return decrypt_block(ciphertext);
                }

            protected:
                key_schedule_type round_key;
                expanded_substitution_type expanded_substitution;

                inline block_type encrypt_block(const block_type &plaintext) const {
                    word_type A = boost::endian::native_to_little(plaintext[0]);
                    word_type B = boost::endian::native_to_little(plaintext[1]);
                    word_type C = boost::endian::native_to_little(plaintext[2]);
                    word_type D = boost::endian::native_to_little(plaintext[3]);

                    A ^= round_key[0];
                    B ^= round_key[1];
                    C ^= round_key[2];
                    D ^= round_key[3];

                    for (size_t k = 8; k != expanded_substitution_size; k += 4) {
                        policy_type::tf_e(A, B, C, D, round_key[k], round_key[k + 1], expanded_substitution);
                        policy_type::tf_e(C, D, A, B, round_key[k + 2], round_key[k + 3], expanded_substitution);
                    }

                    C ^= round_key[4];
                    D ^= round_key[5];
                    A ^= round_key[6];
                    B ^= round_key[7];

                    return {boost::endian::little_to_native(C), boost::endian::little_to_native(D),
                            boost::endian::little_to_native(A), boost::endian::little_to_native(B)};
                }

                inline block_type decrypt_block(const block_type &ciphertext) const {
                    word_type A = boost::endian::native_to_little(ciphertext[0]);
                    word_type B = boost::endian::native_to_little(ciphertext[1]);
                    word_type C = boost::endian::native_to_little(ciphertext[2]);
                    word_type D = boost::endian::native_to_little(ciphertext[3]);

                    A ^= round_key[4];
                    B ^= round_key[5];
                    C ^= round_key[6];
                    D ^= round_key[7];

                    for (size_t k = expanded_substitution_size; k != 8; k -= 4) {
                        policy_type::F_D(A, B, C, D, round_key[k - 2], round_key[k - 1], expanded_substitution);
                        policy_type::F_D(C, D, A, B, round_key[k - 4], round_key[k - 3], expanded_substitution);
                    }

                    C ^= round_key[0];
                    D ^= round_key[1];
                    A ^= round_key[2];
                    B ^= round_key[3];

                    return {boost::endian::little_to_native(C), boost::endian::little_to_native(D),
                            boost::endian::little_to_native(A), boost::endian::little_to_native(B)};
                }

                inline void schedule_key(const key_type &key) {
                    return policy_type::schedule_key(key, expanded_substitution, round_key);
                }
            };
        }    // namespace block
    }        // namespace crypto3
}    // namespace nil

#endif
