/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <string>
#include <set>
#include <unordered_map>

extern "C"
{
#include <sc-core/sc_types.h>
}

#include "sc_defines.hpp"

class ScMemoryContext;
class ScAddr;

namespace scs
{
class Parser;
}  // namespace scs

class _SC_EXTERN ScType
{
  friend class ScMemoryContext;
  template <class TScEvent>
  friend class ScElementaryEventSubscription;
  friend class scs::Parser;
  friend class ScMemoryGenerateElementsJsonAction;
  friend class ScMemoryHandleKeynodesJsonAction;
  friend class ScMemoryMakeTemplateJsonAction;
  friend struct ScTypeHashFunc;

public:
  using RealType = sc_type;

  // All sc-types must be calculating in compile time!
  explicit constexpr ScType()
    : m_realType(sc_type_unknown)
  {
  }

  ~ScType();

  /*!
   * @brief Checks if the current ScType instance represents an connector (edge).
   * @return Returns true if the ScType is a connector (edge), false otherwise.
   */
  SC_DEPRECATED(
      0.10.0,
      "This method is deprecated. Use `IsConnector` instead for better readability and standards "
      "compliance.")
  bool IsEdge() const;

  /*!
   * @brief Checks if the current ScType instance is a connector.
   * @return Returns true if the ScType is a connector, false otherwise.
   */
  bool IsConnector() const;

  /*!
   * @brief Checks if the current ScType instance is a common edge.
   * @return Returns true if the ScType is a common edge, false otherwise.
   */
  bool IsCommonEdge() const;

  /*!
   * @brief Checks if the current ScType instance represents an arc.
   * @return Returns true if the ScType is an arc, false otherwise.
   */
  bool IsArc() const;

  /*!
   * @brief Checks if the current ScType instance is a common arc.
   * @return Returns true if the ScType is a common arc, false otherwise.
   */
  bool IsCommonArc() const;

  /*!
   * @brief Checks if the current ScType instance represents a membership arc.
   * @return Returns true if the ScType is a membership arc, false otherwise.
   */
  bool IsMembershipArc() const;

  /*!
   * @brief Checks if the current ScType instance represents a node.
   * @return Returns true if the ScType is a node, false otherwise.
   */
  bool IsNode() const;

  /*!
   * @brief Checks if the current ScType instance represents a link.
   * @return Returns true if the ScType is a link (node link), false otherwise.
   */
  bool IsLink() const;

  /*!
   * @brief Checks if the current ScType instance has constant subtype.
   * @return Returns true if the ScType has constant characteristics, false otherwise.
   */
  bool IsConst() const;

  /*!
   * @brief Checks if the current ScType instance has variable subtype.
   * @return Returns true if the ScType has variable characteristics, false otherwise.
   */
  bool IsVar() const;

  /*!
   * @brief Checks if the current ScType instance is of unknown type.
   * @return Returns true if m_realType is 0, indicating an unknown type; false otherwise.
   */
  bool IsUnknown() const;

  /*!
   * @brief Checks if the current ScType instance has a constancy subtype set.
   * @return Returns true if there is a constancy subtype in m_realType, false otherwise.
   */
  bool HasConstancyFlag() const;

  /*!
   * @brief Converts this instance to a constant type by modifying its subtype bits.
   * @return A new ScType instance representing the constant version of this type.
   */
  ScType AsConst() const;

  /*!
   * @brief Removes variable subtype from this type and add constant subtype to this type (currently returns AsConst).
   * @return A new constant version of this type.
   */
  ScType UpConstType() const;

  /*!
   * @brief Retrieves the underlying real type value of this instance.
   * @return A real type represented by m_realType.
   */
  sc_type operator*() const;

  /*!
   * @brief Updates this instance's real type with additional bits using bitwise OR.
   * @param bits A bits to be added to m_realType.
   * @return A reference to this updated ScType instance.
   */
  ScType & operator()(RealType bits);

  /*!
   * @brief Compares two ScTypes for equality.
   * @param other A other ScType to compare against.
   * @return Returns true if both instances have equal real types; false otherwise.
   */
  bool operator==(ScType const & other);

  /*!
   * @brief Compares two ScTypes for inequality.
   * @param other A other ScType to compare against.
   * @return Returns true if instances have different real types; false otherwise.
   */
  bool operator!=(ScType const & other);

  /*!
   * @brief Performs a bitwise AND operation with another mask and returns the result.
   * @param inMask A mask to perform AND operation with.
   * @return A result of m_realType AND inMask.
   */
  RealType BitAnd(RealType const & inMask) const;

  /*!
   * @brief Performs a bitwise AND operation with another ScType and returns a new instance.
   * @param other A other ScType to combine with using AND.
   * @return A new ScType representing the result of m_realType AND other.m_realType.
   */
  ScType operator&(ScType const & other) const;

  /*!
   * @brief Updates this instance's real type using bitwise AND with another instance's real type.
   * @param other A other sc-type to combine with using AND.
   * @return A reference to this updated sc-type instance.
   */
  ScType & operator&=(ScType const & other);

  /*!
   * @brief Converts this sc-type to its underlying real type representation.
   * @return An underlying real type value of this sc-type.
   */
  operator RealType() const;

  /*!
   * @brief Converts this sc-type to string name.
   * @return A string name of sc-type.
   */
  operator std::string() const;

  /*!
   * @brief Gets direct designation of current sc-connector sc-type in SCs-code.
   * @return A designation of type.
   */
  std::string GetDirectSCsConnector() const;

  /*!
   * @brief Gets reverse designation of current sc-connector sc-type in SCs-code.
   * @return A designation of type.
   */
  std::string GetReverseSCsConnector() const;

  /*!
   * @brief Gets system identifier of class for current sc-type that can be used to specify types of sc.s-elements.
   * @return A system identifier of class for current sc-type.
   */
  std::string GetSCsElementKeynode() const;

  /*!
   * @brief Determines whether this sc-type can be extended to another sc-type based on semantic and subtype
   * compatibility.
   * @param extType Type A sc-type that may extend this one.
   * @return True if extension is possible based on semantic and subtype checks; false otherwise.
   */
  bool CanExtendTo(ScType const & extType) const;

  //! Returns a set of sc-connector sc-types.
  static std::set<ScType> GetConnectorTypes();

private:
  RealType m_realType;

protected:
  struct ScTypeHashFunc;
  static std::unordered_map<ScType, std::string, ScTypeHashFunc> const m_typesToNames;

  // All sc-types must be calculating in compile time!
  constexpr ScType(RealType type) noexcept
    : m_realType(type)
  {
  }

  /*!
   * @brief Performs a bitwise OR operation with another ScType and returns a new instance.
   * @param other An other sc-type to combine with using OR.
   * @return A new sc-type representing the result of m_realType OR other.m_realType.
   */
  ScType operator|(ScType const & other);

  /*!
   * @brief Updates this instance's real type using bitwise OR with another instance's real type.
   * @param other A other sc-type to combine with using OR.
   * @return A reference to this updated sc-type instance.
   */
  ScType & operator|=(ScType const & other);

public:
  static ScType const Unknown;

  // sc-elements
  static ScType const Node;
  static ScType const Connector;
  static ScType const CommonEdge;
  static ScType const Arc;
  static ScType const CommonArc;
  static ScType const MembershipArc;

  // constancy
  static ScType const Const;
  static ScType const Var;

  static ScType const ConstNode;
  static ScType const VarNode;
  static ScType const ConstConnector;
  static ScType const VarConnector;
  static ScType const ConstCommonEdge;
  static ScType const VarCommonEdge;
  static ScType const ConstArc;
  static ScType const VarArc;
  static ScType const ConstCommonArc;
  static ScType const VarCommonArc;
  static ScType const ConstMembershipArc;
  static ScType const VarMembershipArc;

  // permanency
  static ScType const PermArc;
  static ScType const TempArc;

  static ScType const ConstPermArc;
  static ScType const VarPermArc;
  static ScType const ConstTempArc;
  static ScType const VarTempArc;

  // actuality
  static ScType const ActualTempArc;
  static ScType const InactualTempArc;

  static ScType const ConstActualTempArc;
  static ScType const VarActualTempArc;
  static ScType const ConstInactualTempArc;
  static ScType const VarInactualTempArc;

  // positivity
  static ScType const PosArc;
  static ScType const NegArc;
  static ScType const FuzArc;

  // positive sc-arcs
  static ScType const ConstPosArc;
  static ScType const VarPosArc;

  static ScType const PermPosArc;
  static ScType const TempPosArc;
  static ScType const ActualTempPosArc;
  static ScType const InactualTempPosArc;

  static ScType const ConstPermPosArc;
  static ScType const ConstTempPosArc;
  static ScType const ConstActualTempPosArc;
  static ScType const ConstInactualTempPosArc;

  static ScType const VarPermPosArc;
  static ScType const VarTempPosArc;
  static ScType const VarActualTempPosArc;
  static ScType const VarInactualTempPosArc;

  // negative sc-arcs
  static ScType const ConstNegArc;
  static ScType const VarNegArc;

  static ScType const PermNegArc;
  static ScType const TempNegArc;
  static ScType const ActualTempNegArc;
  static ScType const InactualTempNegArc;

  static ScType const ConstPermNegArc;
  static ScType const ConstTempNegArc;
  static ScType const ConstActualTempNegArc;
  static ScType const ConstInactualTempNegArc;

  static ScType const VarPermNegArc;
  static ScType const VarTempNegArc;
  static ScType const VarActualTempNegArc;
  static ScType const VarInactualTempNegArc;

  // fuzzy sc-arcs
  static ScType const ConstFuzArc;
  static ScType const VarFuzArc;

  // semantic sc-node types
  static ScType const NodeLink;
  static ScType const NodeLinkClass;
  static ScType const NodeTuple;
  static ScType const NodeStructure;
  static ScType const NodeRole;
  static ScType const NodeNonRole;
  static ScType const NodeClass;
  static ScType const NodeSuperclass;
  static ScType const NodeMaterial;

  static ScType const ConstNodeLink;
  static ScType const ConstNodeLinkClass;
  static ScType const ConstNodeTuple;
  static ScType const ConstNodeStructure;
  static ScType const ConstNodeRole;
  static ScType const ConstNodeNonRole;
  static ScType const ConstNodeClass;
  static ScType const ConstNodeSuperclass;
  static ScType const ConstNodeMaterial;

  static ScType const VarNodeLink;
  static ScType const VarNodeLinkClass;
  static ScType const VarNodeTuple;
  static ScType const VarNodeStructure;
  static ScType const VarNodeRole;
  static ScType const VarNodeNonRole;
  static ScType const VarNodeClass;
  static ScType const VarNodeSuperclass;
  static ScType const VarNodeMaterial;

  SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_BEGIN
  // deprecated types
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::CommonEdge` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeUCommon;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::CommonArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeDCommon;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstCommonEdge` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeUCommonConst;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstCommonArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeDCommonConst;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::MembershipArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccess;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstPermPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstPosPerm;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstPermNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstNegPerm;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstFuzPerm;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstTempPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstPosTemp;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstTempNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstNegTemp;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessConstFuzTemp;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarCommonEdge` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeUCommonVar;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarCommonArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeDCommonVar;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarPermPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarPosPerm;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarPermNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarNegPerm;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarFuzPerm;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarTempPosArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarPosTemp;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarTempNegArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarNegTemp;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarFuzArc` instead for better readability and standards "
      "compliance.")
  static ScType const EdgeAccessVarFuzTemp;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNode` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConst;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNode` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVar;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::NodeLink` instead for better readability and standards "
      "compliance.")
  static ScType const Link;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::NodeLinkClass` instead for better readability and standards "
      "compliance.")
  static ScType const LinkClass;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::NodeStructure` instead for better readability and standards "
      "compliance.")
  static ScType const NodeStruct;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeLink` instead for better readability and standards "
      "compliance.")
  static ScType const LinkConst;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeLinkClass` instead for better readability and standards "
      "compliance.")
  static ScType const LinkConstClass;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeStructure` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstStruct;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeTuple` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstTuple;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstRole;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeNonRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstNoRole;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeClass` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstClass;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::ConstNodeMaterial` instead for better readability and standards "
      "compliance.")
  static ScType const NodeConstMaterial;

  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeLink` instead for better readability and standards "
      "compliance.")
  static ScType const LinkVar;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeLinkClass` instead for better readability and standards "
      "compliance.")
  static ScType const LinkVarClass;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeStructure` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarStruct;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeTuple` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarTuple;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarRole;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeNonRole` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarNoRole;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeClass` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarClass;
  SC_DEPRECATED(
      0.10.0,
      "This object is deprecated. Use `ScType::VarNodeMaterial` instead for better readability and standards "
      "compliance.")
  static ScType const NodeVarMaterial;
  SC_PRAGMA_DISABLE_DEPRECATION_WARNINGS_END
};
