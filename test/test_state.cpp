#include "doctest.h"
#include <ios>
#include <sstream>

#include "../src/state.h"

TEST_CASE("Can serialize bytes") {
    std::stringstream ss;

    StateSerialize::put_char(ss, 'g');
    StateSerialize::put_char(ss, 'n');
    StateSerialize::put_char(ss, 'o');
    StateSerialize::put_char(ss, 'k');
    StateSerialize::put_char(ss, 'i');
    StateSerialize::put_char(ss, 'n');
    StateSerialize::put_char(ss, '\001');
    StateSerialize::put_char(ss, '\020');
    StateSerialize::put_char(ss, '\070');
    StateSerialize::put_char(ss, '!');

    CHECK(StateSerialize::get_char(ss) == 'g');
    CHECK(StateSerialize::get_char(ss) == 'n');
    CHECK(StateSerialize::get_char(ss) == 'o');
    CHECK(StateSerialize::get_char(ss) == 'k');
    CHECK(StateSerialize::get_char(ss) == 'i');
    CHECK(StateSerialize::get_char(ss) == 'n');
    CHECK(StateSerialize::get_char(ss) == '\001');
    CHECK(StateSerialize::get_char(ss) == '\020');
    CHECK(StateSerialize::get_char(ss) == '\070');
    CHECK(StateSerialize::get_char(ss) == '!');
}

TEST_CASE("Can serialize shorts") {
    std::stringstream ss;

    StateSerialize::put_short(ss, 0x01234);
    StateSerialize::put_short(ss, 123);
    StateSerialize::put_short(ss, 2270);
    StateSerialize::put_short(ss, 1);

    CHECK(StateSerialize::get_short(ss) == 0x01234);
    CHECK(StateSerialize::get_short(ss) == 123);
    CHECK(StateSerialize::get_short(ss) == 2270);
    CHECK(StateSerialize::get_short(ss) == 1);
}

TEST_CASE("Can serialize longs") {
    std::stringstream ss;

    StateSerialize::put_long(ss, 0xc001cafedeadbeef);
    StateSerialize::put_long(ss, 0x1212121212121212);
    StateSerialize::put_long(ss, 0x0123456789abcdef);
    StateSerialize::put_long(ss, 1);
    StateSerialize::put_long(ss, 0x640414);

    CHECK(StateSerialize::get_long(ss) == 0xc001cafedeadbeef);
    CHECK(StateSerialize::get_long(ss) == 0x1212121212121212);
    CHECK(StateSerialize::get_long(ss) == 0x0123456789abcdef);
    CHECK(StateSerialize::get_long(ss) == 1);
    CHECK(StateSerialize::get_long(ss) == 0x640414);
}

TEST_CASE("Can serialize strings") {
    std::stringstream ss;

    StateSerialize::put_string(ss, "aemil");
    StateSerialize::put_string(ss, "To see Infinity in a grain of sand / and a Heaven in a wild flower");
    StateSerialize::put_string(ss, "");

    CHECK(StateSerialize::get_string(ss) == "aemil");
    CHECK(StateSerialize::get_string(ss) == "To see Infinity in a grain of sand / and a Heaven in a wild flower");
    CHECK(StateSerialize::get_string(ss) == "");
}
