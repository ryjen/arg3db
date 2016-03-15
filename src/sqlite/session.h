/*!
 * @file session.h
 */
#ifndef ARG3_DB_SQLITE_SESSION_H
#define ARG3_DB_SQLITE_SESSION_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBSQLITE3

#include <sqlite3.h>
#include "../session.h"
#include "../session_factory.h"
#include "transaction.h"

namespace arg3
{
    namespace db
    {
        namespace sqlite
        {
            namespace cache
            {
                /*!
                 * level of caching results in a database
                 */
                typedef enum { None, ResultSet, Row, Column } level;
            }

            class factory : public session_factory
            {
               public:
                std::shared_ptr<arg3::db::session_impl> create(const uri &uri);
            };

            /*!
             * a sqlite specific implementation of a database
             */
            class session : public arg3::db::session_impl, public std::enable_shared_from_this<session>
            {
                friend class factory;
                friend class statement;

               protected:
                std::shared_ptr<sqlite3> db_;
                cache::level cacheLevel_;

               public:
                /*!
                 * @param info   the connection info
                 */
                session(const uri &info);

                /* boilerplate */
                session(const session &other) = delete;
                session(session &&other);
                session &operator=(const session &other) = delete;
                session &operator=(session &&other);
                virtual ~session();

                /* sqlsession overrides */
                bool is_open() const;
                void open(int flags);
                void open();
                void close();
                long long last_insert_id() const;
                int last_number_of_changes() const;
                std::shared_ptr<resultset_impl> query(const std::string &sql);
                bool execute(const std::string &sql);
                std::string last_error() const;
                std::shared_ptr<statement_type> create_statement();
                std::shared_ptr<transaction_impl> create_transaction() const;
                std::shared_ptr<transaction_impl> create_transaction(transaction::type type) const;

                /*! @copydoc
                 *  overriden for sqlite3 specific pragma parsing
                 */
                void query_schema(const std::string &dbName, const std::string &tableName, std::vector<column_definition> &columns);

                /*!
                 * sets the cache level
                 * @param level of caching
                 */
                session &cache_level(cache::level level);

                /*!
                 * gets the cache level for this database
                 * @return the level of caching
                 */
                cache::level cache_level() const;
            };
        }
    }
}

#endif

#endif
