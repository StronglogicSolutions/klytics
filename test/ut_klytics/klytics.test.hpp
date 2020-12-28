#ifndef __KLYTICS_TEST_HPP__
#define __KLYTICS_TEST_HPP__
#include "gtest/gtest.h"

#include "klytics/klytics.hpp"

const std::string TEST_CHANNEL_ID_1{"chanID1"};
const std::string TEST_CHANNEL_ID_2{"chanID2"};
const std::string TEST_VIDEO_ID_1{"vID1"};
const std::string TEST_VIDEO_ID_2{"vID2"};
const std::string TEST_VIDEO_ID_3{"vID3"};
const std::string TEST_VIDEO_ID_4{"vID4"};
const std::string TEST_VIDEO_ID_5{"vID5"};
const std::string TEST_VIDEO_ID_6{"vID6"};
const std::string TEST_TITLE_1{"Video Title 1"};
const std::string TEST_TITLE_2{"Video Title 2"};
const std::string TEST_TITLE_3{"Video Title 3"};
const std::string TEST_TITLE_4{"Video Title 4"};
const std::string TEST_TITLE_5{"Video Title 5"};
const std::string TEST_TITLE_6{"Video Title 6"};
const std::string TEST_DESCRIPTION_1{"Video Description 1"};
const std::string TEST_DESCRIPTION_2{"Video Description 2"};
const std::string TEST_DESCRIPTION_3{"Video Description 3"};
const std::string TEST_DESCRIPTION_4{"Video Description 4"};
const std::string TEST_DESCRIPTION_5{"Video Description 5"};
const std::string TEST_DESCRIPTION_6{"Video Description 6"};
const std::string TEST_URL{"http://url.com"};
const std::string TEST_DATETIME       = get_simple_datetime();
const std::string TEST_HUMAN_DATETIME = to_readable_time(TEST_DATETIME.c_str());
const std::string TEST_VIEWS_LOW{"9"};
const std::string TEST_VIEWS_MEDIUM{"999"};
const std::string TEST_VIEWS_HIGH{"999999"};
const std::string TEST_LIKES_LOW{"1"};
const std::string TEST_LIKES_MEDIUM{"111"};
const std::string TEST_LIKES_HIGH{"111111"};
const std::string TEST_DISLIKES_LOW{"6"};
const std::string TEST_DISLIKES_MEDIUM{"666"};
const std::string TEST_DISLIKES_HIGH{"666666"};
const std::string TEST_COMMENTS_LOW{"3"};
const std::string TEST_COMMENTS_MEDIUM{"333"};
const std::string TEST_COMMENTS_HIGH{"333333"};
const std::vector<std::string> TEST_KEYWORDS{
  "test", "testing", "unittesting", "klytics", "kiq"
};

std::vector<VideoInfo> GetTestVideos_1() {
  return std::vector<VideoInfo>{
    VideoInfo{
      .channel_id  = TEST_CHANNEL_ID_1,
      .id          = TEST_VIDEO_ID_1,
      .title       = TEST_TITLE_1,
      .description = TEST_DESCRIPTION_1,
      .datetime    = TEST_DATETIME,
      .time        = TEST_HUMAN_DATETIME,
      .url         = TEST_URL,
      .stats       = VideoStats{
        .views       = TEST_VIEWS_LOW,
        .likes       = TEST_LIKES_LOW,
        .dislikes    = TEST_DISLIKES_LOW,
        .comments    = TEST_COMMENTS_LOW,
        .keywords    = TEST_KEYWORDS
      }
    },
    VideoInfo{
      .channel_id  = TEST_CHANNEL_ID_1,
      .id          = TEST_VIDEO_ID_2,
      .title       = TEST_TITLE_2,
      .description = TEST_DESCRIPTION_2,
      .datetime    = TEST_DATETIME,
      .time        = TEST_HUMAN_DATETIME,
      .url         = TEST_URL,
      .stats       = VideoStats{
        .views       = TEST_VIEWS_MEDIUM,
        .likes       = TEST_LIKES_MEDIUM,
        .dislikes    = TEST_DISLIKES_MEDIUM,
        .comments    = TEST_COMMENTS_MEDIUM,
        .keywords    = TEST_KEYWORDS
      }
    },
    VideoInfo{
      .channel_id  = TEST_CHANNEL_ID_1,
      .id          = TEST_VIDEO_ID_3,
      .title       = TEST_TITLE_3,
      .description = TEST_DESCRIPTION_3,
      .datetime    = TEST_DATETIME,
      .time        = TEST_HUMAN_DATETIME,
      .url         = TEST_URL,
      .stats       = VideoStats{
        .views       = TEST_VIEWS_HIGH,
        .likes       = TEST_LIKES_HIGH,
        .dislikes    = TEST_DISLIKES_HIGH,
        .comments    = TEST_COMMENTS_HIGH,
        .keywords    = TEST_KEYWORDS
      }
    }
  };
}

std::vector<VideoInfo> GetTestVideos_2() {
  return std::vector<VideoInfo>{
    VideoInfo{
      .channel_id  = TEST_CHANNEL_ID_2,
      .id          = TEST_VIDEO_ID_4,
      .title       = TEST_TITLE_4,
      .description = TEST_DESCRIPTION_4,
      .datetime    = TEST_DATETIME,
      .time        = TEST_HUMAN_DATETIME,
      .url         = TEST_URL,
      .stats       = VideoStats{
        .views       = TEST_VIEWS_LOW,
        .likes       = TEST_LIKES_LOW,
        .dislikes    = TEST_DISLIKES_LOW,
        .comments    = TEST_COMMENTS_LOW,
        .keywords    = TEST_KEYWORDS
      }
    },
    VideoInfo{
      .channel_id  = TEST_CHANNEL_ID_2,
      .id          = TEST_VIDEO_ID_5,
      .title       = TEST_TITLE_5,
      .description = TEST_DESCRIPTION_5,
      .datetime    = TEST_DATETIME,
      .time        = TEST_HUMAN_DATETIME,
      .url         = TEST_URL,
      .stats       = VideoStats{
        .views       = TEST_VIEWS_MEDIUM,
        .likes       = TEST_LIKES_MEDIUM,
        .dislikes    = TEST_DISLIKES_MEDIUM,
        .comments    = TEST_COMMENTS_MEDIUM,
        .keywords    = TEST_KEYWORDS
      }
    },
    VideoInfo{
      .channel_id  = TEST_CHANNEL_ID_2,
      .id          = TEST_VIDEO_ID_6,
      .title       = TEST_TITLE_6,
      .description = TEST_DESCRIPTION_6,
      .datetime    = TEST_DATETIME,
      .time        = TEST_HUMAN_DATETIME,
      .url         = TEST_URL,
      .stats       = VideoStats{
        .views       = TEST_VIEWS_HIGH,
        .likes       = TEST_LIKES_HIGH,
        .dislikes    = TEST_DISLIKES_HIGH,
        .comments    = TEST_COMMENTS_HIGH,
        .keywords    = TEST_KEYWORDS
      }
    }
  };
}

#endif // __KLYTICS_TEST_HPP__