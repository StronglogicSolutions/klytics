#include "api.hpp"

bool API::is_authenticated()
{
  return m_authenticator.is_authenticated();
}

bool API::init()
{
  return m_authenticator.FetchToken();
}

std::vector<VideoInfo> API::fetch_channel_videos()
{
  using namespace constants;
  using json = nlohmann::json;

  std::vector<VideoInfo> info_v{};

  cpr::Response r = cpr::Get(
      cpr::Url{URL_VALUES.at(SEARCH_URL_INDEX)},
      cpr::Header{
          {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
          {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}},
      cpr::Parameters{
          {PARAM_NAMES.at(PART_INDEX),       PARAM_VALUES.at(SNIPPET_INDEX)},    // snippet
          {PARAM_NAMES.at(KEY_INDEX),        m_authenticator.get_key()},         // key
          {PARAM_NAMES.at(CHAN_ID_INDEX),    PARAM_VALUES.at(CHAN_KEY_INDEX)},   // channel id
          {PARAM_NAMES.at(TYPE_INDEX),       PARAM_VALUES.at(VIDEO_TYPE_INDEX)}, // type
          {PARAM_NAMES.at(ORDER_INDEX),      PARAM_VALUES.at(DATE_VALUE_INDEX)}, // order by
          {PARAM_NAMES.at(MAX_RESULT_INDEX), std::to_string(5)}                  // limit
      });

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object())
  {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array() && items.size() > 0)
    {
      for (const auto &item : items)
      {
        try
        {
          auto video_id = SanitizeJSON(item["id"]["videoId"].dump());
          auto datetime = SanitizeJSON(item["snippet"]["publishedAt"].dump()).c_str();

          VideoInfo info{
              .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
              .id          = video_id,
              .title       = SanitizeOutput(SanitizeJSON(item["snippet"]["title"].dump())),
              .description = SanitizeOutput(SanitizeJSON(item["snippet"]["description"].dump())),
              .datetime    = datetime,
              .time        = to_readable_time(datetime),
              .url         = youtube_id_to_url(video_id)};

          info_v.push_back(info);
        }
        catch (const std::exception &e)
        {
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

std::vector<VideoStats> API::fetch_video_stats(std::string id_string)
{
  using namespace constants;

  using json = nlohmann::json;

  std::vector<VideoStats> stats{};

  cpr::Response r = cpr::Get(
      cpr::Url{URL_VALUES.at(VIDEOS_URL_INDEX)},
      cpr::Header{
          {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
          {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}},
      cpr::Parameters{
          {PARAM_NAMES.at(PART_INDEX),    VideoParamsFull()},
          {PARAM_NAMES.at(KEY_INDEX),     m_authenticator.get_key()},
          {PARAM_NAMES.at(ID_NAME_INDEX), id_string}});

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object())
  {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array() && items.size())
    {
      for (int i = 0; i < items.size(); i++)
      {
        try
        {
          const auto &item = items.at(i);

          stats.emplace_back(VideoStats{
            .views = (item["statistics"].contains("viewCount")) ? SanitizeJSON(item["statistics"]["viewCount"].dump()) : "0",
            .likes = (item["statistics"].contains("likeCount")) ? SanitizeJSON(item["statistics"]["likeCount"].dump()) : "0",
            .dislikes = (item["statistics"].contains("dislikeCount")) ? SanitizeJSON(item["statistics"]["dislikeCount"].dump()) : "0",
            .comments = (item["statistics"].contains("commentCount")) ? SanitizeJSON(item["statistics"]["commentCount"].dump()) : "0",
            .keywords = (item["snippet"].contains("tags")) &&
                        (!item["snippet"]["tags"].empty()) ?
                          keywords_from_string(SanitizeJSON(item["snippet"]["tags"].dump())) :
                          std::vector<std::string>{}});
        }
        catch (const std::exception &e)
        {
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
std::vector<VideoInfo> API::fetch_youtube_stats()
{
  using namespace constants;
  using json = nlohmann::json;

  m_videos.clear();

  if (m_authenticator.is_authenticated() || m_authenticator.FetchToken())
  {
    m_videos = fetch_channel_videos();
    std::string id_string{};

    for (const auto &info : m_videos)
    {
      id_string += info.id + ",";
    }

    std::vector<VideoStats> stats = fetch_video_stats(id_string);
    auto stats_size = stats.size();

    if (stats_size == m_videos.size())
    {
      for (uint8_t i = 0; i < stats_size; i++)
      {
        m_videos.at(i).stats = stats.at(i);
      }
    }
  }
  return m_videos;
}

std::vector<VideoInfo> API::fetch_rival_videos(VideoInfo video)
{
  using namespace constants;
  using json = nlohmann::json;

  auto search_term = video.stats.keywords.front();

  std::vector<VideoInfo> info_v{};
  std::string id_string{};
  std::string delim{};

  cpr::Response r = cpr::Get(
      cpr::Url{URL_VALUES.at(SEARCH_URL_INDEX)},
      cpr::Header{
          {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
          {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}},
      cpr::Parameters{
          {PARAM_NAMES.at(PART_INDEX),       PARAM_VALUES.at(SNIPPET_INDEX)},    // snippet
          {PARAM_NAMES.at(KEY_INDEX),        m_authenticator.get_key()},         // key
          {PARAM_NAMES.at(QUERY_INDEX),      search_term},                       // query term
          {PARAM_NAMES.at(TYPE_INDEX),       PARAM_VALUES.at(VIDEO_TYPE_INDEX)}, // type
          {PARAM_NAMES.at(ORDER_INDEX),      PARAM_VALUES.at(VIEW_COUNT_INDEX)}, // order by
          {PARAM_NAMES.at(MAX_RESULT_INDEX), std::to_string(5)}                  // limit
      });

  json video_info = json::parse(r.text);

  if (!video_info.is_null() && video_info.is_object())
  {
    auto items = video_info["items"];
    if (!items.is_null() && items.is_array())
    {
      for (const auto &item : items)
      {
        try
        {
          auto video_id = SanitizeJSON(item["id"]["videoId"].dump());
          auto datetime = SanitizeJSON(item["snippet"]["publishedAt"].dump()).c_str();

          VideoInfo info{
              .channel_id  = PARAM_VALUES.at(CHAN_KEY_INDEX),
              .id          = video_id,
              .title       = SanitizeOutput(SanitizeJSON(item["snippet"]["title"].dump())),
              .description = SanitizeOutput(SanitizeJSON(item["snippet"]["description"].dump())),
              .datetime    = datetime,
              .time        = to_readable_time(datetime),
              .url         = youtube_id_to_url(video_id)};

          info_v.push_back(info);
          id_string += delim + video_id;
          delim = ",";
        }
        catch (const std::exception &e)
        {
          std::string error_message{"Exception was caught: "};
          error_message += e.what();
          log(error_message);
        }
      }
      // log("Fetched rival videos");
    }
  }

  std::vector<VideoStats> vid_stats = fetch_video_stats(id_string);
  auto stats_size = vid_stats.size();

  if (stats_size == info_v.size())
  {
    for (uint8_t i = 0; i < stats_size; i++)
    {
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
std::vector<VideoInfo> API::find_similar_videos(VideoInfo video)
{
  return fetch_rival_videos(video);
}

/**
 * get_videos
 */
std::vector<VideoInfo> API::get_videos()
{
  return m_videos;
}

/**
 * has_videos
 *
 * @interface
 */

bool API::has_videos() {
  return !m_videos.empty();
}
