#include <fstream>
#include <iostream>
#include <vulkan/vulkan_raii.hpp>

std::vector<char> readShaderFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);
    std::cout<<fileName<<std::endl;
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();
    return buffer;
}

[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, vk::raii::Device &device){
    vk::ShaderModuleCreateInfo creatInfo{
        .codeSize = code.size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };
    vk::raii::ShaderModule shaderModule{ device, creatInfo };
    return shaderModule;
}