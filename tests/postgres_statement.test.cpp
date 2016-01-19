
#include <bandit/bandit.h>
#include "db.test.h"
#include "postgres_statement.h"

#ifdef HAVE_LIBPQ

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("postgres statement", []() {
        before_each([]() { postgres_testdb.setup(); });

        after_each([]() { postgres_testdb.teardown(); });


        it("is movable", []() {
            postgres_statement stmt(&postgres_testdb);

            stmt.prepare("select * from users");

            AssertThat(stmt.is_valid(), IsTrue());

            postgres_statement s2(std::move(stmt));

            AssertThat(s2.is_valid(), IsTrue());

            AssertThat(stmt.is_valid(), IsFalse());

            postgres_statement s3(&postgres_testdb);

            AssertThat(s3.is_valid(), IsFalse());

            s3 = std::move(s2);

            AssertThat(s3.is_valid(), IsTrue());

            AssertThat(s2.is_valid(), IsFalse());
        });

        it("throws exceptions", []() {
            postgres_db db(uri(""));

            postgres_statement stmt(&db);

            AssertThrows(database_exception, stmt.prepare("select * from users"));

            stmt = postgres_statement(&postgres_testdb);

            AssertThrows(database_exception, stmt.prepare("asdfasdfasdf"));

            AssertThat(stmt.last_error().empty(), IsFalse());

            AssertThrows(binding_error, stmt.bind(1, 1));

            AssertThrows(binding_error, stmt.bind(1, 1234LL));

            AssertThrows(binding_error, stmt.bind(1, 123.123));

            AssertThrows(binding_error, stmt.bind(1, "12134123"));

            AssertThrows(binding_error, stmt.bind(1, sql_blob()));

            AssertThrows(binding_error, stmt.bind(1, sql_null));

        });

        it("can reset", []() {
            postgres_statement stmt(&postgres_testdb);

            stmt.prepare("select * from users");

            AssertThat(stmt.is_valid(), IsTrue());

            stmt.reset();

            stmt.prepare("select id, first_name from users where id > 0");

            AssertThat(stmt.is_valid(), IsTrue());
        });

    });

});

#endif