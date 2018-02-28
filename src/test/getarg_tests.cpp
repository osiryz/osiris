#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-OSR");
    BOOST_CHECK(GetBoolArg("-OSR"));
    BOOST_CHECK(GetBoolArg("-OSR", false));
    BOOST_CHECK(GetBoolArg("-OSR", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-OSRo"));
    BOOST_CHECK(!GetBoolArg("-OSRo", false));
    BOOST_CHECK(GetBoolArg("-OSRo", true));

    ResetArgs("-OSR=0");
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", false));
    BOOST_CHECK(!GetBoolArg("-OSR", true));

    ResetArgs("-OSR=1");
    BOOST_CHECK(GetBoolArg("-OSR"));
    BOOST_CHECK(GetBoolArg("-OSR", false));
    BOOST_CHECK(GetBoolArg("-OSR", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noOSR");
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", false));
    BOOST_CHECK(!GetBoolArg("-OSR", true));

    ResetArgs("-noOSR=1");
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", false));
    BOOST_CHECK(!GetBoolArg("-OSR", true));

    ResetArgs("-OSR -noOSR");  // -OSR should win
    BOOST_CHECK(GetBoolArg("-OSR"));
    BOOST_CHECK(GetBoolArg("-OSR", false));
    BOOST_CHECK(GetBoolArg("-OSR", true));

    ResetArgs("-OSR=1 -noOSR=1");  // -OSR should win
    BOOST_CHECK(GetBoolArg("-OSR"));
    BOOST_CHECK(GetBoolArg("-OSR", false));
    BOOST_CHECK(GetBoolArg("-OSR", true));

    ResetArgs("-OSR=0 -noOSR=0");  // -OSR should win
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", false));
    BOOST_CHECK(!GetBoolArg("-OSR", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--OSR=1");
    BOOST_CHECK(GetBoolArg("-OSR"));
    BOOST_CHECK(GetBoolArg("-OSR", false));
    BOOST_CHECK(GetBoolArg("-OSR", true));

    ResetArgs("--noOSR=1");
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", false));
    BOOST_CHECK(!GetBoolArg("-OSR", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OSR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OSR", "eleven"), "eleven");

    ResetArgs("-OSR -bar");
    BOOST_CHECK_EQUAL(GetArg("-OSR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OSR", "eleven"), "");

    ResetArgs("-OSR=");
    BOOST_CHECK_EQUAL(GetArg("-OSR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OSR", "eleven"), "");

    ResetArgs("-OSR=11");
    BOOST_CHECK_EQUAL(GetArg("-OSR", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-OSR", "eleven"), "11");

    ResetArgs("-OSR=eleven");
    BOOST_CHECK_EQUAL(GetArg("-OSR", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-OSR", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OSR", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-OSR", 0), 0);

    ResetArgs("-OSR -bar");
    BOOST_CHECK_EQUAL(GetArg("-OSR", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-OSR=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-OSR", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-OSR=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-OSR", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--OSR");
    BOOST_CHECK_EQUAL(GetBoolArg("-OSR"), true);

    ResetArgs("--OSR=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-OSR", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noOSR");
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", true));
    BOOST_CHECK(!GetBoolArg("-OSR", false));

    ResetArgs("-noOSR=1");
    BOOST_CHECK(!GetBoolArg("-OSR"));
    BOOST_CHECK(!GetBoolArg("-OSR", true));
    BOOST_CHECK(!GetBoolArg("-OSR", false));

    ResetArgs("-noOSR=0");
    BOOST_CHECK(GetBoolArg("-OSR"));
    BOOST_CHECK(GetBoolArg("-OSR", true));
    BOOST_CHECK(GetBoolArg("-OSR", false));

    ResetArgs("-OSR --noOSR");
    BOOST_CHECK(GetBoolArg("-OSR"));

    ResetArgs("-noOSR -OSR"); // OSR always wins:
    BOOST_CHECK(GetBoolArg("-OSR"));
}

BOOST_AUTO_TEST_SUITE_END()
