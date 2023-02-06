/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include "sc_debug.hpp"
#include "sc_memory.hpp"

#include <algorithm>
#include <utility>

class ScTemplateSearch
{
public:
  ScTemplateSearch(ScTemplate & templ, ScMemoryContext & context, ScAddr const & structure)
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
  {
    std::cout << "START SEARCH" << std::endl;
    PrepareSearch();
  }

  ScTemplateSearch(
      ScTemplate & templ,
      ScMemoryContext & context,
      ScAddr const & structure,
      ScTemplateSearchResultCallback callback,
      ScTemplateSearchResultFilterCallback checkCallback = {})
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
    , m_callback(std::move(callback))
    , m_filterCallback(std::move(checkCallback))
  {
    std::cout << "START SEARCH" << std::endl;
    PrepareSearch();
  }

  ScTemplateSearch(
      ScTemplate & templ,
      ScMemoryContext & context,
      ScAddr const & structure,
      ScTemplateSearchResultCallbackWithRequest callback,
      ScTemplateSearchResultFilterCallback checkCallback = {})
    : m_template(templ)
    , m_context(context)
    , m_structure(structure)
    , m_callbackWithRequest(std::move(callback))
    , m_filterCallback(std::move(checkCallback))
  {
    std::cout << "START SEARCH" << std::endl;
    PrepareSearch();
  }

  using ScTemplateGroupedTriples = ScTemplate::ScTemplateGroupedTriples;
  using ScReplacementTriple = ScAddrTriple;

private:
  /*!
   * Prepares input sc-template to minimize search
   */
  void PrepareSearch()
  {
    //std::cout << "SetUpDependenciesBetweenTriples" << std::endl;
    SetUpDependenciesBetweenTriples();
    //std::cout << "RemoveCycledDependenciesBetweenTriples" << std::endl;
    RemoveCycledDependenciesBetweenTriples();
    //std::cout << "FindConnectivityComponents" << std::endl;
    FindConnectivityComponents();
    //std::cout << "FindTriplesWithMostMinimalArcsForFirstItem" << std::endl;
    FindTriplesWithMostMinimalArcsForFirstItem();
  }

  /*!
   * Find all dependencies between triples. Compares replacement name of each item of the triple
   * with replacement name of each item of the other triple, and if they are equal, then adds
   * dependencies between them.
   * @note All triple items that have valid address must have replacement names to set up dependencies with them.
   */
  void SetUpDependenciesBetweenTriples()
  {
    auto const & AddDependenceFromTripleItemToOtherTriple =
        [this](
            ScTemplateTriple const * triple, ScTemplateItem const & tripleItem, ScTemplateTriple const * otherTriple) {
          std::string const & key = GetKey(triple, tripleItem);

          auto const & found = m_templateItemsNamesToDependedTemplateTriples.find(key);
          if (found == m_templateItemsNamesToDependedTemplateTriples.cend())
            m_templateItemsNamesToDependedTemplateTriples.insert({key, {otherTriple->m_index}});
          else
            found->second.insert(otherTriple->m_index);
        };

    auto const & TryAddDependenceBetweenTriples = [&AddDependenceFromTripleItemToOtherTriple](
                                                      ScTemplateTriple const * triple,
                                                      ScTemplateItem const & tripleItem,
                                                      ScTemplateTriple const * otherTriple,
                                                      ScTemplateItem const & otherTripleItem1,
                                                      ScTemplateItem const & otherTripleItem2,
                                                      ScTemplateItem const & otherTripleItem3) {
      // don't set up dependency with self
      if (triple->m_index == otherTriple->m_index)
        return;

      // don't set up dependency if item of triple has empty replacement name
      if (tripleItem.m_name.empty())
        return;

      // check triple item name with other triple items names and dependencies
      tripleItem.m_name == otherTripleItem1.m_name
          ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
          : (tripleItem.m_name == otherTripleItem2.m_name
                 ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
                 : (tripleItem.m_name == otherTripleItem3.m_name
                        ? AddDependenceFromTripleItemToOtherTriple(triple, tripleItem, otherTriple)
                        : (void)(null_ptr)));
    };

    for (ScTemplateTriple const * triple : m_template.m_templateTriples)
    {
      ScTemplateItem const & item1 = (*triple)[0];
      ScTemplateItem const & item2 = (*triple)[1];
      ScTemplateItem const & item3 = (*triple)[2];

      for (ScTemplateTriple const * otherTriple : m_template.m_templateTriples)
      {
        ScTemplateItem const & otherItem1 = (*otherTriple)[0];
        ScTemplateItem const & otherItem2 = (*otherTriple)[1];
        ScTemplateItem const & otherItem3 = (*otherTriple)[2];

        TryAddDependenceBetweenTriples(triple, item1, otherTriple, otherItem1, otherItem2, otherItem3);
        TryAddDependenceBetweenTriples(triple, item2, otherTriple, otherItem1, otherItem2, otherItem3);
        TryAddDependenceBetweenTriples(triple, item3, otherTriple, otherItem1, otherItem2, otherItem3);
      }
    }
  };

  /*!
   * Finds triples that loop sc-template and eliminates transitions from them
   */
  void RemoveCycledDependenciesBetweenTriples()
  {
    auto const & CheckIfItemIsNodeVarStruct = [this](ScTemplateItem const & item) -> bool {
      auto const & found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
      return found != m_template.m_templateItemsNamesToTypes.cend() && found->second == ScType::NodeVarStruct;
    };

    auto const & faeTriples = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAN];
    auto const & CheckIfItemIsFixedAndOtherEdgeItemIsEdge =
        [&faeTriples](size_t const tripleIdx, ScTemplateItem const & item) -> bool {
      return item.IsAddr() && faeTriples.find(tripleIdx) != faeTriples.cend();
    };

    auto const & UpdateCycledTriples = [this](ScTemplateTriple const * triple, ScTemplateItem const & item) {
      std::string const & key = GetKey(triple, item);

      auto const & dependedTriples = m_templateItemsNamesToDependedTemplateTriples.find(key);
      if (dependedTriples != m_templateItemsNamesToDependedTemplateTriples.cend())
      {
        for (size_t const dependedTripleIdx : dependedTriples->second)
        {
          if (IsTriplesEqual(triple, m_template.m_templateTriples[dependedTripleIdx]))
          {
            m_cycledTemplateTriples.insert(dependedTripleIdx);
          }
        }
      }

      m_cycledTemplateTriples.insert(triple->m_index);
    };

    // save all triples that form cycles
    for (ScTemplateTriple const * triple : m_template.m_templateTriples)
    {
      ScTemplateItem const & item1 = (*triple)[0];

      bool isFound = false;
      if (m_cycledTemplateTriples.find(triple->m_index) == m_cycledTemplateTriples.cend() &&
          (CheckIfItemIsNodeVarStruct(item1) || CheckIfItemIsFixedAndOtherEdgeItemIsEdge(triple->m_index, item1)))
      {
        ScTemplateGroupedTriples checkedTriples;
        FindCycleWithFAATriple(item1, triple, triple, checkedTriples, isFound);
      }

      if (isFound)
      {
        UpdateCycledTriples(triple, item1);
      }
    }

    // remove dependencies with all triples that form cycles
    for (size_t const idx : m_cycledTemplateTriples)
    {
      ScTemplateTriple * triple = m_template.m_templateTriples[idx];
      std::string const & key = GetKey(triple, (*triple)[0]);

      auto const & found = m_templateItemsNamesToDependedTemplateTriples.find(key);
      if (found != m_templateItemsNamesToDependedTemplateTriples.cend())
      {
        for (size_t const otherIdx : m_cycledTemplateTriples)
        {
          found->second.erase(otherIdx);
        }
      }
    }
  };

  void FindCycleWithFAATriple(
      ScTemplateItem const & templateItem,
      ScTemplateTriple const * templateTriple,
      ScTemplateTriple const * templateTripleToFind,
      ScTemplateGroupedTriples checkedTemplateTriples,
      bool & isFound)
  {
    // no iterate more if cycle is found
    if (isFound)
      return;

    auto const & FindCycleWithFAATripleByTripleItem = [this, &templateTripleToFind, &checkedTemplateTriples](
                                                          ScTemplateItem const & item,
                                                          ScTemplateTriple const * triple,
                                                          ScTemplateItem const & previousItem,
                                                          bool & isFound) {
      // no iterate back by the same item name
      if (!item.m_name.empty() && item.m_name == previousItem.m_name)
        return;

      // no iterate back by the same item address
      if (item.m_addrValue.IsValid() && item.m_addrValue == previousItem.m_addrValue)
        return;

      FindCycleWithFAATriple(item, triple, templateTripleToFind, checkedTemplateTriples, isFound);
    };

    ScTemplateGroupedTriples nextTemplateTriples;
    FindDependedTriple(templateItem, templateTriple, nextTemplateTriples);

    for (size_t const otherTemplateTripleIdx : nextTemplateTriples)
    {
      ScTemplateTriple const * otherTriple = m_template.m_templateTriples[otherTemplateTripleIdx];

      if ((otherTemplateTripleIdx == templateTripleToFind->m_index &&
           templateItem.m_name != (*templateTripleToFind)[0].m_name) ||
          isFound)
      {
        isFound = true;
        break;
      }

      // check if triple was passed in branch of sc-template
      if (checkedTemplateTriples.find(otherTemplateTripleIdx) != checkedTemplateTriples.cend())
        continue;

      // iterate by all triple items
      {
        checkedTemplateTriples.insert(otherTemplateTripleIdx);

        FindCycleWithFAATripleByTripleItem((*otherTriple)[0], otherTriple, templateItem, isFound);
        FindCycleWithFAATripleByTripleItem((*otherTriple)[1], otherTriple, templateItem, isFound);
        FindCycleWithFAATripleByTripleItem((*otherTriple)[2], otherTriple, templateItem, isFound);
      }
    }
  }

  void FindConnectivityComponents()
  {
    ScTemplateGroupedTriples checkedTriples;

    for (ScTemplateTriple const * triple : m_template.m_templateTriples)
    {
      ScTemplateGroupedTriples connectivityComponentTriples;
      FindConnectivityComponent(triple, checkedTriples, connectivityComponentTriples);

      m_connectivityComponentsTemplateTriples.push_back(connectivityComponentTriples);
    }
  }

  void FindConnectivityComponent(
      ScTemplateTriple const * templateTriple,
      ScTemplateGroupedTriples & checkedTemplateTriples,
      ScTemplateGroupedTriples & connectivityComponentTemplateTriples)
  {
    // check if triple was passed in branch of sc-template
    if (checkedTemplateTriples.find(templateTriple->m_index) != checkedTemplateTriples.cend())
      return;

    connectivityComponentTemplateTriples.insert(templateTriple->m_index);

    FindConnectivityComponentByItem(
        (*templateTriple)[0], templateTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
    FindConnectivityComponentByItem(
        (*templateTriple)[1], templateTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
    FindConnectivityComponentByItem(
        (*templateTriple)[2], templateTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
  }

  void FindConnectivityComponentByItem(
      ScTemplateItem const & templateItem,
      ScTemplateTriple const * templateTriple,
      ScTemplateGroupedTriples & checkedTemplateTriples,
      ScTemplateGroupedTriples & connectivityComponentTemplateTriples)
  {
    ScTemplateGroupedTriples nextTriples;
    FindDependedTriple(templateItem, templateTriple, nextTriples);

    for (size_t const otherTripleIdx : nextTriples)
    {
      // check if triple was passed in branch of sc-template
      if (checkedTemplateTriples.find(otherTripleIdx) != checkedTemplateTriples.cend())
        continue;

      // iterate by all triple items
      {
        checkedTemplateTriples.insert(otherTripleIdx);
        connectivityComponentTemplateTriples.insert(otherTripleIdx);

        ScTemplateTriple const * otherTriple = m_template.m_templateTriples[otherTripleIdx];

        FindConnectivityComponentByItem(
            (*otherTriple)[0], otherTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
        FindConnectivityComponentByItem(
            (*otherTriple)[1], otherTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
        FindConnectivityComponentByItem(
            (*otherTriple)[2], otherTriple, checkedTemplateTriples, connectivityComponentTemplateTriples);
      }
    }
  }

  /*!
   * Finds all connectivity component triples among all triples that have the fixed first item, but not fixed
   * other items, for which the minimum number of arcs goes/incomes out of the first item compared to the other triples.
   */
  void FindTriplesWithMostMinimalArcsForFirstItem()
  {
    for (ScTemplateGroupedTriples const & connectivityComponentsTriples : m_connectivityComponentsTemplateTriples)
    {
      sc_int32 priorityTripleIdx = -1;
      auto & afaTriples = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::AFA];
      if (!afaTriples.empty())
      {
        priorityTripleIdx = (sc_int32)*afaTriples.cbegin();
      }

      if (priorityTripleIdx == -1)
      {
        priorityTripleIdx = FindTripleWithMostMinimalInputArcsForFirstItem(connectivityComponentsTriples);
        if (priorityTripleIdx == -1)
        {
          priorityTripleIdx = FindTripleWithMostMinimalOutputArcsForFirstItem(connectivityComponentsTriples);
        }
      }

      // save triple in which the first item address has the most minimal count of input/output arcs in vector
      // with more priority
      if (priorityTripleIdx != -1)
      {
        m_connectivityComponentPriorityTemplateTriples.emplace_back(priorityTripleIdx);
      }
    }
  }

  sc_int32 FindTripleWithMostMinimalInputArcsForFirstItem(
      ScTemplateGroupedTriples const & connectivityComponentsTriples)
  {
    auto triplesWithConstEndElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAF];
    if (triplesWithConstEndElement.empty())
    {
      triplesWithConstEndElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::AAF];
    }

    // find triple in which the third item address has the most minimal count of input arcs
    sc_int32 priorityTripleIdx = -1;
    sc_int32 minInputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstEndElement)
    {
      ScTemplateTriple const * triple = m_template.m_templateTriples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementInputArcsCount(triple->GetValues()[2].m_addrValue);

      // check if triple in connectivity component
      if (connectivityComponentsTriples.find(tripleIdx) == connectivityComponentsTriples.cend())
        continue;

      if (minInputArcsCount == -1 || count < minInputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minInputArcsCount = (sc_int32)count;
      }
    }

    return priorityTripleIdx;
  }

  sc_int32 FindTripleWithMostMinimalOutputArcsForFirstItem(
      ScTemplateGroupedTriples const & connectivityComponentsTriples)
  {
    auto triplesWithConstBeginElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAE];
    // if there are no triples with the no edge third item than sort triples with the edge third item
    if (triplesWithConstBeginElement.empty())
    {
      triplesWithConstBeginElement = m_template.m_priorityOrderedTemplateTriples[(size_t)ScTemplateTripleType::FAN];
    }

    // find triple in which the first item address has the most minimal count of output arcs
    sc_int32 priorityTripleIdx = -1;
    sc_int32 minOutputArcsCount = -1;
    for (size_t const tripleIdx : triplesWithConstBeginElement)
    {
      ScTemplateTriple const * triple = m_template.m_templateTriples[tripleIdx];
      auto const count = (sc_int32)m_context.GetElementOutputArcsCount(triple->GetValues()[0].m_addrValue);

      // check if triple in connectivity component
      if (connectivityComponentsTriples.find(tripleIdx) == connectivityComponentsTriples.cend())
        continue;

      if (minOutputArcsCount == -1 || count < minOutputArcsCount)
      {
        priorityTripleIdx = (sc_int32)tripleIdx;
        minOutputArcsCount = (sc_int32)count;
      }
    }

    return priorityTripleIdx;
  }

  //! Returns key - "${item replacement name}${triple index}"
  static std::string GetKey(ScTemplateTriple const * triple, ScTemplateItem const & item)
  {
    std::ostringstream stream;
    stream << item.m_name << "_" << triple->m_index;
    return stream.str();
  }

  void FindDependedTriple(
      ScTemplateItem const & item,
      ScTemplateTriple const * triple,
      ScTemplateGroupedTriples & nextTriples)
  {
    if (item.m_name.empty())
      return;

    std::string const & key = GetKey(triple, item);
    auto const & found = m_templateItemsNamesToDependedTemplateTriples.find(key);
    if (found != m_templateItemsNamesToDependedTemplateTriples.cend())
    {
      nextTriples = found->second;
    }
  }

  bool IsTriplesEqual(
      ScTemplateTriple const * templateTriple,
      ScTemplateTriple const * otherTemplateTriple,
      std::string const & itemName = "")
  {
    if (templateTriple->m_index == otherTemplateTriple->m_index)
      return true;

    auto const & tripleValues = templateTriple->GetValues();
    auto const & otherTripleValues = otherTemplateTriple->GetValues();

    auto const & IsTriplesItemsEqual = [this](ScTemplateItem const & item, ScTemplateItem const & otherItem) -> bool {
      bool isEqual = item.m_typeValue == otherItem.m_typeValue;
      if (!isEqual)
      {
        auto found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
        if (found == m_template.m_templateItemsNamesToTypes.cend())
        {
          found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
          if (found != m_template.m_templateItemsNamesToTypes.cend())
            isEqual = item.m_typeValue == found->second;
        }
        else
          isEqual = found->second == otherItem.m_typeValue;
      }

      if (isEqual)
        isEqual = item.m_addrValue == otherItem.m_addrValue;

      if (!isEqual)
      {
        auto found = m_template.m_templateItemsNamesToReplacementItemsAddrs.find(item.m_name);
        if (found == m_template.m_templateItemsNamesToReplacementItemsAddrs.cend())
        {
          found = m_template.m_templateItemsNamesToReplacementItemsAddrs.find(item.m_name);
          if (found != m_template.m_templateItemsNamesToReplacementItemsAddrs.cend())
            isEqual = item.m_addrValue == found->second;
        }
        else
          isEqual = found->second == otherItem.m_addrValue;
      }

      return isEqual;
    };

    return IsTriplesItemsEqual(tripleValues[0], otherTripleValues[0]) &&
           IsTriplesItemsEqual(tripleValues[1], otherTripleValues[1]) &&
           IsTriplesItemsEqual(tripleValues[2], otherTripleValues[2]) &&
           ((tripleValues[0].m_name == otherTripleValues[0].m_name &&
             (itemName.empty() || otherTripleValues[0].m_name == itemName)) ||
            (tripleValues[2].m_name == otherTripleValues[2].m_name &&
             (itemName.empty() || otherTripleValues[0].m_name == itemName)));
  };

  inline bool IsStructureValid()
  {
    return m_structure.IsValid();
  }

  inline bool IsInStructure(ScAddr const & addr)
  {
    return m_context.HelperCheckEdge(m_structure, addr, ScType::EdgeAccessConstPosPerm);
  }

  ScAddr const & ResolveAddr(
      ScTemplateItem const & templateItem,
      ScAddrVector const & replacementConstruction,
      ScTemplateSearchResult & result) const
  {
    auto const & GetItemAddrInReplacements = [&replacementConstruction,
                                              &result](ScTemplateItem const & item) -> ScAddr const & {
      auto const & it = result.m_templateItemsNamesToReplacementItemsPositions.equal_range(item.m_name);
      for (auto curIt = it.first; curIt != it.second; ++curIt)
      {
        ScAddr const & addr = replacementConstruction[curIt->second];
        if (addr.IsValid())
          return addr;
      }

      return ScAddr::Empty;
    };

    switch (templateItem.m_itemType)
    {
    case ScTemplateItem::Type::Addr:
    {
      return templateItem.m_addrValue;
    }

    case ScTemplateItem::Type::Replace:
    {
      ScAddr const & replacementAddr = GetItemAddrInReplacements(templateItem);
      if (replacementAddr.IsValid())
        return replacementAddr;

      auto const & addrsIt = m_template.m_templateItemsNamesToReplacementItemsAddrs.find(templateItem.m_name);
      if (addrsIt != m_template.m_templateItemsNamesToReplacementItemsAddrs.cend())
        return addrsIt->second;

      return ScAddr::Empty;
    }

    case ScTemplateItem::Type::Type:
    {
      if (!templateItem.m_name.empty())
      {
        return GetItemAddrInReplacements(templateItem);
      }
    }

    default:
    {
      return ScAddr::Empty;
    }
    }
  }

  ScIterator3Ptr CreateIterator(
      ScTemplateTriple const * templateTriple,
      ScAddrVector const & replacementConstruction,
      ScTemplateSearchResult & result)
  {
    ScTemplateItem const & item1 = (*templateTriple)[0];
    ScTemplateItem const & item2 = (*templateTriple)[1];
    ScTemplateItem const & item3 = (*templateTriple)[2];

    ScAddr const & addr1 = ResolveAddr(item1, replacementConstruction, result);
    ScAddr const & addr2 = ResolveAddr(item2, replacementConstruction, result);
    ScAddr const & addr3 = ResolveAddr(item3, replacementConstruction, result);

    auto const & PrepareType = [this](ScTemplateItem const & item) -> ScType {
      ScType type = item.m_typeValue;
      auto const & found = m_template.m_templateItemsNamesToTypes.find(item.m_name);
      if (found != m_template.m_templateItemsNamesToTypes.cend())
      {
        type = found->second;
      }

      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (addr1.IsValid())
    {
      if (!addr2.IsValid())
      {
        if (addr3.IsValid())  // F_A_F
        {
          return m_context.Iterator3(addr1, PrepareType(item2.m_typeValue), addr3);
        }
        else  // F_A_A
        {
          return m_context.Iterator3(addr1, PrepareType(item2.m_typeValue), PrepareType(item3.m_typeValue));
        }
      }
      else
      {
        if (addr3.IsValid())  // F_F_F
        {
          return m_context.Iterator3(addr1, addr2, addr3);
        }
        else  // F_F_A
        {
          return m_context.Iterator3(addr1, addr2, PrepareType(item3.m_typeValue));
        }
      }
    }
    else if (addr3.IsValid())
    {
      if (addr2.IsValid())  // A_F_F
      {
        return m_context.Iterator3(PrepareType(item1.m_typeValue), addr2, addr3);
      }
      else  // A_A_F
      {
        return m_context.Iterator3(PrepareType(item1.m_typeValue), PrepareType(item2.m_typeValue), addr3);
      }
    }
    else if (addr2.IsValid() && !addr3.IsValid())  // A_F_A
    {
      return m_context.Iterator3(PrepareType(item1.m_typeValue), addr2, PrepareType(item3.m_typeValue));
    }

    return {};
  }

  using UsedEdges = std::unordered_set<ScAddr, ScAddrHashFunc<uint32_t>>;

  void DoIterationOnNextEqualTriples(
      ScTemplateGroupedTriples const & templateTriples,
      std::string const & templateItemName,
      size_t const resultIdx,
      ScTemplateGroupedTriples const & currentIterableTemplateTriples,
      ScTemplateSearchResult & result,
      bool & isFinished,
      bool & isLast)
  {
    isLast = true;
    isFinished = true;

    std::unordered_set<size_t> iteratedTemplateTriples;
    for (size_t const idx : templateTriples)
    {
      ScTemplateTriple * triple = m_template.m_templateTriples[idx];
      if (iteratedTemplateTriples.find(triple->m_index) != iteratedTemplateTriples.cend())
        continue;

      ScTemplateGroupedTriples equalTemplateTriples;
      for (ScTemplateTriple * otherTemplateTriple : m_template.m_templateTriples)
      {
        // check if iterable triple is equal to current, not checked and not iterable with previous
        auto const & checkedTemplateTriples = m_checkedTemplateTriplesInReplacementConstructions[resultIdx];
        if (checkedTemplateTriples.find(otherTemplateTriple->m_index) == checkedTemplateTriples.cend() &&
            std::find_if(
                currentIterableTemplateTriples.cbegin(),
                currentIterableTemplateTriples.cend(),
                [&](size_t const otherIdx) {
                  return idx == otherIdx;
                }) == currentIterableTemplateTriples.cend() &&
            IsTriplesEqual(triple, otherTemplateTriple, templateItemName))
        {
          equalTemplateTriples.insert(otherTemplateTriple->m_index);
          iteratedTemplateTriples.insert(otherTemplateTriple->m_index);
        }
      }

      if (!equalTemplateTriples.empty())
      {
        isLast = false;
        DoDependenceIteration(equalTemplateTriples, resultIdx, result);

        isFinished =
            std::all_of(equalTemplateTriples.begin(), equalTemplateTriples.end(), [this, resultIdx](size_t const idx) {
              return m_checkedTemplateTriplesInReplacementConstructions[resultIdx].find(idx) !=
                     m_checkedTemplateTriplesInReplacementConstructions[resultIdx].cend();
            });

        if (!isFinished)
        {
          break;
        }
      }
    }
  }

  void DoDependenceIteration(
      ScTemplateGroupedTriples const & templateTriples,
      size_t replacementConstructionIdx,
      ScTemplateSearchResult & result)
  {
    auto const & UpdateResultByItem =
        [&result](
            ScTemplateItem const & item, ScAddr const & addr, size_t const elementNum, ScAddrVector & resultAddrs) {
          resultAddrs[elementNum] = addr;

          if (item.m_name.empty())
            return;

          result.m_templateItemsNamesToReplacementItemsPositions.insert({item.m_name, elementNum});
        };

    auto const & UpdateResult = [this, &UpdateResultByItem, &result](
                                    ScTemplateTriple * templateTriple,
                                    size_t const replacementConstructionIdx,
                                    ScAddrTriple const & replacementTriple) {
      m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].insert(templateTriple->m_index);
      m_usedEdgesInReplacementConstructions[replacementConstructionIdx].insert(replacementTriple[1]);

      size_t itemIdx = templateTriple->m_index * 3;

      for (size_t i = replacementConstructionIdx; i < result.Size(); ++i)
      {
        ScAddrVector & resultAddrs = result.m_replacementConstructions[i];

        UpdateResultByItem((*templateTriple)[0], replacementTriple[0], itemIdx, resultAddrs);
        UpdateResultByItem((*templateTriple)[1], replacementTriple[1], itemIdx + 1, resultAddrs);
        UpdateResultByItem((*templateTriple)[2], replacementTriple[2], itemIdx + 2, resultAddrs);
      }
    };

    auto const & ClearResult = [this](
                                   size_t const tripleIdx,
                                   size_t const replacementConstructionIdx,
                                   ScAddrVector & replacementConstruction) {
      m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].erase(tripleIdx);

      size_t itemIdx = tripleIdx * 3;

      replacementConstruction[itemIdx] = ScAddr::Empty;
      m_usedEdgesInReplacementConstructions[replacementConstructionIdx].erase(replacementConstruction[++itemIdx]);
      m_notUsedEdgesInTemplateTriples[tripleIdx].insert(replacementConstruction[itemIdx]);
      replacementConstruction[itemIdx] = ScAddr::Empty;
      replacementConstruction[++itemIdx] = ScAddr::Empty;
    };

    ScTemplateTriple * templateTriple = m_template.m_templateTriples[*templateTriples.begin()];

    bool isForLastTemplateTripleAllChildrenFinished = true;
    bool isTemplateTripleHasNoChildren = false;

    ScIterator3Ptr it =
        CreateIterator(templateTriple, result.m_replacementConstructions[replacementConstructionIdx], result);
    if (!it || !it->IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "During search procedure has been chosen var triple");
    }

//    std::cout << "Try [" << templateTriples.size() << "][" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//              << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//              << (*templateTriple)[2].m_name << "}" << std::endl;

    ScAddrVector nextResultReplacementTriples{result.m_replacementConstructions[replacementConstructionIdx]};
    ScTemplateGroupedTriples nextCheckedTemplateTriples{
        m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx]};
    UsedEdges nextUsedReplacementEdges{m_usedEdgesInReplacementConstructions[replacementConstructionIdx]};

    size_t checkedCurrentResultEqualTemplateTriplesCount = 0;

    bool isReplacementTriplesIteratorNext = false;
    ScReplacementTriple replacementTriple;

    bool isTemplateTriplesIteratorNext = false;
    ScTemplateGroupedTriples::const_iterator templateTriplesIterator;
    do
    {
      isReplacementTriplesIteratorNext = it->Next();
      if (isReplacementTriplesIteratorNext)
      {
        replacementTriple = it->Get();
      }
      else
      {
        break;
      }

      // check triple elements by structure belonging or predicate callback
      if ((IsStructureValid() && (!IsInStructure(replacementTriple[0]) || !IsInStructure(replacementTriple[1]) ||
                                  !IsInStructure(replacementTriple[2]))) ||
          (m_filterCallback && !m_filterCallback(replacementTriple[0], replacementTriple[1], replacementTriple[2])))
      {
        m_usedEdgesInReplacementConstructions[replacementConstructionIdx].insert(replacementTriple[1]);
        continue;
      }

      // check if edge is used for other equal triple
      if (m_usedEdgesInReplacementConstructions[replacementConstructionIdx].find(replacementTriple[1]) != m_usedEdgesInReplacementConstructions[replacementConstructionIdx].cend())
      {
//        std::cout << "Used [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                  << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                  << (*templateTriple)[2].m_name << "}" << std::endl;
        continue;
      }

      if (m_notUsedEdgesInTemplateTriples[templateTriple->m_index].find(replacementTriple[1]) != m_notUsedEdgesInTemplateTriples[templateTriple->m_index].cend())
      {
//        std::cout << "Not used [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                  << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                  << (*templateTriple)[2].m_name << "}" << std::endl;
        continue;
      }

      do
      {
        if (!isTemplateTriplesIteratorNext)
        {
          templateTriplesIterator = templateTriples.cbegin();
          isTemplateTriplesIteratorNext = true;
        }
        else if (templateTriplesIterator != templateTriples.cend())
        {
          ++templateTriplesIterator;
        }

        // check if all equal triples found to make a new search result item
        if (checkedCurrentResultEqualTemplateTriplesCount == templateTriples.size())
        {
//          std::cout << "Next [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                    << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                    << (*templateTriple)[2].m_name << "}" << std::endl;
          replacementConstructionIdx = ++m_lastReplacementConstructionIdx;
          checkedCurrentResultEqualTemplateTriplesCount = 0;

          if (replacementConstructionIdx >= DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount)
          {
            ++m_resultReserveCount;
            result.m_replacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount);
            m_checkedTemplateTriplesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount);
            m_usedEdgesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE * m_resultReserveCount);
          }

          result.m_replacementConstructions.emplace_back(nextResultReplacementTriples);
          m_checkedTemplateTriplesInReplacementConstructions.emplace_back(nextCheckedTemplateTriples);
          m_usedEdgesInReplacementConstructions.emplace_back(nextUsedReplacementEdges);

          templateTriplesIterator = templateTriples.cbegin();
        }

        if (!isForLastTemplateTripleAllChildrenFinished)
        {
//          std::cout << "Release [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                    << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                    << (*templateTriple)[2].m_name << "}" << std::endl;
          result.m_replacementConstructions[replacementConstructionIdx].assign(nextResultReplacementTriples.cbegin(), nextResultReplacementTriples.cend());
          m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx] = nextCheckedTemplateTriples;
          m_usedEdgesInReplacementConstructions[replacementConstructionIdx] = nextUsedReplacementEdges;
        }

        if (templateTriplesIterator == templateTriples.cend())
        {
          isTemplateTriplesIteratorNext = false;
          break;
        }

        size_t const tripleIdx = *templateTriplesIterator;

        templateTriple = m_template.m_templateTriples[tripleIdx];

        if (m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].find(tripleIdx) !=
            m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].cend())
        {
//          std::cout << "Checked [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                    << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                    << (*templateTriple)[2].m_name << "}" << std::endl;
          continue;
        }

        ScAddrVector & replacementConstruction = result.m_replacementConstructions[replacementConstructionIdx];

        if (templateTriple->AnyOf(
                [this, &replacementConstruction, &result, &replacementTriple](
                    ScTemplateItem const & item, size_t const index) -> bool {
                  ScAddr const & resolvedAddr = ResolveAddr(item, replacementConstruction, result);
                  return resolvedAddr.IsValid() && resolvedAddr != replacementTriple[index];
                }))
        {
          isForLastTemplateTripleAllChildrenFinished = false;
          continue;
        }

//        std::cout << "Iterate [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                  << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                  << (*templateTriple)[2].m_name << "---" << (*templateTriple)[2].m_typeValue << "}" << std::endl;


        // update data
        {
          UpdateResult(templateTriple, replacementConstructionIdx, replacementTriple);
        }

        // find next depended on triples and analyse result
        {
          isForLastTemplateTripleAllChildrenFinished = true;
          isTemplateTripleHasNoChildren = true;
          // first of all check triples by edge, it is more effectively
          if (templateTriple->AnyOf(
                  [this,
                   &templateTriple,
                   &replacementConstructionIdx,
                   &templateTriples,
                   &isForLastTemplateTripleAllChildrenFinished,
                   &isTemplateTripleHasNoChildren,
                   &result](ScTemplateItem const & item, size_t const index) -> bool {
                    bool isChildFinished = false;
                    bool isNoChild = false;
                    ScTemplateGroupedTriples nextTemplateTriples;
                    FindDependedTriple(item, templateTriple, nextTemplateTriples);

                    DoIterationOnNextEqualTriples(
                        nextTemplateTriples,
                        item.m_name,
                        replacementConstructionIdx,
                        templateTriples,
                        result,
                        isChildFinished,
                        isNoChild);

                    isForLastTemplateTripleAllChildrenFinished &= isChildFinished;
                    isTemplateTripleHasNoChildren &= isNoChild;
                    return !isForLastTemplateTripleAllChildrenFinished && !isTemplateTripleHasNoChildren;
                  }))
          {
//            std::cout << "Clear [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                      << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                      << (*templateTriple)[2].m_name << "}" << std::endl;

            ClearResult(tripleIdx, replacementConstructionIdx, replacementConstruction);
            continue;
          }

          // all connected triples found
          if (isForLastTemplateTripleAllChildrenFinished)
          {
            ++checkedCurrentResultEqualTemplateTriplesCount;

//            std::cout << "Found [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
//                      << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
//                      << (*templateTriple)[2].m_name << "}" << std::endl;

            // current edge is busy for all equal triples
            m_usedEdgesInReplacementConstructions[replacementConstructionIdx].insert(replacementTriple[1]);

            break;
          }
        }

        ++templateTriplesIterator;
      } while (templateTriplesIterator != templateTriples.cend());

      // there are no next triples for current triple, it is last
      if (isTemplateTripleHasNoChildren && isForLastTemplateTripleAllChildrenFinished &&
          m_checkedTemplateTriplesInReplacementConstructions[replacementConstructionIdx].size() ==
              m_template.m_templateTriples.size())
      {
        std::cout << "Append [" << replacementConstructionIdx << "][" << templateTriple->m_index << "] = {"
            << (*templateTriple)[0].m_name << "} --{" << (*templateTriple)[1].m_name << "}--> {"
                  << (*templateTriple)[2].m_name << "}" << std::endl;

        AppendFoundReplacementConstruction(result, replacementConstructionIdx);
      }
    } while (isReplacementTriplesIteratorNext && !isStopped);
  }

  void AppendFoundReplacementConstruction(ScTemplateSearchResult & result, size_t & resultIdx)
  {
    if (m_callback)
    {
      m_callback(ScTemplateSearchResultItem(
          &result.m_replacementConstructions[resultIdx], &result.m_templateItemsNamesToReplacementItemsPositions));
    }
    else if (m_callbackWithRequest)
    {
      ScTemplateSearchRequest const & request = m_callbackWithRequest(ScTemplateSearchResultItem(
          &result.m_replacementConstructions[resultIdx], &result.m_templateItemsNamesToReplacementItemsPositions));
      switch (request)
      {
      case ScTemplateSearchRequest::STOP:
      {
        isStopped = true;
        break;
      }
      case ScTemplateSearchRequest::ERROR:
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Requested error state during search");
      }
      default:
        break;
      }
    }
    else
    {
      m_foundReplacementConstructions.insert(resultIdx);
    }
  }

  void DoIterations(ScTemplateSearchResult & result)
  {
    if (m_template.m_templateTriples.empty())
      return;

    ScAddrVector newResult;
    newResult.resize(CalculateOneResultSize());
    result.m_replacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE);
    result.m_replacementConstructions.emplace_back(newResult);

    m_notUsedEdgesInTemplateTriples.resize(m_template.m_templateTriples.size());
    m_usedEdgesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE);
    m_usedEdgesInReplacementConstructions.emplace_back();
    m_checkedTemplateTriplesInReplacementConstructions.reserve(DEFAULT_RESULT_RESERVE_SIZE);
    m_checkedTemplateTriplesInReplacementConstructions.emplace_back();

    bool isFinished = false;
    bool isLast = false;

    for (size_t const tripleIdx : m_connectivityComponentPriorityTemplateTriples)
    {
      DoIterationOnNextEqualTriples({tripleIdx}, "", 0, {}, result, isFinished, isLast);
    }
  }

public:
  ScTemplate::Result operator()(ScTemplateSearchResult & result)
  {
    result.Clear();

    DoIterations(result);

    std::vector<ScAddrVector> checkedResults;
    checkedResults.reserve(result.Size());
    for (size_t const foundIdx : m_foundReplacementConstructions)
    {
      checkedResults.emplace_back(result.m_replacementConstructions[foundIdx]);
    }
    result.m_replacementConstructions.assign(checkedResults.cbegin(), checkedResults.cend());

    std::cout << "END SEARCH" << std::endl;

    return ScTemplate::Result(result.Size() > 0);
  }

  void operator()()
  {
    ScTemplateSearchResult result;

    DoIterations(result);

    std::cout << "END SEARCH" << std::endl;
  }

  size_t CalculateOneResultSize() const
  {
    return m_template.m_templateTriples.size() * 3;
  }

private:
  ScTemplate & m_template;
  ScMemoryContext & m_context;

  bool isStopped = false;

  ScAddr const m_structure;
  ScTemplateSearchResultCallback m_callback;
  ScTemplateSearchResultCallbackWithRequest m_callbackWithRequest;
  ScTemplateSearchResultFilterCallback m_filterCallback;

  std::map<std::string, ScTemplateGroupedTriples> m_templateItemsNamesToDependedTemplateTriples;
  ScTemplateGroupedTriples m_cycledTemplateTriples;
  std::vector<ScTemplateGroupedTriples> m_connectivityComponentsTemplateTriples;
  std::vector<size_t> m_connectivityComponentPriorityTemplateTriples;

  std::vector<UsedEdges> m_notUsedEdgesInTemplateTriples;
  std::vector<UsedEdges> m_usedEdgesInReplacementConstructions;
  std::vector<std::unordered_set<size_t>> m_checkedTemplateTriplesInReplacementConstructions;

  size_t const DEFAULT_RESULT_RESERVE_SIZE = 512;
  size_t m_resultReserveCount = 1;
  size_t m_lastReplacementConstructionIdx = 0;
  std::unordered_set<size_t> m_foundReplacementConstructions;
};

ScTemplate::Result ScTemplate::Search(ScMemoryContext & ctx, ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty);
  return search(result);
}

void ScTemplate::Search(
    ScMemoryContext & ctx,
    ScTemplateSearchResultCallback const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty, callback, filterCallback);
  search();
}

void ScTemplate::Search(
    ScMemoryContext & ctx,
    ScTemplateSearchResultCallbackWithRequest const & callback,
    ScTemplateSearchResultFilterCallback const & filterCallback) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, ScAddr::Empty, callback, filterCallback);
  search();
}

ScTemplate::Result ScTemplate::SearchInStruct(
    ScMemoryContext & ctx,
    ScAddr const & scStruct,
    ScTemplateSearchResult & result) const
{
  ScTemplateSearch search(const_cast<ScTemplate &>(*this), ctx, scStruct);
  return search(result);
}
