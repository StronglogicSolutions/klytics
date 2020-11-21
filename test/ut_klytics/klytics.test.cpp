#include "klytics.test.hpp"

TEST(KLyticsTest, Instantiate) {
  EXPECT_NO_THROW(klytics::KLytics{});
}

TEST(KLyticsTest, DISABLED_FindComparableVideosFromCompetitor) {

  klytics::KLytics klytics{};

  klytics.fetch_video_stats();

  std::vector<VideoInfo> videos       = klytics.get_youtube_videos();
  std::vector<VideoInfo> rival_videos = klytics.find_similar_videos(videos.front());
  tabulate::Table        table        = videos_to_table(rival_videos);

}


TEST(KLyticsTest, FunctionFormatsStringsWithLineBreaks) {

  std::string input_s_1{"Learn KOREAN with KPOP 마마무 (MAMAMOO) - AYA Learn KOREAN with KPOP 마마무 (MAMAMOO) - AYA Learn KOREAN with KPOP 마마무 (MAMAMOO) - AYA Learn KOREAN with KPOP 마마무 (MAMAMOO) - AYA  "};

  std::string input_s_2{" Learn KOREAN with KPOP TWICE – &#39;I CAN&#39;T STOP ME’  Learn KOREAN with KPOP TWICE – &#39;I CAN&#39;T STOP ME’  Learn KOREAN with KPOP TWICE – &#39;I CAN&#39;T STOP ME’ "};

  tabulate::Table table{};

  table.add_row({"ID", "Value"});

  std::string result_s_1 = CreateStringWithBreaks(input_s_1, 30);
  std::string result_s_2 = CreateStringWithBreaks(input_s_2, 30);


  table.add_row({"1", result_s_1});
  table.add_row({"2", result_s_2});

  std::cout << table << std::endl;
}

