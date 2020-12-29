#include "klytics.test.hpp"

TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

/**

*/
TEST(KLyticsTest, ComparatorComparesVideoVectors) {
  using namespace klytics;

  bool all_videos_added{true};
  KLytics klytics{};

  std::vector<std::vector<VideoInfo>> videos{
    GetTestVideos_1(),
    GetTestVideos_2()
  };

  for (const auto& video : videos) {
    if (!klytics.add_videos(video)) {
      all_videos_added = false;
    }
  }

  VideoCreatorComparison comparison_result = klytics.get_findings();

  EXPECT_TRUE(all_videos_added);

  VideoCreatorComparison::ResultMap result_map = comparison_result.get_result();

  EXPECT_FALSE(result_map.empty());
}