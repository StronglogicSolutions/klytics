#include "api.test.hpp"

/**
 * Instantiate
 */
TEST(APITest, Instantiate) {
  EXPECT_NO_THROW(ktube::YouTubeDataAPI{});
}

/**
 * ComputeYouTubeKeywordScore
 */
TEST(APITest, ComputeYouTubeKeywordScore) {
  ktube::YouTubeDataAPI api{};

  std::vector<ktube::TermInfo> term_info_v =
  api.fetch_term_info(
    api.fetch_youtube_stats().front().videos.front().get_primary_keywords()
  );

  for (const auto& info : term_info_v)
    std::cout << "Term:  " << info.term << "\nScore: " << info.value << "\n" << std::endl;

  std::cout << "Used " << api.get_quota_used() << " quota units" << std::endl;

  EXPECT_FALSE(term_info_v.empty());
}

TEST(APITest, FetchChannelInfo) {
  ktube::YouTubeDataAPI api{};

  std::vector<ktube::ChannelInfo> channel_v = api.fetch_channel_info(
    std::string{
      ktube::constants::CHANNEL_IDS.at(ktube::constants::KSTYLEYO_CHANNEL_ID_INDEX) + "," +
      ktube::constants::CHANNEL_IDS.at(ktube::constants::STRONGLOGICSOLUTIONS_CHANNEL_ID_INDEX)
    }
  );

  for (const auto& channel : channel_v) {
    std::cout << channel << std::endl;
  }

  EXPECT_FALSE(channel_v.empty());
}