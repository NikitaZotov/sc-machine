/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

namespace scAgentsCommon
{

ScKeynodeClass const CoreKeynodes::question("question");
ScKeynodeClass const CoreKeynodes::question_initiated("question_initiated");
ScKeynodeClass const CoreKeynodes::question_finished("question_finished");
ScKeynodeClass const CoreKeynodes::question_finished_successfully("question_finished_successfully");
ScKeynodeClass const CoreKeynodes::question_finished_unsuccessfully("question_finished_unsuccessfully");
ScKeynodeNoRole const CoreKeynodes::nrel_answer("nrel_answer");
ScKeynodeNoRole const CoreKeynodes::nrel_inclusion("nrel_inclusion");
ScKeynodeNoRole const CoreKeynodes::nrel_idtf("nrel_idtf");
ScKeynodeNoRole const CoreKeynodes::nrel_main_idtf("nrel_main_idtf");
ScKeynodeNoRole const CoreKeynodes::nrel_lexicographically_more("nrel_lexicographically_more");
ScKeynodeNoRole const CoreKeynodes::nrel_lexicographically_less("nrel_lexicographically_less");
ScKeynodeNoRole const CoreKeynodes::nrel_equal_strings("nrel_equal_strings");
ScKeynodeRole const CoreKeynodes::rrel_1("rrel_1");
ScKeynodeRole const CoreKeynodes::rrel_2("rrel_2");
ScKeynodeRole const CoreKeynodes::rrel_3("rrel_3");
ScKeynodeRole const CoreKeynodes::rrel_last_added_sc_element("rrel_last_added_sc_element");
ScKeynodeClass const CoreKeynodes::number("number");
ScKeynodeNoRole const CoreKeynodes::nrel_equal_numbers("nrel_equal_numbers");
ScKeynodeNoRole const CoreKeynodes::nrel_be_more("nrel_be_more");
ScKeynodeRole const CoreKeynodes::rrel_key_sc_element("rrel_key_sc_element");
ScKeynodeNoRole const CoreKeynodes::nrel_sc_text_translation("nrel_sc_text_translation");
ScKeynodeRole const CoreKeynodes::rrel_main_key_sc_element("rrel_main_key_sc_element");
ScKeynodeClass const CoreKeynodes::cardinality("cardinality");
ScKeynodeClass const CoreKeynodes::lang_ru("lang_ru");
ScKeynodeNoRole const CoreKeynodes::nrel_basic_sequence("nrel_basic_sequence");

}  // namespace scAgentsCommon
