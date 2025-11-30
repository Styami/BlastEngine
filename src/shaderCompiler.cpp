#include <array>
#include <iostream>
#include <print>
#include <ranges>
#include <bit>
#include "shaderCompiler.hpp"

ShaderCompiler::ShaderCompiler() {
  slang::createGlobalSession(globalSession.writeRef());
}


void ShaderCompiler::setOptions() {

}

void ShaderCompiler::createSession(const SlangCompileTarget format, const std::string& profile) {
  slang::TargetDesc targetDesc = {};
  slang::SessionDesc sessionDesc = {};
  targetDesc.format = format;
  targetDesc.profile = globalSession->findProfile(profile.c_str());
  
  const char* shadersFolder[] = {"shaders"};

  sessionDesc.searchPaths = shadersFolder;
  sessionDesc.searchPathCount = 1;
  sessionDesc.targets = &targetDesc;
  sessionDesc.targetCount = 1;

  globalSession->createSession(sessionDesc, session.writeRef());

  std::array<slang::CompilerOptionEntry, 2> options = {{
    {
      slang::CompilerOptionName::EmitSpirvDirectly,
      {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
    },
    {
      slang::CompilerOptionName::GenerateWholeProgram,
      {slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
    }
  }};
  
  sessionDesc.compilerOptionEntries = options.data();
  sessionDesc.compilerOptionEntryCount = static_cast<uint32_t>(options.size());
}

void ShaderCompiler::diagnoseIfNeeded(slang::IBlob* diagnosticBlob) const {
  if (diagnosticBlob != nullptr) {
    std::cout << static_cast<const char*>(diagnosticBlob->getBufferPointer()) << std::endl;
  }
}

std::string ShaderCompiler::loadProgram(const std::string& moduleNames) {
  Slang::ComPtr<slang::IModule> slangModule;
  Slang::ComPtr<slang::IBlob> diagnosticsBlob; // Permit to know every warning or error during compilation pipeline
  
  slangModule = session->loadModule(moduleNames.c_str(),diagnosticsBlob.writeRef());
  diagnoseIfNeeded(diagnosticsBlob);
  if(!slangModule)
    throw std::runtime_error("Failed to load Slang shader.");

  Slang::ComPtr<slang::IComponentType> composedProgram;
  
  Slang::ComPtr<slang::IComponentType> linkedProgram;
  Slang::Result result = slangModule->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
  diagnoseIfNeeded(diagnosticsBlob);
  if (SLANG_FAILED(result))
    throw std::runtime_error("Failed to link program.");

  Slang::ComPtr<slang::IBlob> compiledCodeObject;
  result = linkedProgram->getTargetCode(0, compiledCodeObject.writeRef(), diagnosticsBlob.writeRef());
  diagnoseIfNeeded(diagnosticsBlob);
  if (SLANG_FAILED(result))
    throw std::runtime_error("Failed to generate spirv program.");

  std::string compiledCode = std::string(static_cast<const char*>(compiledCodeObject->getBufferPointer()), compiledCodeObject->getBufferSize());

  return compiledCode;
}