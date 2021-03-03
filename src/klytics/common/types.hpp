#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

namespace klytics {

static std::string ParseMediaURL(const std::string& url)
{
  auto pos = url.find_first_of('?');
  if (pos != std::string::npos)
    return url.substr(0, pos);
  return url;
}

class ResultInterface {
public:
virtual ~ResultInterface() {}
virtual bool read(std::string s) = 0;
virtual std::string to_string() = 0;
};

struct IGFeedItem {
uint32_t                 time;
uint32_t                 pk;
std::string              id;
std::string              username;
std::string              content;
std::vector<std::string> media_urls;
};

inline std::vector<std::string> get_media_urls(const nlohmann::json& data)
{
  std::vector<std::string> urls{};

  for (const auto& media_data : data)
  {
    urls.emplace_back(media_data["url"]);
  }

  return urls;
}

class IGFeedJSONResult : public ResultInterface {
public:
virtual ~IGFeedJSONResult() override {}

virtual bool read(std::string s) override {
  using json = nlohmann::json;

  auto items_json = json::parse(s, nullptr, false);

  if (!items_json.is_null() && items_json.is_array())
  {
    for (const auto& item : items_json)
    {
      m_feed_items.emplace_back(
        IGFeedItem{
          .time = GetJSONValue<uint32_t>(item, "taken_at"),
          .pk   = GetJSONValue<uint32_t>(item["user"], "pk"),
          .id   = GetJSONStringValue(item, "id"),
          .username = GetJSONStringValue(item["user"], "username"),
          .content = GetJSONStringValue(item["caption"], "text"),
          .media_urls = get_media_urls(item["image_versions2"]["candidates"])
        }
      );
    }
    return true;
  }
  return false;
}

virtual std::string to_string() override {
  nlohmann::json output_json = nlohmann::json::array();

  for (const auto& item : m_feed_items)
  {
    nlohmann::json item_json{};

    item_json["content"] = item.content;
    item_json["time"] = std::to_string(item.time);
    item_json["pk"] = item.pk;
    item_json["username"] = item.username;
    item_json["id"] = item.id;
    item_json["urls"] = item.media_urls;

    if (item_json.is_structured())
      output_json.emplace_back(
        item_json
      );
    else
      throw std::runtime_error{"invalid json"};
  }

  return output_json.dump();
}

private:
std::vector<IGFeedItem> m_feed_items;
};

} // namespace klytics
