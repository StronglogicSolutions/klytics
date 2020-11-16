#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <ctime>
#include <iomanip>

/**
 * Poor man's log
 */
template<typename T>
inline void log(T s) {
  std::cout << s << std::endl;
}

/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param [in] {std::string&} A reference to a string object
 */
inline void SanitizeJSON(std::string& s) {
  s.erase(
    std::remove(s.begin(), s.end(),'\"'),
    s.end()
  );
}

/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param [in] {std::string&} A reference to a string object
 */
inline void StripLineBreaks(std::string& s) {
  s.erase(
    std::remove(s.begin(), s.end(),'\n'),
    s.end()
  );
}

inline std::string CreateStringWithBreaks(const std::string &in, const size_t every_n) {
  std::string out;
  out.reserve(in.size() + in.size() / every_n);
  for(std::string::size_type i = 0; i < in.size(); i++) {
      if (!(i % every_n) && i) {
          out.push_back('\n');
      }
      out.push_back(in[i]);
  }
  return out;
}


inline const std::time_t to_unixtime(const char* datetime) {
  std::tm            t{};
  std::istringstream ss{datetime};

  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  return mktime(&t);
}

inline std::string to_readable_time(const char* datetime) {
  std::tm            t{};
  std::istringstream ss{datetime};
  char               b[64];
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  strftime(b, 64, "%B %d %H:%M:%S", &t);

  return std::string{b};
}


#endif // __UTIL_HPP__