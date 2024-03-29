#pragma once

#include <vector>
#include <string>
#include <string_view>

namespace klytics {

namespace constants {
// Paths

const std::string FOLLOWER_JSON{"../config/followers.json"};
const std::string FOLLOWERS_IG_JSON{"../config/ig_followers.json"};
const std::string YOUTUBE_QUOTA_PATH{"../config/youtube_quota.txt"};

// URL Indexes
const uint8_t SEARCH_URL_INDEX           = 0x00;
const uint8_t VIDEOS_URL_INDEX           = 0x01;
const uint8_t LIVE_CHAT_URL_INDEX        = 0x02;
const uint8_t GOOGLE_AUTH_URL_INDEX      = 0x03;
const uint8_t CHANNELS_URL_INDEX         = 0x04;

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
const uint8_t ORDER_INDEX                = 0x0B;
const uint8_t MAX_RESULT_INDEX           = 0x0C;
const uint8_t QUERY_INDEX                = 0x0D;

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
const uint8_t DATE_VALUE_INDEX           = 0x0A;
const uint8_t CONTENT_DETAILS_INDEX      = 0x0B;
const uint8_t VIEW_COUNT_INDEX           = 0x0C;
const uint8_t SNIPPET_STATS_INDEX        = 0x0D;

// Strings
const std::vector<std::string> URL_VALUES{
  "https://www.googleapis.com/youtube/v3/search",
  "https://www.googleapis.com/youtube/v3/videos",
  "https://www.googleapis.com/youtube/v3/liveChat/messages",
  "https://oauth2.googleapis.com/token",
  "https://www.googleapis.com/youtube/v3/channels"
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
  "grant_type",
  "order",
  "maxResults",
  "q"
};

static const uint8_t KSTYLEYO_CHANNEL_ID_INDEX             = 0x00;
static const uint8_t WALKAROUNDWORLD_CHANNEL_ID_INDEX      = 0x01;
static const uint8_t STRONGLOGICSOLUTIONS_CHANNEL_ID_INDEX = 0x02;

static const std::vector<std::string> CHANNEL_IDS{
  "UC1XoiwW6b0VIYPOaP1KgV7A", // KStyleYo
  "UCAQ52nRnrg5epd6znUny-VA", // Walk Around The World
  "UCK0xH_L9OBM0CVwC438bMGA", // StrongLogic Solutions
};

static const std::vector<std::string> PARAM_VALUES{
  "UC1XoiwW6b0VIYPOaP1KgV7A",    // KStyleYo
  // "UCAQ52nRnrg5epd6znUny-VA", // Walk Around The World
  // "UCK0xH_L9OBM0CVwC438bMGA", // StrongLogic Solutions
  // "UCm5J1Fu_dHgBcMTpXu-NXUw", // Pangburn
  // "UCLwNTXWEjVd2qIHLcXxQWxA", // Timecast IRL
  // "UCfpnY5NnBl-8L7SvICuYkYQ", // Scott Adams
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
  "date",
  "contentDetails",
  "viewCount",
  "snippet,statistics"
};

static const std::string E_CHANNEL_ID{"UCFP7BAwQIzqml"};
static const std::string DEFAULT_CONFIG_PATH{"config/config.ini"};
static const std::string KLYTICS_CONFIG_SECTION{"klytics"};
static const std::string GOOGLE_CONFIG_SECTION{"google"};
static const std::string REFRESH_TOKEN{"refresh_token"};
static const std::string CLIENT_ID{"client_id"};
static const std::string CLIENT_SECRET{"client_secret"};
static const std::string YOUTUBE_KEY{"key"};
static const std::string YOUTUBE_CONFIG_SECTION{"youtube"};
static const std::string YOUTUBE_TOKEN_APP{"token_app"};
static const std::string YOUTUBE_USERNAME{"chat_name"};
static const std::string YOUTUBE_GREET{"greet"};
static const std::string YOUTUBE_TEST_MODE{"test_mode"};
static const std::string YOUTUBE_RETRY_MODE{"retry"};
static const std::string INSTAGRAM_CONFIG_SECTION{"instagram"};
static const std::string INSTAGRAM_USERNAME{"stronglogicp"};
static const std::string IG_FEED_APP_KEY{"ig_feed_app"};
static const std::string TW_FEED_APP_KEY{"tw_feed_app"};
static const std::string YT_FOLL_APP_KEY = "yt_follower_app";
static const std::string IG_FOLL_APP_KEY = "ig_follower_app";

inline const std::string VideoParamsFull() {
  return std::string{
    PARAM_VALUES.at(SNIPPET_INDEX) + "," +
    PARAM_VALUES.at(CONTENT_DETAILS_INDEX) + "," +
    PARAM_VALUES.at(STATISTICS_INDEX)
  };
        //  + "," + PARAM_VALUES.at(ID_VALUE_INDEX)
}

/**
* youtube data api constants
*/
namespace youtube {
const uint32_t YOUTUBE_DAILY_QUOTA        = 10000;

const uint8_t  VIDEO_LIST_QUOTA_INDEX     = 0x00;
const uint8_t  CHANNEL_LIST_QUOTA_INDEX   = 0x01;
const uint8_t  COMMENT_LIST_QUOTA_INDEX   = 0x02;
const uint8_t  SEARCH_LIST_QUOTA_INDEX    = 0x03;
const uint8_t  COMMENT_INSERT_QUOTA_INDEX = 0x04;

const std::vector<uint32_t> QUOTA_LIMIT{
  1,
  1,
  1,
  100,
  50
};

const uint8_t YOUTUBE_VIDEO_ID_LENGTH     = 11;
} // namespace youtube
} // namespace constants
} // namespace klytics
