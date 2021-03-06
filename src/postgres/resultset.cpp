#include "resultset.h"
#include "../exception.h"
#include "row.h"
#include "session.h"

using namespace std;

namespace coda::db::postgres {
      resultset::resultset(const std::shared_ptr<postgres::session> &sess, const shared_ptr<PGresult> &stmt)
          : stmt_(stmt), sess_(sess), currentRow_(-1) {
        if (sess_ == nullptr) {
          throw database_exception("No database provided to postgres resultset");
        }

        if (stmt_ == nullptr) {
          throw database_exception("no statement provided to postgres resultset");
        }
      }

      bool resultset::is_valid() const noexcept { return stmt_ != nullptr && stmt_; }

      bool resultset::next() {
        if (!is_valid()) {
          return false;
        }

        return ++currentRow_ < PQntuples(stmt_.get());
      }

      void resultset::reset() { currentRow_ = -1; }

      resultset::row_type resultset::current_row() { return row_type(make_shared<row>(sess_, stmt_, currentRow_)); }
}  // namespace coda::db::postgres
