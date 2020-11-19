#include "klytics.test.hpp"

TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

TEST(KLyticsTest, FindComparableVideoFromCompetitor) {
  klytics::KLytics klytics{};

  klytics.fetch_video_stats();

  auto videos = klytics.get_youtube_videos();

  EXPECT_FALSE(videos.empty());

  auto rival_videos = klytics.get_rival_videos(videos.front());

  EXPECT_FALSE(rival_videos.empty());

  for (const auto& video : rival_videos) {
    std::cout << video.title << std::endl;
  }
}

