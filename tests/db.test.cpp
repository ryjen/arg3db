/*!
 * @copyright ryan jennings (arg3.com), 2013 under LGPL
 */
#ifndef HAVE_CONFIG_H
#include "config.h"
#endif
#include <bandit/bandit.h>
#include "record.h"
#include "select_query.h"
#include "sqldb.h"
#include "db.test.h"

using namespace bandit;

using namespace std;

using namespace arg3::db;

std::shared_ptr<arg3::db::session> current_session;

std::random_device r;
std::default_random_engine rand_eng(r());

std::string get_env_uri(const char *name, const std::string &def)
{
    char *temp = getenv(name);

    if (temp != NULL) {
        return temp;
    }

    return def;
}


void register_test_sessions()
{
#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)
    auto sqlite_factory = std::make_shared<test_sqlite3_factory>();
    sqldb::register_session("file", sqlite_factory);
    sqldb::register_session("sqlite", sqlite_factory);
#endif
#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)
    auto mysql_factory = std::make_shared<test_mysql_factory>();
    sqldb::register_session("mysql", mysql_factory);
#endif
#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)
    auto pq_factory = std::make_shared<test_postgres_factory>();
    sqldb::register_session("postgres", pq_factory);
    sqldb::register_session("postgresql", pq_factory);
#endif
}
void setup_current_session()
{
    auto session = dynamic_pointer_cast<test_session>(current_session);

    if (session) {
        session->setup();
    }
}

void teardown_current_session()
{
    auto session = dynamic_pointer_cast<test_session>(current_session);

    if (session) {
        session->teardown();
    }
}

string random_name()
{
    char alpha[27] = "abcdefghijklmnopqrstuvwxyz";

    const int len = random_num(5, 9);
    char buf[20] = {0};

    for (int i = 0; i < len; i++) {
        int c = random_num<int>(0, 27);
        buf[i] = alpha[c];
    }
    buf[len] = 0;
    return string(buf) + std::to_string(random_num<int>(1000, 9999));
}

#if defined(HAVE_LIBSQLITE3) && defined(TEST_SQLITE)
arg3::db::session *test_sqlite3_factory::create(const arg3::db::uri &value)
{
    return new test_sqlite3_session(value);
}

void test_sqlite3_session::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key autoincrement, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob, tval timestamp)");
    execute(
        "create table if not exists user_settings(id integer primary key autoincrement, user_id integer not null, valid int(1), created_at "
        "timestamp)");
}

void test_sqlite3_session::teardown()
{
    close();
    unlink(connection_info().path.c_str());
    clear_schema("users");
    clear_schema("user_settings");
}
#endif

#if defined(HAVE_LIBMYSQLCLIENT) && defined(TEST_MYSQL)
arg3::db::session *test_mysql_factory::create(const arg3::db::uri &value)
{
    return new test_mysql_session(value);
}

void test_mysql_session::setup()
{
    open();
    execute(
        "create table if not exists users(id integer primary key auto_increment, first_name varchar(45), last_name varchar(45), dval real, data "
        "blob, tval timestamp)");
    execute(
        "create table if not exists user_settings(id integer primary key auto_increment, user_id integer not null, valid int(1), created_at "
        "timestamp)");
}

void test_mysql_session::teardown()
{
    execute("drop table users");
    execute("drop table user_settings");
    close();
    clear_schema("users");
    clear_schema("user_settings");
}
#endif

#if defined(HAVE_LIBPQ) && defined(TEST_POSTGRES)
arg3::db::session *test_postgres_factory::create(const arg3::db::uri &value)
{
    return new test_postgres_session(value);
}
void test_postgres_session::setup()
{
    open();
    execute(
        "create table if not exists users(id serial primary key unique, first_name varchar(45), last_name varchar(45), dval real, data bytea, tval "
        "timestamp)");

    execute("create table if not exists user_settings(id serial primary key unique, user_id integer not null, valid smallint, created_at timestamp)");
}

void test_postgres_session::teardown()
{
    execute("drop table users");
    execute("drop table user_settings");
    close();
    clear_schema("users");
    clear_schema("user_settings");
}
#endif

go_bandit([]() {
    describe("database", []() {
        it("can_parse_uri", []() {
            try {
#ifdef HAVE_LIBSQLITE3
                auto file = sqldb::create_session("file://test.db");
                AssertThat(file.get() != NULL, IsTrue());
#endif
#ifdef HAVE_LIBMYSQLCLIENT
                auto mysql = sqldb::create_session("mysql://localhost:4000/test");
                AssertThat(mysql.get() != NULL, IsTrue());
                AssertThat(mysql->connection_info().host, Equals("localhost"));
                AssertThat(mysql->connection_info().port, Equals("4000"));
                AssertThat(mysql->connection_info().path, Equals("test"));
#endif
#ifdef HAVE_LIBPQ
                auto postgres = sqldb::create_session("postgres://localhost:4000/test");
                AssertThat(postgres.get() != NULL, IsTrue());
                AssertThat(mysql->connection_info().host, Equals("localhost"));
                AssertThat(mysql->connection_info().port, Equals("4000"));
                AssertThat(mysql->connection_info().path, Equals("test"));
#endif

            } catch (const std::exception &e) {
                cerr << e.what() << endl;
                throw e;
            }
        });
    });
});
