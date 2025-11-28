#include <vector>
#include <string>
#include <filesystem>
#include "slang.h"
#include "slang-com-ptr.h"
#include "slang-com-helper.h"

class ShaderCompiler
{
private:
  Slang::ComPtr<slang::IGlobalSession> globalSession;
  Slang::ComPtr<slang::ISession> session;
  void setOptions();
  void diagnoseIfNeeded(slang::IBlob* diagnosticBlob) const;

public:
  ShaderCompiler();
  //ShaderCompiler& setTarget();
  void createSession(const SlangCompileTarget format, const std::string& profile);
  std::string  loadProgram(const std::string& moduleNames, const std::filesystem::path& modulePath, const std::string& moduleSources);
};
