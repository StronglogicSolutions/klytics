#ifndef __API_HPP__
#define __API_HPP__
#include <cpr/cpr.h>
#include <tabulate/table.hpp>
#include <HTML/HTML.h>

#include "klytics/auth/auth.hpp"

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░ STRUCTS ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/
struct VideoStats {
std::string              views;
std::string              likes;
std::string              dislikes;
std::string              comments;
std::vector<std::string> keywords;
};

struct VideoInfo {
std::string              channel_id;
std::string              id;
std::string              title;
std::string              description;
std::string              time;
std::string              url;
VideoStats               stats;
};

struct Findings {
bool has_videos() { return !videos.empty(); }
std::vector<VideoInfo> videos;
};

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░ FUNCTIONS ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

/**
 * keywords_from_string
 *
 * @param
 * @returns
 */
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


/**
 * youtube_id_to_url
 *
 * @param
 * @returns
 */
inline std::string youtube_id_to_url(std::string id) {
  return std::string{
    "https://youtube.com/watch?v=" + id
  };
}

/**
 * tags_to_string
 *
 * @param
 * @returns
 */
inline std::string tags_to_string(std::vector<std::string> tags) {
  std::string s{};
  s.reserve(tags.size() * 9);

  for (const auto& tag : tags) s += tag + '\n';

  return s;
}

/**
 * FormatTable
 *
 * @param
 * @param
 */
void FormatTable(tabulate::Table& table, uint8_t column_num);

/**
 * videos_to_table
 */
inline tabulate::Table videos_to_table(const std::vector<VideoInfo>& videos) {
  using namespace tabulate;

  Table table{};

  if (!videos.empty()) {
    table.add_row({"ID", "Title", "Time", "Views", "Likes", "Dislikes", "Comments", "Tags"});

    for (const auto& video : videos) {
      table.add_row({
        video.id, video.title, video.time, video.stats.views, video.stats.likes, video.stats.dislikes, video.stats.comments, tags_to_string(video.stats.keywords)
      });
    }
  }
  return table;
}

/**
 * counts_to_html
 *
 * @param
 * @returns
 */
inline std::string videos_to_html(const std::vector<VideoInfo>& videos) {
  HTML::Document document{"KIQ Analytics"};

  document.addAttribute("lang", "en");
  document.head() << HTML::Meta("utf-8")
                  << HTML::Meta("viewport", "width=device-width, initial-scale=1, shrink-to-fit=no");
  document.head() << HTML::Style(".navbar{margin-bottom:20px;}");
  document.body().cls("videos");

  HTML::Div main{"container"};
  main << HTML::Header1("KIQ Analytics");
  main << HTML::Header2("Follower Counts") << HTML::Break() << HTML::Break();

  HTML::Table table{};
  table.cls("table");
  table << HTML::Caption{"Results"}; //"ID", "Title", "Time", "Views", "Likes", "Dislikes", "Comments", "Tags"
  table << (HTML::Row() << HTML::ColHeader("ID")       << HTML::ColHeader("Title") << HTML::ColHeader("Time")
                        << HTML::ColHeader("Views")    << HTML::ColHeader("Likes") << HTML::ColHeader("Dislikes")
                        << HTML::ColHeader("Comments") << HTML::ColHeader("Tags"));

  for (const auto& video : videos)
    table << (HTML::Row() << HTML::Col(video.id) << HTML::Col(video.title) << HTML::Col(video.time) << HTML::Col(video.stats.views) << HTML::Col(video.stats.likes) << HTML::Col(video.stats.dislikes) << HTML::Col(video.stats.comments) << HTML::Col(tags_to_string(video.stats.keywords)));

  main     << std::move(table);
  document << std::move(main);

  return SanitizeOutput(document.toString());
}

/**
 * fetch_video_stats
 * @returns [out] {std::string}
 */
inline std::string table_to_formatted_string(tabulate::Table table) {
  using namespace tabulate;

  try {
    FormatTable(table, 7);
    table.column(2).format().font_align(FontAlign::right);
  } catch (const std::exception& e) {
    log(e.what());
  }
  return table.str();
}

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░ API CLASS ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/
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
          auto video_id = SanitizeJSON(item["id"]["videoId"].dump());

          VideoInfo info{
            .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
            .id          = video_id,
            .title       = SanitizeJSON(item["snippet"]["title"].dump()),
            .description = SanitizeJSON(item["snippet"]["description"].dump()),
            .time        = to_readable_time(SanitizeJSON(item["snippet"]["publishedAt"].dump()).c_str()),
            .url         = youtube_id_to_url(video_id)
          };

          info_v.push_back(info);

        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }
    }
    // log("Fetched video info for channel " + PARAM_VALUES.at(CHAN_KEY_INDEX));
  }

  return info_v;
}

std::vector<VideoStats> fetch_video_stats(std::string id_string) {
  using namespace constants;
  using namespace tabulate;

  using json = nlohmann::json;

  std::vector<VideoStats> stats{};

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
    if (!items.is_null() && items.is_array() && items.size()) {
      for (int i = 0; i < items.size(); i++) {
        try {
          const auto& item = items.at(i);
          // VideoInfo& info  = m_findings.videos.at(i);
          stats.emplace_back(VideoStats{
            .views    = SanitizeJSON(item["statistics"]["viewCount"].dump()),
            .likes    = SanitizeJSON(item["statistics"]["likeCount"].dump()),
            .dislikes = SanitizeJSON(item["statistics"]["dislikeCount"].dump()),
            .comments = SanitizeJSON(item["statistics"]["commentCount"].dump()),
            .keywords = (item["snippet"].contains("tags")) &&
                        (!item["snippet"]["tags"].empty()) ?
                          keywords_from_string(SanitizeJSON(item["snippet"]["tags"].dump())) :
                          std::vector<std::string>{}
          });

        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }
    }
  }
  return stats;
}

/**
 * fetch_youtube_stats
 *
 * @returns [out] {std::vector<VideoInfo}
 */
std::vector<VideoInfo> fetch_youtube_stats() {
  using namespace constants;
  using json = nlohmann::json;

  m_findings.videos.clear();

  if (m_authenticator.is_authenticated() || m_authenticator.FetchToken()) {
    m_findings.videos = fetch_channel_videos();
    std::string id_string{};

    for (const auto& info : m_findings.videos) {
      id_string += info.id + ",";
    }

    std::vector<VideoStats> stats      =  fetch_video_stats(id_string);
    auto                    stats_size = stats.size();

    if (stats_size == m_findings.videos.size()) {
      for (uint8_t i = 0; i < stats_size; i++) {
        m_findings.videos.at(i).stats = stats.at(i);
      }
    }
  }
  return m_findings.videos;
}

std::vector<VideoInfo> fetch_rival_videos(VideoInfo video) {
  using namespace constants;
  using json = nlohmann::json;

  auto search_term = video.stats.keywords.front();

  std::vector<VideoInfo> info_v{};
  std::string            id_string{};
  std::string            delim{};

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

  log(r.text);

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object()) {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array()) {
      for (const auto& item : items) {
        try {
          auto video_id = SanitizeJSON(item["id"]["videoId"].dump());
          VideoInfo info{
            .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
            .id          = video_id,
            .title       = SanitizeJSON(item["snippet"]["title"].dump()),
            .description = SanitizeJSON(item["snippet"]["description"].dump()),
            .time        = to_readable_time(SanitizeJSON(item["snippet"]["publishedAt"].dump()).c_str()),
            .url         = youtube_id_to_url(video_id)
          };

          info_v.push_back(info);
          id_string += delim + video_id;
          delim = ",";
        } catch (const std::exception& e) {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }
      // log("Fetched rival videos");
    }
  }

  std::vector<VideoStats> vid_stats  = fetch_video_stats(id_string);
  auto                    stats_size = vid_stats.size();

  if (stats_size == info_v.size()) {
    for (uint8_t i = 0; i < stats_size; i++) {
      info_v.at(i).stats = vid_stats.at(i);
    }
  }
  return info_v;
}

/**
 * find_similar_videos
 *
 * @param   [in]  {VideoInfo}
 * @returns [out] {std::vector<VideoInfo>}
 */
std::vector<VideoInfo> find_similar_videos(VideoInfo video) {
  return fetch_rival_videos(video);
}

/**
 * get_videos
 */
std::vector<VideoInfo> get_videos() {
  return m_findings.videos;
}

private:
Authenticator          m_authenticator;
Findings               m_findings;
};

#endif // __API_HPP__