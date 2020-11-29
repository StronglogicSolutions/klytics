#ifndef __KLYTICS_HPP__
#define __KLYTICS_HPP__

#include <iostream>
#include <ctime>
#include <cpr/cpr.h>
#include <string>

#include <nlohmann/json.hpp>
#include <tabulate/table.hpp>
#include <HTML/HTML.h>

#include "process.hpp"
#include "api/api.hpp"

inline std::string test_html() {
  HTML::Document document("Welcome to HTML");

    document.head() << HTML::Rel("stylesheet", "https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css")
        .integrity("sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T").crossorigin("anonymous");
    document.head() << HTML::Style(".navbar{margin-bottom:20px;}");

    // Body
    document.body().cls("bg-light");

    // Content in a container
    HTML::Div main("container");
    main << HTML::Header1("Welcome to HTML").id("anchor_link_1");
    main << "Text directly in the body.";
    main << HTML::Text("Text directly in the body. ") << HTML::Text("Text directly in the body.") << HTML::Break()
        << HTML::Text("Text directly in the body.");
    main << HTML::Paragraph("This is the way to go for a big text in a multi-line paragraph.");
    main << HTML::Link("Google", "http://google.com").cls("my_style");
    main << (HTML::Paragraph("A paragraph. ").style("font-family:arial")
        << HTML::Text("Text child.") << HTML::Break() << HTML::Text("And more text."));

    main << (HTML::List()
        << (HTML::ListItem("Text item"))
        << (HTML::ListItem() << HTML::Link("Github Link", "http://srombauts.github.io").title("SRombaut's Github home page"))
        << (HTML::ListItem() << (HTML::List()
                << HTML::ListItem("val1")
                << HTML::ListItem("val2"))));

    main << (HTML::Table().cls("table table-hover table-sm")
        << HTML::Caption("Table caption")
        << (HTML::Row() << HTML::ColHeader("A") << HTML::ColHeader("B"))
        << (HTML::Row() << HTML::Col("Cell_11") << HTML::Col("Cell_12"))
        << (HTML::Row() << HTML::Col("Cell_21") << (HTML::Col() << HTML::Link("Wikipedia", "https://www.wikipedia.org/")))
        << (HTML::Row() << HTML::Col("") << HTML::Col("Cell_32")));

    main << HTML::Small("Copyright Sebastien Rombauts @ 2017-2019");

    main << HTML::Link().id("anchor_link_2");

    document << std::move(main);

    // Javascript at the end of the body
    document << HTML::Script("https://code.jquery.com/jquery-3.3.1.slim.min.js")
        .integrity("sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo").crossorigin("anonymous");
    document << HTML::Script("https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js")
        .integrity("sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1").crossorigin("anonymous");
    document << HTML::Script("https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js")
        .integrity("sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM").crossorigin("anonymous");

    return SanitizeOutput(document.toString());
}

namespace klytics {
namespace constants {
const std::string FOLLOWER_APP{"../external/followers/followers.js"};
} // namespace constants

ProcessResult execute(std::string program);

struct FollowerCount {
std::string name;
std::string platform;
std::string value;
};

/**
 * counts_to_html
 *
 * @param
 * @returns
 */
inline std::string counts_to_html(std::vector<FollowerCount> counts) {
  HTML::Document document{"KIQ Analytics"};

  document.addAttribute("lang", "en");
  document.head() << HTML::Meta("utf-8")
                  << HTML::Meta("viewport", "width=device-width, initial-scale=1, shrink-to-fit=no");
  document.head() << HTML::Style(".navbar{margin-bottom:20px;}");
  document.body().cls("follower-counts");

  HTML::Div main{"container"};
  main << HTML::Header1("KIQ Analytics");
  main << HTML::Header2("Follower Counts") << HTML::Break() << HTML::Break();

  HTML::Table table{};
  table.cls("table");
  table << HTML::Caption{"Results"};
  table << (HTML::Row() << HTML::ColHeader("Name") << HTML::ColHeader("Platform") << HTML::ColHeader("Count"));

  for (const auto& count : counts)
    table << (HTML::Row() << HTML::Col(count.name) << HTML::Col(count.platform) << HTML::Col(count.value));

  main     << std::move(table);
  document << std::move(main);

  return SanitizeOutput(document.toString());
}

class ResultInterface {
public:
virtual std::string to_string() = 0;
};

class JSONResult : public ResultInterface {
using counts = std::vector<FollowerCount>;
public:
virtual std::string to_string() override {
  return counts_to_html(m_counts);
}

/**
 * read
 */
bool read(std::string s) {
  using json = nlohmann::json;

  json r_json = json::parse(s);

  if (!r_json.is_null() && r_json.is_object()) {
    auto instagram = r_json["instagram"];
    auto youtube   = r_json["youtube"];

    if (!instagram.is_null() && instagram.is_object()) {
      for (const auto& it : instagram.items()) {
        m_counts.push_back(
          FollowerCount{
            .name     = it.key(),
            .platform = "instagram",
            .value    = it.value()["value"].dump()
          }
        );
      }
    }

    if (!youtube.is_null() && youtube.is_object()) {
      for (const auto& it : youtube.items()) {
      m_counts.push_back(
        FollowerCount{
          .name     = it.key(),
          .platform = "youtube",
          .value    = it.value()["value"].dump()
        }
      );
      }
    }
    return true;
  }
  return false;
}

private:
counts m_counts;
};

/**
 *
 * KLytics - Main class
 *
 */
class KLytics {
public:


/**
 * fetch_follower_count
 * @returns [out] {std::string}
 */
std::string fetch_follower_count() {
  ProcessResult result = execute(constants::FOLLOWER_APP);
  if (result.error) {
    return "Error executing followers app";
  }

  JSONResult json{};
  if (json.read(result.output)) {
    return json.to_string();
  }

  return "Error processing followers app data";
}


/**
 * generate_report
 *
 * @returns [out] {std::string}
 */
std::string generate_report() {
  using namespace tabulate;

  std::string::size_type extra_text_size{102};

  std::string            output, video_stats_output, competitor_stats_output;
  std::string            follower_count = fetch_follower_count();
  std::vector<VideoInfo> videos         = m_api.fetch_youtube_stats();

  if (!videos.empty()) {
    video_stats_output      = table_to_formatted_string(videos_to_table(videos));
    competitor_stats_output = table_to_formatted_string(videos_to_table(m_api.find_similar_videos(videos.front())));
  }

  output.resize(follower_count.size() + video_stats_output.size() + competitor_stats_output.size() + extra_text_size);

  output += "FOLLOWER COUNT\n\n";
  output += follower_count;
  output += "\n\n";
  output += "LATEST VIDEOS\n\n";
  output += video_stats_output;
  output += "\n\n";
  output += "COMPETITOR VIDEOS (based on your most recent video)\n\n";
  output += competitor_stats_output;
  output += "\n\n";
  output += "Thank you!!!";

  return output;
}

/**
 * generate_video_stats_table
 *
 * @returns [out] {std::string}
 */
std::string generate_video_stats_table() {
  return table_to_formatted_string(
    videos_to_table(
      m_api.fetch_youtube_stats()
    )
  );
}

/**
 * fetch_videos
 */
std::vector<VideoInfo> fetch_videos() {
  return m_api.fetch_channel_videos();
}
/**
 * get_youtube_videos
 *
 * @returns [out] {std::vector<VideoInfo}
 */
std::vector<VideoInfo> get_youtube_videos() {
  return m_api.get_videos();
}

private:
API m_api;
};

} // namespace klytics

#endif // __KLYTICS_HPP__
