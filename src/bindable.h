/*!
 * @file bindable.h
 * An interface for data binding
 */
#ifndef CODA_DB_BINDABLE_H
#define CODA_DB_BINDABLE_H

#ifdef ENABLE_PARAMETER_MAPPING
#include <regex>
#endif

#include <set>
#include <unordered_map>
#include <vector>

namespace coda::db {
  class sql_value;

  /*!
   * represents something that can have a sql value binded to it
   */
  class bindable {
   protected:
    static const size_t prealloc_size;
    static const size_t prealloc_increment;

    /*!
     * bind_all override for one sql_value parameter
     * @param index the index of the binding
     * @param value the value to bind
     * @return a reference to this instance
     */
    template<typename T>
    bindable &bind_list(size_t index, const T &value) {
      return bind(index, value);
    }

   public:
#ifdef ENABLE_PARAMETER_MAPPING
    static const std::regex param_regex;
    static const std::regex index_regex;
    static const std::regex named_regex;
#endif

    bindable() = default;

    bindable(const bindable &other) = default;

    bindable(bindable &&other) noexcept = default;

    virtual ~bindable() = default;

    bindable &operator=(const bindable &other) = default;

    bindable &operator=(bindable &&other) noexcept = default;

    template<typename T, typename... List>
    bindable &bind_all(const T &value, const List &... argv) {
      return bind_list(1, value, argv...);
    }

    /*!
     * bind a list of a values, using the order of values as the index
     * @param index the initial index for the list
     * @param value the first value in the list
     * @param argv the remaining values
     * @return a reference to this instance
     */
    template<typename T, typename... List>
    bindable &bind_list(size_t index, const T &value, const List &... argv) {
      bind(index, value);
      bind_list(index + 1, argv...);
      return *this;
    }

    /*!
     * Binds a vector of values by index
     * @param values the vector of values
     * @param start_index the starting index for the values
     * @return a reference to this instance
     */
    bindable &bind(const std::vector<sql_value> &values, size_t start_index = 1);

    /*!
     * Binds a map of named parameters
     * @param values the map of values
     * @return a reference to this instance
     */
    bindable &bind(const std::unordered_map<std::string, sql_value> &values);

    /*!
     * Binds a sql_value using the other bind methods
     * @param index the index of the binding
     * @param value the value of the binding
     * @return a reference to this instance
     */
    virtual bindable &bind(size_t index, const sql_value &value) = 0;

    /*!
     * bind a named parameter
     * @param name the name of the parameter
     * @param value the value to bind
     * @return a reference to this instance
     */
    virtual bindable &bind(const std::string &name, const sql_value &value) = 0;

    virtual size_t num_of_bindings() const noexcept = 0;
  };
}  // namespace coda::db

#endif
