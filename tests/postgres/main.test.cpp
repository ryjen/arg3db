/*!
 * @copyright ryan jennings (coda.life), 2013 under LGPL
 */
#include <string>

#include "../db.test.h"
#include "../util.h"
#include "postgres/session.h"
#include "record.h"
#include "select_query.h"
#include "sqldb.h"
#include <bandit/bandit.h>

using namespace bandit;

using namespace std;

using namespace coda::db;

using namespace snowhouse;

namespace coda::db::test {
      void register_current_session() {
        auto pq_factory = std::make_shared<test::factory>();
        register_session("postgres", pq_factory);
        register_session("postgresql", pq_factory);

        auto uri_s = get_env_uri("POSTGRES_URI", "postgres://test:test@127.0.0.1:5432/test");
        current_session = coda::db::create_session(uri_s);
        cout << "connecting to " << uri_s << endl;
        current_session->open();
        current_session->impl()->execute("create table if not exists users(id serial primary key "
                  "unique, first_name varchar(45), "
                  "last_name varchar(45), dval real, "
                  "data bytea, "
                  "tval "
                  "timestamp)");

        current_session->impl()->execute("create table if not exists user_settings(id serial primary "
                  "key unique, user_id integer not "
                  "null, valid smallint, created_at "
                  "timestamp)");
      }

      void unregister_current_session() {
        current_session->close();
      }

      class postgres_session : public coda::db::postgres::session,
                               public test::session {
        public:
        using postgres::session::session;

        void setup() override {
        }

        void teardown() override {
          execute("delete from users");
          execute("delete from user_settings");
        }
      };

      std::shared_ptr<coda::db::session_impl>
      factory::create(const coda::db::uri &value) {
        return std::make_shared<postgres_session>(value);
      }
} // namespace coda::db::test

go_bandit([]() {
  describe("postgres database", []() {
    before_each([]() { test::setup_current_session(); });
    after_each([]() { test::teardown_current_session(); });

    it("can handle bad parameters", []() {
      auto db = create_session("postgres://zzzzz:zzzzz@zzzz/zzzzz:0");

      AssertThrows(database_exception, db->open());
    });
    it("can_parse_uri", []() {
      auto postgres = create_session("postgres://localhost:4000/test");
      AssertThat(postgres != nullptr, IsTrue());
      AssertThat(postgres->connection_info().host, Equals("localhost"));
      AssertThat(postgres->connection_info().port, Equals("4000"));
      AssertThat(postgres->connection_info().path, Equals("test"));
    });
  });
});
