#ifndef __API_HPP__
#define __API_HPP__
#include <cpr/cpr.h>
#include <tabulate/table.hpp>
#include "auth.hpp"

struct VideoStats {
std::string views;
std::string likes;
std::string dislikes;
std::string comments;
};

struct VideoInfo {
std::string channel_id;
std::string id;
std::string title;
std::string description;
std::string time;
VideoStats  stats;
};

class API {

public:

bool is_authenticated() {
  return m_authenticator.is_authenticated();
}

bool init() {
  return m_authenticator.FetchToken();
}

std::vector<VideoInfo> fetch_channel_videos() {
  using namespace constants;
  using json = nlohmann::json;

  std::vector<VideoInfo> info_v{};

  cpr::Response r = cpr::Get(
    cpr::Url{URL_VALUES.at(SEARCH_URL_INDEX)},
    cpr::Header{
      {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
      {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}
    },
    cpr::Parameters{
      {PARAM_NAMES.at(PART_INDEX),    PARAM_VALUES.at(SNIPPET_INDEX)},
      {PARAM_NAMES.at(KEY_INDEX),     m_authenticator.get_key()},
      {PARAM_NAMES.at(CHAN_ID_INDEX), PARAM_VALUES.at(CHAN_KEY_INDEX)},
      {PARAM_NAMES.at(EVENT_T_INDEX), PARAM_VALUES.at(COMPLETED_EVENT_TYPE_INDEX)},
      {PARAM_NAMES.at(TYPE_INDEX),    PARAM_VALUES.at(VIDEO_TYPE_INDEX)}
    }
  );

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object()) {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array() && items.size() > 0) {
      for (const auto& item : items) {
        try {
          auto time = item["snippet"]["publishedAt"].dump();
          SanitizeJSON(time);

          VideoInfo info{
            .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
            .id          = item["id"]["videoId"].dump(),
            .title       = item["snippet"]["title"].dump(),
            .description = item["snippet"]["description"].dump(),
            .time        = to_readable_time(time.c_str())
          };

          SanitizeJSON(info.channel_id);
          SanitizeJSON(info.id);
          SanitizeJSON(info.title);
          SanitizeJSON(info.description);
          StripLineBreaks(info.title);

          info.title = CreateStringWithBreaks(info.title, 40);

          info_v.push_back(info);

        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }
    }
    log("Fetched video info for channel " + PARAM_VALUES.at(CHAN_KEY_INDEX));
  }

  return info_v;
}

tabulate::Table fetch_video_stats(std::string id_string) {
  using namespace constants;
  using namespace tabulate;

  using json = nlohmann::json;

  cpr::Response r = cpr::Get(
    cpr::Url{URL_VALUES.at(VIDEOS_URL_INDEX)},
    cpr::Header{
      {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
      {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}
    },
    cpr::Parameters{
      {PARAM_NAMES.at(PART_INDEX),    PARAM_VALUES.at(STATISTICS_INDEX) + "," + PARAM_VALUES.at(ID_VALUE_INDEX)},
      {PARAM_NAMES.at(KEY_INDEX),     m_authenticator.get_key()},
      {PARAM_NAMES.at(ID_NAME_INDEX), id_string}
    }
  );

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object()) {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array() && items.size() == m_video_info.size()) {
      for (int i = 0; i < items.size(); i++) {
        try {
          const auto& item = items.at(i);
          VideoInfo& info  = m_video_info.at(i);

          info.stats.views    = item["statistics"]["viewCount"].dump();
          info.stats.likes    = item["statistics"]["likeCount"].dump();
          info.stats.dislikes = item["statistics"]["dislikeCount"].dump();
          info.stats.comments = item["statistics"]["commentCount"].dump();

          SanitizeJSON(info.stats.views);
          SanitizeJSON(info.stats.likes);
          SanitizeJSON(info.stats.dislikes);
          SanitizeJSON(info.stats.comments);

        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }

      Table table{};
      table.add_row({"ID", "Title", "Time", "Views", "Likes", "Dislikes", "Comments"});

      for (const auto& info : m_video_info) {
        table.add_row({
          info.id, info.title, info.time, info.stats.views, info.stats.likes, info.stats.dislikes, info.stats.comments
        });
      }

      return table;
    }
  }
  return Table{};
}

tabulate::Table fetch_youtube_counts() {
  using namespace constants;
  using json = nlohmann::json;

  if (!m_authenticator.is_authenticated() && !m_authenticator.FetchToken()) {
    return tabulate::Table{};
  }

  m_video_info = fetch_channel_videos();

  std::string id_string{};

  for (const auto& info : m_video_info) {
    id_string += info.id + ",";
  }

  return fetch_video_stats(id_string);
}

private:
Authenticator          m_authenticator;
std::vector<VideoInfo> m_video_info;
};

#endif // __API_HPP__