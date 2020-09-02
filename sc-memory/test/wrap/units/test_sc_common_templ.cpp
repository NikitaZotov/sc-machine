/*
* This source file is part of an OSTIS project. For the latest info, see http://ostis.net
* Distributed under the MIT License
* (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
*/

#include "catch2/catch.hpp"

#include "sc-memory/cpp/utils/sc_test.hpp"
#include "sc-memory/cpp/sc_common_templ.hpp"
#include "sc-memory/cpp/sc_keynodes.hpp"

TEST_CASE("sc_ResolveRelationTuple", "[test sc common templ]")
{
 ScMemoryContext ctx(sc_access_lvl_make_min, "sc_ResolveRelationTuple");

  ScAddr const el = ctx.CreateNode(ScType::NodeConst);
  ScAddr const relAddr = ctx.CreateNode(ScType::NodeConstNoRole);

  ScAddr const tuple = sc::ResolveRelationTuple(ctx, el, relAddr);
  {
    ScTemplate templ;
    templ.TripleWithRelation(
          ScType::NodeVarTuple >> "_tuple",
          ScType::EdgeDCommonVar,
          el,
          ScType::EdgeAccessVarPosPerm,
          relAddr);

    ScTemplateSearchResult res;
    REQUIRE(ctx.HelperSearchTemplate(templ, res));
    REQUIRE(res.Size() == 1);
    REQUIRE(tuple == res[0]["_tuple"]);
  }

  ScAddr const tuple2 = sc::ResolveRelationTuple(ctx, el, relAddr);
  REQUIRE(tuple == tuple2);

  ctx.Destroy();
}

TEST_CASE("sc_SetRelationValue", "[test sc common templ]")
{
  ScMemoryContext ctx(sc_access_lvl_make_min, "sc_SetRelationValue");

  ScAddr const el = ctx.CreateNode(ScType::NodeConst);

  SECTION("RewriteValue")
  {
    SUBTEST_START("RewriteValue")
    {
      std::string const value1 = "test_value";
      ScAddr const relAddr1 = ctx.CreateNode(ScType::NodeConstNoRole);
      ScAddr const linkAddr1 = sc::SetRelationValue(ctx, el, relAddr1, value1);

      ScTemplate templ;
      templ.TripleWithRelation(
            el,
            ScType::EdgeDCommonVar,
            ScType::Link >> "_link",
            ScType::EdgeAccessVarPosPerm,
            relAddr1);

      ScLink link(ctx, linkAddr1);

      ScTemplateSearchResult res;
      REQUIRE(ctx.HelperSearchTemplate(templ, res));
      REQUIRE(res.Size() == 1);
      REQUIRE(linkAddr1 == res[0]["_link"]);
      REQUIRE(link.Get<std::string>() == value1);

      // change value
      uint32_t const value2 = 57;
      ScAddr const linkAddr2 = sc::SetRelationValue(ctx, el, relAddr1, value2);
      REQUIRE(linkAddr1 == linkAddr2);
      REQUIRE(link.Get<uint32_t>() == value2);
    }
    SUBTEST_END()
  }

  ctx.Destroy();
}