#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include "klytics/common/types.hpp"

class SecureAPI {
public:
virtual ~SecureAPI() {}

virtual bool is_authenticated() = 0;
virtual bool init()             = 0;
};

class VideoAPI {
public:
virtual ~VideoAPI() {}

virtual std::vector<VideoInfo> get_videos() = 0;
virtual bool                   has_videos() = 0;
};

#endif // __INTERFACE_HPP__
