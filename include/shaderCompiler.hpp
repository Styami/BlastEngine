#include <string>
#include "slang.h"
#include "slang-com-ptr.h"

class ShaderCompiler
{
private:
  Slang::ComPtr<slang::IGlobalSession> globalSession;
  Slang::ComPtr<slang::ISession> session;
  void setOptions();
  void diagnoseIfNeeded(slang::IBlob* diagnosticBlob) const;

public:
  ShaderCompiler();
  void createSession(const SlangCompileTarget format, const std::string& profile);
  std::string  loadProgram(const std::string& moduleNames);
};
