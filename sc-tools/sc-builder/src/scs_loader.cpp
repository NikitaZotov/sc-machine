/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc-builder/scs_loader.hpp"

#include <sc-memory/sc_memory.hpp>

#include "scs_translator.hpp"

bool ScsLoader::loadScsFile(ScMemoryContext & context, std::string const & filename)
{
  SCsTranslator translator = SCsTranslator(context);

  Translator::Params translateParams;
  translateParams.m_fileName = filename;
  translateParams.m_autoFormatInfo = true;

  return translator.Translate(translateParams);
}
