/*!
 * @file select_query.h
 * @copyright ryan jennings (coda.life), 2013
 */
#ifndef CODA_DB_SELECT_QUERY_H
#define CODA_DB_SELECT_QUERY_H

#include "join_clause.h"
#include "query.h"
#include <utility>
#include "resultset.h"
#include "where_clause.h"

namespace coda::db {
  struct union_operator;

  namespace union_op {
    typedef enum { none, all } type;
  }

  /*!
   * a query to select values from a table
   */
  class select_query : public query, public whereable<select_query> {
   private:
    where_builder where_;
    std::vector<join_clause> join_;
    std::string limit_;
    std::string orderBy_;
    std::string groupBy_;
    std::vector<std::string> columns_;
    std::string tableName_;
    std::shared_ptr<union_operator> union_;

    /*!
     * selects a column
     * @param value the name of the column to find
     * @return a reference to this
     */
    select_query &column(const std::string &value) {
      if (!value.empty()) {
        columns_.push_back(value);
        set_modified();
      }
      return *this;
    }

    /*!
     * generates the sql for this query
     */
    std::string generate_sql() const override;

   public:
    /*!
     * defaults to 'select *'
     * @param db the database in use
     */
    explicit select_query(const std::shared_ptr<coda::db::session> &session);

    /*!
     * @param db        the database in use
     * @param tableName the table name to query
     * @param columns   the columns to query
     */
    select_query(const std::shared_ptr<coda::db::session> &session, const std::vector<std::string> &columns);

    /*!
     * @param schema    the schema to query
     */
    explicit select_query(const std::shared_ptr<schema> &schema);

    /*!
     * @param db  the database in use
     * @param columns the columns to query
     * @param tableName the table to query from
     */
    select_query(const std::shared_ptr<coda::db::session> &session, const std::vector<std::string> &columns,
                 const std::string &tableName);

    /* boilerplate */
    select_query(const select_query &other) = default;

    select_query(select_query &&other) noexcept = default;

    ~select_query() override = default;

    select_query &operator=(const select_query &other) = default;

    select_query &operator=(select_query &&other) noexcept = default;

    /*!
     * sets which table to select from
     * @param  tableName the table name
     * @return           a reference to this instance
     */
    select_query &from(const std::string &tableName);

    /*!
     * sets the table to select from
     * @param tableName the table name to select from
     * @param alias the alias for the table name
     * @return a reference to this
     */
    select_query &from(const std::string &tableName, const std::string &alias);

    /*!
     * gets the select from table name for this query
     * @return the table name
     */
    std::string from() const;

    /*!
     * sets the columns to select
     * @param other a vector of column names
     * @return a reference to this
     */
    select_query &columns(const std::vector<std::string> &other);

    /*!
     * sets the columns to select
     * @param value the initial column name
     * @param args the remaining column names
     * @return a reference to this
     */
    template<typename... List>
    select_query &columns(const std::string &value, const List &... args) {
      column(value);
      columns(args...);
      return *this;
    }

    /*!
     * @return the columns being queried
     */
    std::vector<std::string> columns() const;

    /*!
     * gets the limit clause for the query
     * @return the limit sql string
     */
    std::string limit() const;

    /*!
     * gets the order by clause for the query
     * @return the order by sql string
     */
    std::string order_by() const;

    /*!
     * gets the group by clause for the query
     * @return the group by sql string
     */
    std::string group_by() const;

    /*!
     * gets the where builder for the query
     * @return a writeable reference to the where builder
     */
    where_builder &where() override;

    /*!
     * gets the where clause for this query
     * @return the where clause
     */
    where_builder &where(const sql_operator &value) override;

#ifdef ENABLE_PARAMETER_MAPPING
    /*!
     * adds a where clause to this query
     * @param  value the where clause
     * @return       a reference to this
     */
    select_query &where(const where_clause &value);

    /*!
     * builds a where clause from a SQL string
     * @return a reference to this
     */
    where_builder &where(const std::string &sql);

    /*!
     * adds a where clause to this query and binds parameters to it
     * @param value the sql where string
     * @param args the variadic list of bind values
     * @return a reference to this instance
     */
    template <typename... List>
    select_query &where(const std::string &value, const List &... args) {
      where(value);
      bind_all(args...);
      return *this;
    }

    /*!
     * adds a where clause and binds parameters to it
     * @param value the where clause
     * @param args a variadic list of bind values
     * @return a reference to this instance
     */
    template <typename... List>
    select_query &where(const where_clause &value, const List &... args) {
      where(value);
      bind_all(args...);
      return *this;
    }
#endif

    /*!
     * sets the limit by clause for this query
     * @param  value the limit sql string
     * @return       a reference to this
     */
    select_query &limit(const std::string &value);

    /*!
     * sets the order by clause for this query
     * @param  value the order by sql string
     * @return       a reference to this
     */
    select_query &order_by(const std::string &value);

    /*!
     * sets the group by clause for this query
     * @param  value the group by sql string
     * @return       a reference to this
     */
    select_query &group_by(const std::string &value);

    /*!
     * sets the join clause for this query
     * @param  tableName the table name to join
     * @param  type      the type of join
     * @return           a join clause to perform additional modification
     */
    join_clause &join(const std::string &tableName, join::type type = join::inner);

    /*!
     * sets the join clause for this query
     * @param  tableName    the table name to join
     * @param  alias        the alias of the table name
     * @param  type         the type of join
     * @return a reference to this
     */
    join_clause &join(const std::string &tableName, const std::string &alias, join::type type = join::inner);

    /*!
     * sets the join clause for this query
     * @param  value the join clause to set
     * @return       a reference to this
     */
    select_query &join(const join_clause &value);

    /*!
     * sets a union query
     * @param  query the query to union with
     * @param  type  the type of union
     * @return       a reference to this instance
     */
    select_query &union_with(const select_query &query, union_op::type type = union_op::none);

    /*!
     * executes this query
     * @return a resultset object
     */
    resultset execute();

    /*!
     * executes this query
     * @param funk a callback to perform on the resultset
     */
    void execute(const std::function<void(const resultset &)> &funk);

    /*!
     * executes this query
     * @return a count of the number of rows
     */
    long long count();

    /*!
     * resets this query for re-execution
     */
    void reset() override;

    /*!
     * return the first column in the first row of the result set
     */
    template<typename T, typename = std::enable_if<is_sql_value<T>::value || is_sql_number<T>::value>>
    T execute_scalar() {
      auto rs = execute();

      if (!rs.is_valid()) {
        return T();
      }

      auto row = rs.begin();

      if (row == rs.end() || !row->is_valid()) {
        return T();
      }

      auto col = row->begin();

      if (col == row->end() || !col->is_valid()) {
        return T();
      }

      return col->value();
    }
  };

  /*!
   * union's one select query with another
   */
  struct union_operator {
    select_query query;
    union_op::type type;

    /*!
     * @param query the query to find
     * @param type the type of union
     */
    explicit union_operator(select_query query, union_op::type type = union_op::none)
        : query(std::move(query)), type(type) {}
  };

  /*!
   * output stream operator for a select query
   */
  std::ostream &operator<<(std::ostream &out, const select_query &other);
}  // namespace coda::db

#endif
