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
std::string              channel_id;
std::string              id;
std::string              title;
std::string              description;
std::string              time;
std::vector<std::string> keywords;
VideoStats               stats;
};

struct Findings {
std::vector<VideoInfo> videos;

bool has_videos() { return !videos.empty(); }
};

inline std::vector<std::string> keywords_from_string(std::string s) {
  std::vector<std::string> keywords;

  if (s.size() > 2) {
    s = s.substr(1, s.size() - 2);
    size_t start;
    size_t end{0};
    auto delim = ",";

    while ((start = s.find_first_not_of(delim, end)) != std::string::npos) {
      end = s.find(delim, start);
      keywords.push_back(s.substr(start, end - start));
    }
  }
  return keywords;
}

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
      {PARAM_NAMES.at(PART_INDEX),       PARAM_VALUES.at(SNIPPET_INDEX)},              // snippet
      {PARAM_NAMES.at(KEY_INDEX),        m_authenticator.get_key()},                   // key
      {PARAM_NAMES.at(CHAN_ID_INDEX),    PARAM_VALUES.at(CHAN_KEY_INDEX)},             // channel id
      {PARAM_NAMES.at(EVENT_T_INDEX),    PARAM_VALUES.at(COMPLETED_EVENT_TYPE_INDEX)}, // event type
      {PARAM_NAMES.at(TYPE_INDEX),       PARAM_VALUES.at(VIDEO_TYPE_INDEX)},           // type
      {PARAM_NAMES.at(ORDER_INDEX),      PARAM_VALUES.at(DATE_VALUE_INDEX)},           // order by
      {PARAM_NAMES.at(MAX_RESULT_INDEX), std::to_string(5)}                            // limit
    }
  );

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object()) {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array() && items.size() > 0) {
      for (const auto& item : items) {
        try {
          VideoInfo info{
            .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
            .id          = SanitizeJSON(item["id"]["videoId"].dump()),
            .title       = StripLineBreaks(SanitizeJSON(item["snippet"]["title"].dump())),
            .description = SanitizeJSON(item["snippet"]["description"].dump()),
            .time        = to_readable_time(SanitizeJSON(item["snippet"]["publishedAt"].dump()).c_str())
          };

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
      {PARAM_NAMES.at(PART_INDEX),    VideoParamsFull()},
      {PARAM_NAMES.at(KEY_INDEX),     m_authenticator.get_key()},
      {PARAM_NAMES.at(ID_NAME_INDEX), id_string}
    }
  );

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object()) {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array() && items.size() == m_findings.videos.size()) {
      for (int i = 0; i < items.size(); i++) {
        try {
          const auto& item = items.at(i);
          VideoInfo& info  = m_findings.videos.at(i);

          info.stats.views    = SanitizeJSON(item["statistics"]["viewCount"].dump());
          info.stats.likes    = SanitizeJSON(item["statistics"]["likeCount"].dump());
          info.stats.dislikes = SanitizeJSON(item["statistics"]["dislikeCount"].dump());
          info.stats.comments = SanitizeJSON(item["statistics"]["commentCount"].dump());
          info.keywords       = keywords_from_string(SanitizeJSON(item["snippet"]["tags"].dump()));

        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }

      Table table{};
      table.add_row({"ID", "Title", "Time", "Views", "Likes", "Dislikes", "Comments"});

      for (const auto& video : m_findings.videos) {
        table.add_row({
          video.id, video.title, video.time, video.stats.views, video.stats.likes, video.stats.dislikes, video.stats.comments
        });
      }

      return table;
    }
  }
  return Table{};
}

tabulate::Table fetch_youtube_stats() {
  using namespace constants;
  using json = nlohmann::json;

  if (!m_authenticator.is_authenticated() && !m_authenticator.FetchToken()) {
    return tabulate::Table{};
  }

  m_findings.videos = fetch_channel_videos();

  std::string id_string{};

  for (const auto& info : m_findings.videos) {
    id_string += info.id + ",";
  }

  return fetch_video_stats(id_string);
}

std::vector<VideoInfo> fetch_rival_videos(VideoInfo video) {
  using namespace constants;
  using json = nlohmann::json;

  auto search_term = video.keywords.front();

  std::vector<VideoInfo> info_v{};

  cpr::Response r = cpr::Get(
    cpr::Url{URL_VALUES.at(SEARCH_URL_INDEX)},
    cpr::Header{
      {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
      {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}
    },
    cpr::Parameters{
      {PARAM_NAMES.at(PART_INDEX),       PARAM_VALUES.at(SNIPPET_INDEX)},              // snippet
      {PARAM_NAMES.at(KEY_INDEX),        m_authenticator.get_key()},                   // key
      {PARAM_NAMES.at(QUERY_INDEX),      search_term},                                 // query term
      {PARAM_NAMES.at(EVENT_T_INDEX),    PARAM_VALUES.at(COMPLETED_EVENT_TYPE_INDEX)}, // event type
      {PARAM_NAMES.at(TYPE_INDEX),       PARAM_VALUES.at(VIDEO_TYPE_INDEX)},           // type
      {PARAM_NAMES.at(ORDER_INDEX),      PARAM_VALUES.at(VIEW_COUNT_INDEX)},           // order by
      {PARAM_NAMES.at(MAX_RESULT_INDEX), std::to_string(5)}                            // limit
    }
  );

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object()) {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array()) {
      for (const auto& item : items) {
        try {
          VideoInfo info{
            .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
            .id          = SanitizeJSON(item["id"]["videoId"].dump()),
            .title       = StripLineBreaks(SanitizeJSON(item["snippet"]["title"].dump())),
            .description = SanitizeJSON(item["snippet"]["description"].dump()),
            .time        = to_readable_time(SanitizeJSON(item["snippet"]["publishedAt"].dump()).c_str())
          };

          info_v.push_back(info);

        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }
      log("Fetched rival videos");
    }
  }
  return info_v;
}

std::vector<VideoInfo> get_videos() {
  return m_findings.videos;
}

private:
Authenticator          m_authenticator;
Findings               m_findings;
};

#endif // __API_HPP__