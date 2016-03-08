/*!
 * implementation of a query
 * @copyright ryan jennings (arg3.com), 2013
 */
#include <cassert>
#include "query.h"
#include "exception.h"
#include "session.h"
#include "log.h"

using namespace std;

namespace arg3
{
    namespace db
    {
        query::query(const std::shared_ptr<arg3::db::session> &session) : session_(session), stmt_(nullptr), params_(), named_params_()
        {
            if (session_ == nullptr) {
                throw database_exception("No database provided for query");
            }
        }

        query::query(const query &other) noexcept : session_(other.session_),
                                                    stmt_(other.stmt_),
                                                    params_(other.params_),
                                                    named_params_(other.named_params_)
        {
        }

        query::query(query &&other) noexcept : session_(std::move(other.session_)),
                                               stmt_(std::move(other.stmt_)),
                                               params_(std::move(other.params_)),
                                               named_params_(std::move(other.named_params_))
        {
            other.session_ = nullptr;
            other.stmt_ = nullptr;
        }

        query::~query()
        {
        }

        query &query::operator=(const query &other)
        {
            session_ = other.session_;
            stmt_ = other.stmt_;
            params_ = other.params_;
            named_params_ = other.named_params_;
            return *this;
        }

        query &query::operator=(query &&other)
        {
            session_ = std::move(other.session_);
            stmt_ = std::move(other.stmt_);
            params_ = std::move(other.params_);
            named_params_ = std::move(other.named_params_);
            other.session_ = nullptr;
            other.stmt_ = nullptr;

            return *this;
        }

        std::shared_ptr<session> query::session() const
        {
            return session_;
        }

        void query::prepare(const string &sql)
        {
            if (stmt_ != nullptr) {
                // check if the statement is already prepared
                if (stmt_->is_valid()) {
                    return;
                }
            } else {
                stmt_ = session_->create_statement();
            }

            log::trace("Query: %s", sql.c_str());

            stmt_->prepare(sql);  // throws exception on error

            for (size_t i = 1; i <= params_.size(); i++) {
                auto &value = params_[i - 1];

                stmt_->bind_value(i, value);
            }

            for (auto &it : named_params_) {
                stmt_->bind(it.first, it.second);
            }
        }

        size_t query::assert_binding_index(size_t index)
        {
            if (index == 0) {
                throw binding_error("parameter index must be greater than zero");
            }

            if (index > params_.size()) {
                params_.resize(index);
            }

            return index - 1;
        }

        query &query::bind(size_t index, const string &value, int len)
        {
            params_[assert_binding_index(index)] = len > 0 ? value.substr(0, len) : value;

            return *this;
        }
        query &query::bind(size_t index, const wstring &value, int len)
        {
            params_[assert_binding_index(index)] = len > 0 ? value.substr(0, len) : value;

            return *this;
        }
        query &query::bind(size_t index, int value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }
        query &query::bind(size_t index, unsigned value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index, long long value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }
        query &query::bind(size_t index, unsigned long long value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index)
        {
            params_[assert_binding_index(index)] = nullptr;

            return *this;
        }

        query &query::bind(size_t index, const sql_null_type &value)
        {
            return bind(index);
        }
        query &query::bind(size_t index, float value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }
        query &query::bind(size_t index, double value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(size_t index, const sql_blob &value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }
        query &query::bind(size_t index, const sql_time &value)
        {
            params_[assert_binding_index(index)] = value;

            return *this;
        }

        query &query::bind(const string &name, const sql_value &value)
        {
            named_params_[name] = value;
            return *this;
        }

        string query::last_error()
        {
            if (stmt_ == nullptr) {
                return string();
            }

            return stmt_->last_error();
        }

        bool query::is_valid() const
        {
            return session_ != nullptr;
        }
        void query::reset()
        {
            params_.clear();
            named_params_.clear();
            stmt_ = nullptr;
        }
    }

    string join_params(const vector<string> &columns, bool update)
    {
        ostringstream buf;

        for (string::size_type i = 0; i < columns.size(); i++) {
            if (update) {
                buf << columns[i];
                buf << " = ";
            }

            buf.put('$');
            buf << (i + 1);

            if (i + 1 < columns.size()) {
                buf.put(',');
            }
        }
        return buf.str();
    }
}
