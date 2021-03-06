
#include "sqldb.h"
#include <unordered_map>
#include "exception.h"
#include "session.h"
#include "session_factory.h"

namespace coda::db {

  namespace impl {
    using factory_storage = std::unordered_map<std::string, std::shared_ptr<session_factory>>;

    factory_storage &factories() {
      // plain old static variable was not happy
      static factory_storage values;
      return values;
    };
  }  // namespace impl

  std::shared_ptr<session> create_session(const std::string &uristr) {
    db::uri uri(uristr);
    return create_session(uri);
  }

  std::shared_ptr<session> create_session(const uri &uri) {
    auto factory = impl::factories()[uri.protocol];

    if (factory == nullptr) {
      throw database_exception("unregistered database protocol: " + uri.protocol);
    }

    return std::make_shared<session>(factory->create(uri));
  }

  std::shared_ptr<session> open_session(const std::string &uristr) {
    db::uri uri(uristr);
    return open_session(uri);
  }

  std::shared_ptr<session> open_session(const uri &uri) {
    auto factory = impl::factories()[uri.protocol];

    if (factory == nullptr) {
      throw database_exception("unregistered database protocol: " + uri.protocol);
    }

    auto value = std::make_shared<session>(factory->create(uri));

    value->open();

    return value;
  }

  void register_session(const std::string &protocol, const std::shared_ptr<session_factory> &factory) {
    if (protocol.empty()) {
      throw database_exception("invalid protocol for session factory registration");
    }

    if (factory == nullptr) {
      throw database_exception("invalid factory for session factory registration");
    }

    impl::factories()[protocol] = factory;
  }
}  // namespace coda::db