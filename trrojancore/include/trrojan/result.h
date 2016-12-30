/// <copyright file="result.h" company="SFB-TRR 161 Quantitative Methods for Visual Computing">
/// Copyright � 2016 SFB-TRR 161. Alle Rechte vorbehalten.
/// </copyright>
/// <author>Christoph M�ller</author>

#pragma once

#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "trrojan/configuration.h"
#include "trrojan/named_variant.h"


namespace trrojan {

    /// <summary>
    /// The results from a benchmark run in a single configuration.
    /// </summary>
    /// <remarks>
    /// The <see cref="trrojan::basic_result" /> can be used directly, but also
    /// be extended if a benchmark needs to return more information. Note that
    /// this information might not be processed by the built-in persistence
    /// layer.
    /// </remarks>
    class TRROJANCORE_API basic_result {

    public:

        /// <summary>
        /// The type used to store the results.
        /// </summary>
        typedef std::vector<variant> result_type;

        /// <summary>
        /// The type used to store the names of the results.
        /// </summary>
        typedef std::vector<std::string> result_names_type;

        inline basic_result(const configuration& configuration,
            const result_names_type& result_names)
            : _configuration(configuration),
            _result_names(result_names) { }

        inline basic_result(configuration&& configuration,
            result_names_type&& result_names)
            : _configuration(std::move(configuration)),
            _result_names(std::move(result_names)) { }

        inline basic_result(configuration&& configuration,
            std::initializer_list<std::string> result_names)
            : _configuration(std::move(configuration)),
            _result_names(std::move(result_names)) { }

        virtual ~basic_result(void);

        /// <summary>
        /// Append the results of a new measurement at the end.
        /// </summary>
        /// <param name="results"></param>
        /// <exception cref="std::invalid_argument">If the size of
        /// <paramref name="results" /> is not the same as the size of
        /// <see cref="basic_result::result_names" />.</exception>
        inline void add(const result_type& results) {
            this->check_size(results);
            this->_results.insert(this->_results.end(),
                results.cbegin(), results.cend());
        }

        /// <summary>
        /// Append the results of a new measurement at the end.
        /// </summary>
        /// <param name="results"></param>
        /// <exception cref="std::invalid_argument">If the size of
        /// <paramref name="results" /> is not the same as the size of
        /// <see cref="basic_result::result_names" />.</exception>
        inline void add(std::initializer_list<variant> results) {
            this->check_size(results);
            this->_results.insert(this->_results.end(), results);
        }

        /// <summary>
        /// Answer the <see cref="trrojan::configuration" /> the results are for.
        /// </summary>
        /// <returns>The configuration the results are for.</returns>
        inline const trrojan::configuration& configuration(void) const {
            return this->_configuration;
        }

        /// <summary>
        /// Answer the number of distinct measurements stored in the object, ie
        /// how often values for <see cref="basic_result::result_names" /> are
        /// available.
        /// </summary>
        /// <returns>The number of measurements.</returns>
        inline size_t measurements(void) const {
            return (this->_result_names.size() > 0)
                ? (this->_results.size() / this->_result_names.size())
                : 0;
        }

        /// <summary>
        /// Answer the raw results (ordered in tuples according to
        /// <see cref="basic_result::result_names" />) of the test.
        /// </summary>
        /// <returns>The results stored for the configuration.</returns>
        inline const result_type& raw_results(void) const {
            return this->_results;
        }

        /// <summary>
        /// Answer the names of the results stored for the configuration.
        /// </summary>
        /// <returns>A set of result names.</returns>
        inline const result_names_type& result_names(void) const {
            return this->_result_names;
        }

        /// <summary>
        /// Answer the results as key-value pairs.
        /// </summary>
        /// <remarks>
        /// <para>This operation expands the results in a way that it is easier
        /// to access certain data. However, it creates a copy and is rather
        /// expensive to perform.</para>
        /// <para>The order of the per-measurement results returned is
        /// guaranteed to be the same as the order of insertion.</para>
        /// </remarks>
        /// <returns>The results.</returns>
        std::vector<std::vector<named_variant>> results(void) const;

        /// <summary>
        /// Answer all results values for a specific result name.
        /// </summary>
        /// <remarks>
        /// <para>This method assumes that the result names are unique, which is
        /// not enforced by the ctors, but should be enforces by callers.</para>
        /// <para>The results for the different measurements are guaranteed to
        /// be returned in order.</para>
        /// </remarks>
        /// <param name="result_name"></param>
        /// <returns>All results for the given result name.</returns>
        /// <exception cref="std::invalid_argument">If the given result name was
        /// not found in the <see cref="basic_result" />.</exception>
        result_type results(const std::string& result_name) const;

    private:

        template<class T> void check_size(const T& results);

        trrojan::configuration _configuration;
        result_names_type _result_names;
        result_type _results;

    };


    /// <summary>
    /// A benchmark result.
    /// </summary>
    typedef std::shared_ptr<basic_result> result;
}

#include "trrojan/result.inl"
