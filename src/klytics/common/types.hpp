#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#include <vector>
#include <string>
#include <unordered_map>

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
double                   view_score;
double                   like_score;
double                   dislike_score;
double                   comment_score;
};

/**
 * VideoInfo struct
 *
 * TODO: consider wrapping as a class with an ID
 */
struct VideoInfo {
std::string              channel_id;
std::string              id;
std::string              title;
std::string              description;
std::string              datetime;
std::string              time;
std::string              url;
VideoStats               stats;

friend std::ostream &operator<<(std::ostream& o, const VideoInfo& v) {
  o << "Channel ID:  " << v.channel_id     << "\n" <<
       "Video ID:    " << v.id             << "\n" <<
       "Title:       " << v.title          << "\n" <<
       "Description: " << v.description    << "\n" <<
       "Datetime:    " << v.datetime       << "\n" <<
       "URL:         " << v.url            << "\n" <<

       "STATISTICS\n" <<

       "➝ Views:         " << v.stats.views      << "\n" <<
       "➝ Likes:         " << v.stats.likes      << "\n" <<
       "➝ Dislikes:      " << v.stats.dislikes   << "\n" <<
       "➝ Comments:      " << v.stats.comments   << "\n" <<
       "➝ View Score:    " << v.stats.view_score << "\n" <<
       "➝ Like Score:    " << v.stats.like_score << "\n" <<
       "➝ Dislike Score: " << v.stats.dislike_score << "\n" <<
       "➝ Comment Score: " << v.stats.comment_score << "\n";

  return o;
}

};

struct Findings {
bool has_videos() { return !videos.empty(); }
std::vector<VideoInfo> videos;
};

struct FollowerCount {
std::string name;
std::string platform;
std::string value;
std::string time;
std::string delta_t;
std::string delta_v;
};

#endif // __TYPES_HPP__
