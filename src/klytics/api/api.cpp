#include "api.hpp"

/**
 * @brief Construct a new API::API object
 *
 * @constructor
 *
 * @returns [out] {API}
 *
 * Notes:
 * - Sets the channels upon which API functions will be performed
 *
 */
API::API()
: m_channel_ids{
  constants::CHANNEL_IDS.at(constants::KSTYLEYO_CHANNEL_ID_INDEX),
  constants::CHANNEL_IDS.at(constants::WALKAROUNDWORLD_CHANNEL_ID_INDEX)
} {}

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool API::is_authenticated()
{
  return m_authenticator.is_authenticated();
}

/**
 * @brief
 *
 * @return true
 * @return false
 */
bool API::init()
{
  return m_authenticator.FetchToken();
}

bool API::fetch_channel_data() {
  std::string id_string{};
  for (const auto& channel_id : m_channel_ids) id_string += channel_id + ",";

  try {
    m_channels = fetch_channel_info(id_string);
    if (m_channels.size() == m_channel_ids.size()) {
      for (uint8_t i = 0; i < m_channels.size(); i++) {
        m_channels.at(i).id = m_channel_ids.at(i);
      }
      return true;
    }
  }
  catch (const std::exception& e) {
    log(e.what());
  }
  return false;
}

/**
 * fetch_channel_videos
 *
 * @return std::vector<VideoInfo>
 */
bool API::fetch_channel_videos()
{
  using namespace constants;
  using json = nlohmann::json;

  if (m_channels.empty()) {
    if (!fetch_channel_data()) {
      log("Unable to fetch channel data");
      return false;
    }
  }
  // TODO: Replace below with standard for loop?
  std::for_each(
    m_channels.begin(), m_channels.end(),
    [this](ChannelInfo& channel)
    {
      std::vector<VideoInfo> info_v{};

      cpr::Response r = cpr::Get(
        cpr::Url{URL_VALUES.at(SEARCH_URL_INDEX)},
        cpr::Header{
          {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
          {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}},
        cpr::Parameters{
          {PARAM_NAMES.at(PART_INDEX),       PARAM_VALUES.at(SNIPPET_INDEX)},    // snippet
          {PARAM_NAMES.at(KEY_INDEX),        m_authenticator.get_key()},         // key
          {PARAM_NAMES.at(CHAN_ID_INDEX),    channel.id},                        // channel id
          {PARAM_NAMES.at(TYPE_INDEX),       PARAM_VALUES.at(VIDEO_TYPE_INDEX)}, // type
          {PARAM_NAMES.at(ORDER_INDEX),      PARAM_VALUES.at(DATE_VALUE_INDEX)}, // order by
          {PARAM_NAMES.at(MAX_RESULT_INDEX), std::to_string(5)}                  // limit
        }
      );

      m_quota += youtube::QUOTA_LIMIT.at(youtube::SEARCH_LIST_QUOTA_INDEX);

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
              auto datetime = SanitizeJSON(item["snippet"]["publishedAt"].dump());

              info_v.emplace_back(
                VideoInfo{
                  .channel_id  = channel.id,
                  .id          = video_id,
                  .title       = SanitizeOutput(SanitizeJSON(item["snippet"]["title"].dump())),
                  .description = SanitizeOutput(SanitizeJSON(item["snippet"]["description"].dump())),
                  .datetime    = datetime,
                  .time        = to_readable_time(datetime.c_str()),
                  .url         = youtube_id_to_url(video_id)
                }
              );
            }
            catch (const std::exception &e)
            {
              std::string error_message{"Exception was caught: "};
              error_message += e.what();
              log(error_message);
              return false;
            }
          }
        }
      }
      channel.videos = info_v;
    }
  );

  return true;
}

/**
 * fetch_video_stats
 *
 * @param id_string
 * @return std::vector<VideoStats>
 */
std::vector<VideoStats> API::fetch_video_stats(std::string id_string)
{
  using namespace constants;

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

  m_quota += youtube::QUOTA_LIMIT.at(youtube::VIDEO_LIST_QUOTA_INDEX);

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
std::vector<ChannelInfo> API::fetch_youtube_stats()
{
  using namespace constants;
  using json = nlohmann::json;

  if (m_authenticator.is_authenticated() || m_authenticator.FetchToken())
  {
    if (fetch_channel_videos()) {
      for (auto& channel : m_channels) {
        std::string id_string{};

        for (const auto &info : channel.videos) id_string += info.id + ",";

        std::vector<VideoStats> stats = fetch_video_stats(id_string);
        std::size_t             stats_size = stats.size();

        if (stats_size == channel.videos.size())
        {
          for (uint8_t i = 0; i < stats_size; i++)
          {
            channel.videos.at(i).stats = stats.at(i);
          }
        }
      }
    }
  }

  return m_channels;
}

/**
 * fetch_rival_videos
 *
 * @param video
 * @return std::vector<VideoInfo>
 */
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
    }
  );

  m_quota += youtube::QUOTA_LIMIT.at(youtube::SEARCH_LIST_QUOTA_INDEX);

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
          auto datetime = SanitizeJSON(item["snippet"]["publishedAt"].dump());

          VideoInfo info{
              .channel_id  = SanitizeOutput(SanitizeJSON(item["snippet"]["channelId"].dump())),
              .id          = video_id,
              .title       = SanitizeOutput(SanitizeJSON(item["snippet"]["title"].dump())),
              .description = SanitizeOutput(SanitizeJSON(item["snippet"]["description"].dump())),
              .datetime    = datetime,
              .time        = to_readable_time(datetime.c_str()),
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
 * @returns [out] {std::vector<ChannelInfo>}
 */
std::vector<ChannelInfo> API::find_similar_videos(VideoInfo video)
{
  std::vector<ChannelInfo> channels{};
  std::vector<VideoInfo>   videos = fetch_rival_videos(video);

  auto    it     = std::make_move_iterator(videos.begin());
  auto    it_end = std::make_move_iterator(videos.end());
  uint8_t idx{0};

  while (it != it_end) {
    auto chan_it = std::find_if(
      channels.begin(), channels.end(),
      [&it](const ChannelInfo& channel) {
        return channel.id.compare(it->channel_id) == 0;
      }
    );

    if (chan_it != channels.end()) {
      chan_it->videos.emplace_back(std::move((*it)));
    } else {
      std::vector<ChannelInfo> channel_infos = fetch_channel_info(it->channel_id);

      if (!channel_infos.empty()) {
        if (channel_infos.size() > 1) {
          // TODO: Logger should note that this was a strange result
        }

        channels.emplace_back(std::move(channel_infos.front()));
        channels.back().videos.emplace_back(std::move((*it)));
      } else {
        // TODO: Unable to find channel. Strange
      }
    }

    idx++;
    it = std::make_move_iterator(videos.begin() + idx);
  }

  return channels;
}

/**
 * get_videos
 */
std::vector<VideoInfo> API::get_videos()
{
  std::vector<VideoInfo> videos{};

  for (const auto& channel : m_channels) {
    videos.insert(
      videos.end(),
      channel.videos.begin(),
      channel.videos.end()
    );
  }

  return videos;
}

/**
 * has_videos
 *
 * @interface
 */

bool API::has_videos() {
  return !m_videos.empty();
}

/**
 *
 * fetch_google_trends
 *
 * @param   [in]  {std::vector<std::string>} terms
 * @returns [out] {std::vector<GoogleTrend>}
 */
std::vector<GoogleTrend> API::fetch_google_trends(std::vector<std::string> terms) {
  return query_google_trends(terms);
}

/**
 * fetch_videos_by_terms
 *
 * @param terms
 * @return std::vector<VideoInfo>
 */
std::vector<VideoInfo> API::fetch_videos_by_terms(std::vector<std::string> terms) {
  using namespace constants;
  using json = nlohmann::json;

  const char delimiter{'|'};

  std::vector<VideoInfo> info_v{};

  std::string query = std::accumulate(
    std::next(terms.cbegin()),
    terms.cend(),
    terms.front(),
    [&delimiter](const std::string& a, const std::string& b) {
      return a + delimiter + b;
    }
  );

  cpr::Response r = cpr::Get(
    cpr::Url{URL_VALUES.at(SEARCH_URL_INDEX)},
    cpr::Header{
      {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
      {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}},
    cpr::Parameters{
      {PARAM_NAMES.at(PART_INDEX),           PARAM_VALUES.at(SNIPPET_INDEX)},    // snippet
      {PARAM_NAMES.at(KEY_INDEX),            m_authenticator.get_key()},         // key
      {PARAM_NAMES.at(QUERY_INDEX),          query},                             // query terms
      {PARAM_NAMES.at(TYPE_INDEX),           PARAM_VALUES.at(VIDEO_TYPE_INDEX)}, // type
      {PARAM_NAMES.at(ORDER_INDEX),          PARAM_VALUES.at(VIEW_COUNT_INDEX)}, // order by
      {PARAM_NAMES.at(MAX_RESULT_INDEX),     std::to_string(5)}                  // limit
    }
  );

  m_quota += youtube::QUOTA_LIMIT.at(youtube::SEARCH_LIST_QUOTA_INDEX);

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

  return info_v;
}

/**
 * fetch_term_info
 *
 * @param terms
 * @return std::vector<TermInfo>
 */
std::vector<TermInfo> API::fetch_term_info(std::vector<std::string> terms) {
  using namespace constants;
  const char delimiter{','};

  std::vector<TermInfo> metadata_v{};

  if (m_authenticator.is_authenticated() || m_authenticator.FetchToken())
  {
    std::vector<VideoInfo> videos = fetch_videos_by_terms(terms);

    if (!videos.empty()) {
    std::string                                    id_string{};
    std::vector<VideoInfo>::const_iterator it = videos.cbegin();

    id_string.reserve(videos.size() * youtube::YOUTUBE_VIDEO_ID_LENGTH);
    id_string += (*it).id;

    while (++it != videos.cend()) id_string += delimiter + (*it).id;

    std::vector<VideoStats> stats = fetch_video_stats(id_string);

    if (stats.size() == videos.size())
    {
      for (uint8_t i = 0; i < stats.size(); i++)
      {
        videos.at(i).stats = stats.at(i);
      }
    }

    int score{};

    for (const auto& video : videos) {
      if (std::stoi(video.stats.views) > 100)
        score ++;
    }

    for (const auto& term : terms)
      metadata_v.emplace_back(
        TermInfo{
          .term = term, .value = score
        });
    }
  }

  return metadata_v;
}

std::vector<ChannelInfo> API::fetch_channel_info(std::string id_string) {
  using namespace constants;
  using json = nlohmann::json;

  bool  NO_EXCEPTIONS_THROWN{false};

  std::vector<ChannelInfo> info_v{};

  cpr::Response r = cpr::Get(
    cpr::Url{URL_VALUES.at(CHANNELS_URL_INDEX)},
    cpr::Header{
      {HEADER_NAMES.at(ACCEPT_HEADER_INDEX), HEADER_VALUES.at(APP_JSON_INDEX)},
      {HEADER_NAMES.at(AUTH_HEADER_INDEX),   m_authenticator.get_token()}},
    cpr::Parameters{
      {PARAM_NAMES.at(PART_INDEX),           PARAM_VALUES.at(SNIPPET_STATS_INDEX)}, // snippet
      {PARAM_NAMES.at(KEY_INDEX),            m_authenticator.get_key()},            // key
      {PARAM_NAMES.at(ID_NAME_INDEX),        id_string},                            // query term
      {PARAM_NAMES.at(TYPE_INDEX),           PARAM_VALUES.at(VIDEO_TYPE_INDEX)},    // type
      {PARAM_NAMES.at(ORDER_INDEX),          PARAM_VALUES.at(VIEW_COUNT_INDEX)},    // order by
      {PARAM_NAMES.at(MAX_RESULT_INDEX),     std::to_string(5)}                     // limit
    }
  );

  m_quota += youtube::QUOTA_LIMIT.at(youtube::SEARCH_LIST_QUOTA_INDEX);

  json channel_json = json::parse(r.text, nullptr, NO_EXCEPTIONS_THROWN);

  if (!channel_json.is_null() && channel_json.is_object() && channel_json.contains("items")) {
    json items = channel_json["items"];

    // TODO: Combine follower counts with channel info
    if (!items.is_null() && items.is_array()) {
      for (const auto& item : items) {
        info_v.emplace_back(
          ChannelInfo{
            .name          = item["snippet"]["title"],
            .description   = item["snippet"]["description"],
            .created       = item["snippet"]["publishedAt"],
            .thumb_url     = item["snippet"]["thumbnails"]["default"]["url"],
            .stats         = ChannelStats{
                .views       = item["statistics"]["viewCount"],
                .subscribers = item["statistics"]["subscriberCount"],
                .videos      = item["statistics"]["videoCount"]
            },
            .id            = item["id"]
          }
        );
      }
    }
  }

  return info_v;
}
/**
 * get_quota_used
 *
 * @returns [out] {uint32_t}
 */
const uint32_t API::get_quota_used() const {
  return m_quota;
}
