//---------------------------------------------------------------------------//
// Copyright (c) 2018-2020 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2020 Nikita Kaskov <nbering@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_BLOCK_SM4_HPP
#define CRYPTO3_BLOCK_SM4_HPP

#include <boost/endian/conversion.hpp>

#include <nil/crypto3/block/detail/sm4/sm4_policy.hpp>

#include <nil/crypto3/block/detail/block_stream_processor.hpp>
#include <nil/crypto3/block/detail/cipher_modes.hpp>

namespace nil {
    namespace crypto3 {
        namespace block {
            /*!
             * @brief SM4. A 128-bit Chinese national cipher, required for use
             * in certain commercial applications in China. Quite slow. Probably
             * no reason to use it outside of legal requirements.
             *
             * @ingroup block
             */
            class sm4 {
            protected:
                typedef detail::sm4_policy policy_type;

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

                template<class Mode, typename StateAccumulator, std::size_t ValueBits>
                struct stream_processor {
                    struct params_type {

                        constexpr static const std::size_t value_bits = ValueBits;
                        constexpr static const std::size_t length_bits = policy_type::word_bits * 2;
                    };

                    typedef block_stream_processor<Mode, StateAccumulator, params_type> type;
                };

                
                typedef typename stream_endian::little_octet_big_bit endian_type;

                sm4(const key_type &key) {
                    schedule_key(key);
                }

                ~sm4() {
                    key_schedule.fill(0);
                }

                inline block_type encrypt(const block_type &plaintext) const {
                    return encrypt_block(plaintext);
                }

                inline block_type decrypt(const block_type &ciphertext) const {
                    return decrypt_block(ciphertext);
                }

            protected:
#define SM4_RNDS(k0, k1, k2, k3, F, C)               \
    do {                                             \
        B0 ^= F(B1 ^ B2 ^ B3 ^ key_schedule[k0], C); \
        B1 ^= F(B0 ^ B2 ^ B3 ^ key_schedule[k1], C); \
        B2 ^= F(B0 ^ B1 ^ B3 ^ key_schedule[k2], C); \
        B3 ^= F(B0 ^ B1 ^ B2 ^ key_schedule[k3], C); \
    } while (0)

                key_schedule_type key_schedule;

                inline block_type encrypt_block(const block_type &plaintext) const {
                    word_type B0 = boost::endian::native_to_big(plaintext[0]);
                    word_type B1 = boost::endian::native_to_big(plaintext[1]);
                    word_type B2 = boost::endian::native_to_big(plaintext[2]);
                    word_type B3 = boost::endian::native_to_big(plaintext[3]);

                    SM4_RNDS(0, 1, 2, 3, policy_type::t_slow, policy_type::constants);
                    SM4_RNDS(4, 5, 6, 7, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(8, 9, 10, 11, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(12, 13, 14, 15, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(16, 17, 18, 19, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(20, 21, 22, 23, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(24, 25, 26, 27, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(28, 29, 30, 31, policy_type::t_slow, policy_type::constants);

                    return {boost::endian::big_to_native(B3), boost::endian::big_to_native(B2),
                            boost::endian::big_to_native(B1), boost::endian::big_to_native(B0)};
                }

                inline block_type decrypt_block(const block_type &plaintext) const {
                    word_type B0 = boost::endian::native_to_big(plaintext[0]);
                    word_type B1 = boost::endian::native_to_big(plaintext[1]);
                    word_type B2 = boost::endian::native_to_big(plaintext[2]);
                    word_type B3 = boost::endian::native_to_big(plaintext[3]);

                    SM4_RNDS(31, 30, 29, 28, policy_type::t_slow, policy_type::constants);
                    SM4_RNDS(27, 26, 25, 24, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(23, 22, 21, 20, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(19, 18, 17, 16, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(15, 14, 13, 12, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(11, 10, 9, 8, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(7, 6, 5, 4, policy_type::t, policy_type::transposed_constants);
                    SM4_RNDS(3, 2, 1, 0, policy_type::t_slow, policy_type::constants);

                    return {boost::endian::big_to_native(B3), boost::endian::big_to_native(B2),
                            boost::endian::big_to_native(B1), boost::endian::big_to_native(B0)};
                }

                void schedule_key(const key_type &key) {
                    std::array<word_type, 4> K = {0};
                    K[0] = boost::endian::native_to_big(key[0]) ^ policy_type::fk[0];
                    K[1] = boost::endian::native_to_big(key[1]) ^ policy_type::fk[1];
                    K[2] = boost::endian::native_to_big(key[2]) ^ policy_type::fk[2];
                    K[3] = boost::endian::native_to_big(key[3]) ^ policy_type::fk[3];

                    for (size_t i = 0; i != 32; ++i) {
                        K[i % 4] ^=
                            policy_type::tp(K[(i + 1) % 4] ^ K[(i + 2) % 4] ^ K[(i + 3) % 4] ^ policy_type::ck[i],
                                            policy_type::constants);
                        key_schedule[i] = K[i % 4];
                    }

                    K.fill(0);
                }
            };
        }    // namespace block
    }        // namespace crypto3
}    // namespace nil
#endif
