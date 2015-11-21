#include <bandit/bandit.h>
#include "schema.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

go_bandit([]() {

    describe("schema", []() {
        before_each([]() { setup_testdb(); });

        after_each([]() { teardown_testdb(); });


        it("has primary keys", []() {
            user u;

            auto keys = u.schema()->primary_keys();

            Assert::That(keys.size(), Equals(1));

            Assert::That(keys[0], Equals("id"));
        });

        it("has operators", []() {
            schema s(testdb, "users");

            s.init();

            schema other(std::move(s));

            Assert::That(s.is_valid(), Equals(false));

            Assert::That(other.is_valid(), Equals(true));

            schema copy(testdb, "other_users");

            copy = other;

            Assert::That(copy.table_name(), Equals(other.table_name()));

            schema moved(testdb, "moved_users");

            moved = std::move(other);

            Assert::That(moved.is_valid(), Equals(true));

            Assert::That(other.is_valid(), Equals(false));
        });

        it("has columns", []() {
            schema s(testdb, "users");

            s.init();

            auto cd = s[0];

            ostringstream os;

            os << cd;

            Assert::That(os.str(), Equals("id"));
        });

    });


});
