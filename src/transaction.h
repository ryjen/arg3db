#ifndef CODA_DB_TRANSACTION_H
#define CODA_DB_TRANSACTION_H

#include <memory>
#include <string>

namespace coda::db {
  class session;

  class transaction_impl {
   public:
    transaction_impl() = default;

    transaction_impl(const transaction_impl &other) = default;

    transaction_impl(transaction_impl &&other) noexcept = default;

    virtual ~transaction_impl() = default;

    transaction_impl &operator=(const transaction_impl &other) = default;

    transaction_impl &operator=(transaction_impl &&other) noexcept = default;

    virtual void start() = 0;

    virtual bool is_active() const noexcept = 0;
  };

  class transaction {
    friend class session;

   public:
    struct isolation {
      typedef enum { none, serializable, repeatable_read, read_commited, read_uncommited } level;
    };
    typedef enum { none, read_write, read_only } type;

    using session_type = session;

    transaction(const std::shared_ptr<session_type> &session, const std::shared_ptr<transaction_impl> &impl);

    transaction(const transaction &other) = default;

    transaction(transaction &&other) noexcept = default;

    ~transaction();

    transaction &operator=(const transaction &other) = default;

    transaction &operator=(transaction &&other) noexcept = default;

    void save(const std::string &name);

    void release(const std::string &name);

    void rollback(const std::string &name);

    void start();

    void commit();

    void rollback();

    bool is_active() const noexcept;

    void set_successful(bool value);

    bool is_successful() const noexcept;

    std::shared_ptr<transaction_impl> impl() const;

    std::shared_ptr<session_type> get_session() const;

   private:
    bool successful_;
    std::shared_ptr<session_type> session_;
    std::shared_ptr<transaction_impl> impl_;
  };
}  // namespace coda::db

#endif
