#pragma once

#include "sc-memory/sc_memory.hpp"

#include "sc-test-framework/sc_test_unit.hpp"

class BuilderTestUnit : public test::ScTestUnit
{
public:
  BuilderTestUnit(char const * name, char const * filename, void(*fn)())
    : test::ScTestUnit(name, filename, fn)
  {
  }

private:
  
  void InitMemory(std::string const & configPath, std::string const& extPath)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);

    params.clear = SC_FALSE;
    params.repo_path = "sc-builder-test-repo";
    params.config_file = configPath.c_str();
    params.ext_path = extPath.empty() ? nullptr : extPath.c_str();

    ScMemory::LogMute();
    SC_ASSERT(ScMemory::Initialize(params), ("Unable to initialize memory"));
    ScMemory::LogUnmute();
  }

  void ShutdownMemory(bool save)
  {
    ScMemory::LogMute();
    ScMemory::Shutdown(false);
    ScMemory::LogUnmute();
  }
};