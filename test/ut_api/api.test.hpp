#ifndef __API_TEST_HPP__
#define __API_TEST_HPP__
#include "gtest/gtest.h"

#include "klytics/api/api.hpp"

std::vector<std::string> GetTestTerms() {
  return std::vector<std::string>{
    "kpop",
    "korean",
    "hangeul"
  };
}

#endif // __API_TEST_HPP__
