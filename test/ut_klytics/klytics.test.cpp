#include "klytics.test.hpp"

TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

/**

*/
TEST(KLyticsTest, ComparatorComparesVideoVectors) {
  using namespace klytics;
  using VideoAnalysis = VideoAnalyst::VideoAnalysis;
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

  VideoCreatorComparison  comparison_result = klytics.get_findings();
  VideoAnalysis           analysis          = comparison_result.get_result();
  ResultMap               result_map        = analysis.get_result_map();

  EXPECT_TRUE(all_videos_added);
  EXPECT_FALSE(result_map.empty());
  EXPECT_FALSE(analysis.most_likes_index.empty());
  EXPECT_FALSE(analysis.most_dislikes_index.empty());
  EXPECT_FALSE(analysis.best_view_score_index.empty());
}