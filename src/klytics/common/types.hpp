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
};

struct VideoInfo {
std::string              channel_id;
std::string              id;
std::string              title;
std::string              description;
std::string              datetime;
std::string              time;
std::string              url;
VideoStats               stats;
};

struct Findings {
bool has_videos() { return !videos.empty(); }
std::vector<VideoInfo> videos;
};

using VideoMap = std::unordered_map<std::string, std::vector<VideoInfo>>;

#endif // __TYPES_HPP__
