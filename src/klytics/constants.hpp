#ifndef __YOUTUBE_CONSTANTS_HPP__
#define __YOUTUBE_CONSTANTS_HPP__

#include <vector>
#include <string>
#include <string_view>

namespace constants {
// URL Indexes
const uint8_t SEARCH_URL_INDEX           = 0x00;
const uint8_t VIDEOS_URL_INDEX           = 0x01;
const uint8_t LIVE_CHAT_URL_INDEX        = 0x02;
const uint8_t GOOGLE_AUTH_URL_INDEX      = 0x03;

// Header Name Indexes
const uint8_t ACCEPT_HEADER_INDEX        = 0x00;
const uint8_t AUTH_HEADER_INDEX          = 0x01;
const uint8_t CONTENT_TYPE_INDEX         = 0x02;

// Header Value Indexes
const uint8_t APP_JSON_INDEX             = 0x00;
const uint8_t FORM_URL_ENC_INDEX         = 0x01;

// Param Name Indexes
const uint8_t PART_INDEX                 = 0x00;
const uint8_t CHAN_ID_INDEX              = 0x01;
const uint8_t EVENT_T_INDEX              = 0x02;
const uint8_t TYPE_INDEX                 = 0x03;
const uint8_t KEY_INDEX                  = 0x04;
const uint8_t ID_NAME_INDEX              = 0x05;
const uint8_t LIVE_CHAT_ID_INDEX         = 0x06;
const uint8_t CLIENT_ID_INDEX            = 0x07;
const uint8_t CLIENT_SECRET_INDEX        = 0x08;
const uint8_t REFRESH_TOKEN_NAME_INDEX   = 0x09;
const uint8_t GRANT_TYPE_INDEX           = 0x0A;

// Param Value Indexes
const uint8_t CHAN_KEY_INDEX             = 0x00;
const uint8_t LIVE_EVENT_TYPE_INDEX      = 0x01;
const uint8_t COMPLETED_EVENT_TYPE_INDEX = 0x02;
const uint8_t SNIPPET_INDEX              = 0x03;
const uint8_t VIDEO_TYPE_INDEX           = 0x04;
const uint8_t LIVESTREAM_DETAILS_INDEX   = 0x05;
const uint8_t KY_CHAN_KEY_INDEX          = 0x06;
const uint8_t STATISTICS_INDEX           = 0x07;
const uint8_t ID_VALUE_INDEX             = 0x08;
const uint8_t REFRESH_TOKEN_VALUE_INDEX  = 0x09;

// Strings
const std::vector<std::string> URL_VALUES{
  "https://www.googleapis.com/youtube/v3/search",
  "https://www.googleapis.com/youtube/v3/videos",
  "https://www.googleapis.com/youtube/v3/liveChat/messages",
  "https://oauth2.googleapis.com/token"
  // "https://www.googleapis.com/oauth2/v3/token"
};

const std::vector<std::string> HEADER_NAMES{
  "Accept",
  "Authorization",
  "Content-Type"
};

const std::vector<std::string> HEADER_VALUES{
  "application/json",
  "application/x-www-form-urlencoded"
};

const std::vector<std::string> PARAM_NAMES = {
  "part",
  "channelId",
  "eventType",
  "type",
  "key",
  "id",
  "liveChatId",
  "client_id",
  "client_secret",
  "refresh_token",
  "grant_type"
};

const std::vector<std::string> PARAM_VALUES{
  // "UCK0xH_L9OBM0CVwC438bMGA",   // StrongLogic Solutions
  // "UCm5J1Fu_dHgBcMTpXu-NXUw", // Pangburn
  // "UCLwNTXWEjVd2qIHLcXxQWxA", // Timecast IRL
  "UC1XoiwW6b0VIYPOaP1KgV7A", // KStyleYo
  // "UCfpnY5NnBl-8L7SvICuYkYQ",  // Scott Adams
  // "UCQ7ZcQSfCiY0EJL8ZdbZ3Hw", // Samara games
  // "UCPfC_6VjklQDGuxgRt43m1w", // Fazmash
  // "UCPGuorlvarThSlwJpyTHOmQ", // Capcom fighting
  // "UCd534c_ehOvrLVL2v7Nl61w", // Muselk
  // "UCWjBpFRzYt4vB3zIleLKrLA", // Korea Museum stream

  "live",
  "completed",
  "snippet",
  "video",
  "liveStreamingDetails",
  "S15j0LRydks",
  "statistics",
  "id",
  "refresh_token",
};

const std::string E_CHANNEL_ID{"UCFP7BAwQIzqml"};
const std::string DEFAULT_CONFIG_PATH{"config/config.ini"};
const std::string GOOGLE_CONFIG_SECTION{"google"};
const std::string REFRESH_TOKEN{"refresh_token"};
const std::string CLIENT_ID{"client_id"};
const std::string CLIENT_SECRET{"client_secret"};
const std::string YOUTUBE_KEY{"key"};
const std::string YOUTUBE_CONFIG_SECTION{"youtube"};
const std::string YOUTUBE_TOKEN_APP{"token_app"};
const std::string YOUTUBE_USERNAME{"chat_name"};
const std::string YOUTUBE_GREET{"greet"};
const std::string YOUTUBE_TEST_MODE{"test_mode"};
const std::string YOUTUBE_RETRY_MODE{"retry"};

} // namespace constants

#endif // __YOUTUBE_CONSTANTS_HPP__
