#include "doctest.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <string>

#include "../src/generators.h"

TEST_CASE("Generated names are 5 letters") {
    std::vector<std::string> names;
    for (int i = 0; i < 1000; i++) {
        names.push_back(Generators::generate_yokin_name(5));
    }

    CHECK(std::all_of(begin(names), end(names), [](const std::string &s) {
        return s.length() == 5;
    }));
}
