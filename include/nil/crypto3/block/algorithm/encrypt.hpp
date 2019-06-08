//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nilfoundation.org>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_BLOCK_ENCRYPT_HPP
#define CRYPTO3_BLOCK_ENCRYPT_HPP

#include <nil/crypto3/block/cipher_value.hpp>
#include <nil/crypto3/block/cipher_state.hpp>

namespace nil {
    namespace crypto3 {
        /*!
         * @defgroup block_algorithms Algorithms
         * @ingroup block
         * @brief Algorithms are meant to provide encryption interface similar to STL algorithms' one.
         */

        /*!
         * @brief
         *
         * @ingroup block_algorithms
         *
         * @tparam BlockCipher
         * @tparam InputIterator
         * @tparam OutputIterator
         * @tparam StreamEncrypter
         *
         * @param first
         * @param last
         * @param out
         *
         * @return
         */
        template<typename BlockCipher, typename InputIterator, typename KeyIterator, typename OutputIterator>
        OutputIterator encrypt(InputIterator first, InputIterator last, KeyIterator key_first, KeyIterator key_last,
                               OutputIterator out) {

            typedef typename BlockCipher::stream_encrypter_type EncryptionMode;
            typedef typename block::block_accumulator<EncryptionMode> CipherAccumulator;

            typedef block::detail::value_encrypter_impl<CipherAccumulator> StreamEncrypterImpl;
            typedef block::detail::itr_encrypter_impl<StreamEncrypterImpl, OutputIterator> EncrypterImpl;

            return EncrypterImpl(first, last, std::move(out), CiperState(BlockCipher(key_first, key_last)));
        }

        /*!
         * @brief
         *
         * @ingroup block_algorithms
         *
         * @tparam BlockCipher
         * @tparam InputIterator
         * @tparam OutputIterator
         * @tparam StreamEncrypter
         *
         * @param first
         * @param last
         * @param out
         *
         * @return
         */
        template<typename BlockCipher,
                 typename InputIterator,
                 typename OutputAccumulator = typename block::block_accumulator<
                         typename BlockCipher::stream_encoder_type>>
        OutputAccumulator &encrypt(InputIterator first, InputIterator last, typename OutputAccumulator::type &acc) {

            typedef typename BlockCipher::stream_encrypter_type EncryptionMode;
            typedef typename block::block_accumulator<EncryptionMode> CipherAccumulator;

            typedef block::detail::ref_encrypter_impl<CipherAccumulator> StreamEncrypterImpl;
            typedef block::detail::range_encrypter_impl<StreamEncrypterImpl> EncrypterImpl;

            return EncrypterImpl(first, last, acc);
        }

        /*!
         * @brief
         *
         * @ingroup block_algorithms
         *
         * @tparam BlockCipher
         * @tparam InputIterator
         * @tparam CipherState
         * @param first
         * @param last
         * @return
         */
        template<typename BlockCipher,
                 typename InputIterator,
                 typename KeyIterator,
                 typename CipherAccumulator = typename block::block_accumulator<
                         typename BlockCipher::stream_encoder_type>>
        block::detail::range_encrypter_impl<block::detail::value_encrypter_impl<CipherAccumulator>> encrypt(
                InputIterator first, InputIterator last, KeyIterator key_first, KeyIterator key_last) {
            typedef block::detail::value_encrypter_impl<CipherAccumulator> StreamEncrypterImpl;
            typedef block::detail::range_encrypter_impl<StreamEncrypterImpl> EncrypterImpl;

            return EncrypterImpl(first, last, CipherAccumulator(BlockCipher(key_first, key_last)));
        }

        /*!
         * @brief
         *
         * @ingroup block_algorithms
         *
         * @tparam BlockCipher
         * @tparam SinglePassRange
         * @tparam OutputIterator
         * @tparam CipherState
         * @param rng
         * @param out
         * @return
         */
        template<typename BlockCipher, typename SinglePassRange, typename KeyRange, typename OutputIterator>
        OutputIterator encrypt(const SinglePassRange &rng, const KeyRange &key, OutputIterator out) {

            typedef typename block::detail::range_stream_encrypter_traits<typename BlockCipher::stream_encrypter_type,
                                                                          SinglePassRange>::type CipherState;

            typedef block::detail::value_encrypter_impl<CipherState> StreamEncrypterImpl;
            typedef block::detail::itr_encrypter_impl<StreamEncrypterImpl, OutputIterator> EncrypterImpl;

            return EncrypterImpl(rng, std::move(out), CipherState(BlockCipher(key)));
        }

        /*!
         * @brief
         *
         * @ingroup block_algorithms
         *
         * @tparam BlockCipher
         * @tparam SinglePassRange
         * @tparam CipherState
         * @param r
         * @return
         */
        template<typename BlockCipher,
                 typename SinglePassRange,
                 typename KeyRange,
                 typename CipherState = typename block::detail::range_stream_encrypter_traits<
                         typename BlockCipher::stream_encrypter_type, SinglePassRange>::type>
        block::detail::range_encrypter_impl<detail::value_encrypter_impl<CipherState>> encrypt(const SinglePassRange &r,
                                                                                               const KeyRange &key) {

            typedef block::detail::value_encrypter_impl<CipherState> StreamEncrypterImpl;
            typedef block::detail::range_encrypter_impl<StreamEncrypterImpl> EncrypterImpl;

            return EncrypterImpl(r, CipherState(BlockCipher(key)));
        }
    } // namespace crypto3
} // namespace nil

#endif // include guard