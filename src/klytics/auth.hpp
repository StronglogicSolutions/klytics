#ifndef __AUTH_HPP__
#define __AUTH_HPP__

#include <process.hpp>
#include <INIReader.h>
#include <nlohmann/json.hpp>

#include "constants.hpp"
#include "util.hpp"

struct AuthData {
  std::string access_token;
  std::string scope;
  std::string token_type;
  std::string expiry_date;
  std::string key;
  std::string token_app_path;
};

class Authenticator {

public:

Authenticator() {
  INIReader reader{constants::DEFAULT_CONFIG_PATH};

  if (reader.ParseError() < 0) {
    log("Error loading config");
    throw std::invalid_argument{"No configuration path"};
  }

  auto app_path = reader.GetString(constants::YOUTUBE_CONFIG_SECTION, constants::YOUTUBE_TOKEN_APP, "");
  if (!app_path.empty()) {
    m_auth.token_app_path = app_path;
  }

  auto youtube_key = reader.GetString(constants::YOUTUBE_CONFIG_SECTION, constants::YOUTUBE_KEY, "");
  if (!youtube_key.empty()) {
    m_auth.key = youtube_key;
  }
}

/**
 * FetchToken
 *
 * @returns [out] {bool}
 */
bool FetchToken() {
  using json = nlohmann::json;
  if (m_auth.access_token.empty()) {

    ProcessResult result = qx({m_auth.token_app_path});

    if (!result.error) {
      json auth_json = json::parse(result.output);

      if (!auth_json.is_null() && auth_json.is_object()) {
        m_auth.access_token = auth_json["access_token"].dump();
        m_auth.scope        = auth_json["scope"].dump();
        m_auth.token_type   = auth_json["token_type"].dump();
        m_auth.expiry_date  = auth_json["expiry_date"].dump();

        log("Fetched token successfully");
        m_authenticated = true;

        return true;
      }
    }
  }
  return false;
}

bool is_authenticated() {
  return m_authenticated;
}

std::string get_token() {
  return m_auth.access_token;
}

std::string get_key() {
  return m_auth.key;
}

private:
AuthData     m_auth;
bool         m_authenticated;
};

#endif // __AUTH_HPP__