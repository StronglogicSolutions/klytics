#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <ctime>
#include <iomanip>
#include <ctype.h>

/**
 * Poor man's log
 *
 * @param
 */
template<typename T>
inline void log(T s) {
  std::cout << s << std::endl;
}

inline std::string SanitizeOutput(const std::string& s) {
  std::string o{};

  for (const char& c : s) {
    if (c == '\'')
      o += "\\'";
    else
    if (c == '"')
      o += "\\\"";
    else
    if (c == '(')
      o += "&#x28;";
    else
    if (c == ')')
      o += "&#x29;";
    else
      o += c;
  }

  return o;
}

/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param   [in] {std::string}
 * @returns [in] {std::string}
 */
inline std::string SanitizeJSON(std::string s) {
  s.erase(
    std::remove(s.begin(), s.end(),'\"'),
    s.end()
  );
  return s;
}

/**
 * SanitizeInput
 *
 * Helper function to remove quotes from a string
 *
 * @param   [in] {std::string}
 * @returns [in] {std::string}
 */
inline std::string SanitizeInput(std::string s) {
  s.erase(
    std::remove_if(s.begin(), s.end(), [](char c){
      return c == '\'' || c == '\"';
    }),
  s.end());

  return s;
}

/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param [in] {std::string&} A reference to a string object
 */
inline std::string StripLineBreaks(std::string s) {
  s.erase(
    std::remove(s.begin(), s.end(),'\n'),
    s.end()
  );

  return s;
}

/**
 * CreateStringWithBreaks
 *
 * @param
 * @param
 * @returns
 */
inline std::string CreateStringWithBreaks(const std::string &in, const size_t every_n) {
  std::string out{};
  out.reserve(in.size() + in.size() / every_n);
  for(std::string::size_type i = 0; i < in.size(); i++) {
    (isascii(static_cast<uint8_t>(in[i]))) ?
    (!(i % every_n) && i) ? out.push_back('\n') : out.push_back(in[i]) :
    (void)(out);
  }
  return out;
}

/**
 * to_unixtime
 *
 * @param
 * @returns
 */
inline const std::time_t to_unixtime(const char* datetime) {
  std::tm            t{};
  std::istringstream ss{datetime};

  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  return mktime(&t);
}

/**
 * to_readable_time
 *
 * @param
 * @returns
 */
inline std::string to_readable_time(const char* datetime) {
  uint8_t            buffer_size{24};
  std::tm            t{};
  std::istringstream ss{datetime};
  char               b[buffer_size];
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  strftime(b, buffer_size, "%B %d %H:%M:%S", &t);

  return std::string{b};
}

#endif // __UTIL_HPP__