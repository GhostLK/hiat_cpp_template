#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>

namespace sx
{

// 轻量级错误码类型
using ErrorCode = uint32_t;

// 成功常量
constexpr ErrorCode SUCCESS = 0;

class ErrorModule
{
public:
    explicit ErrorModule(uint32_t baseCode) : baseCode_(baseCode) {}

    bool addError(uint32_t localCode, std::string_view description);

    std::string getError(uint32_t localCode) const noexcept;

    uint32_t getFullCode(uint32_t localCode) const noexcept {
        return baseCode_ | (localCode & 0xFFF);
    }

    [[nodiscard]] uint32_t baseCode() const noexcept { return baseCode_; }

private:
    uint32_t baseCode_;
    mutable std::shared_mutex mutex_;
    std::unordered_map<uint32_t, std::string> errorMap_;
};

class ErrorRegistry
{
public:
    static ErrorRegistry& instance();

    std::shared_ptr<ErrorModule> registerModule(std::string_view name, uint32_t baseCode);

    std::shared_ptr<ErrorModule> getModule(std::string_view name) const noexcept;

    std::string getErrorDescription(uint32_t fullCode) const noexcept;

private:
    ErrorRegistry() = default;

    mutable std::shared_mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<ErrorModule>> modules_;
    std::unordered_map<uint32_t, std::weak_ptr<ErrorModule>> baseIndex_;
};

class ModuleRegistrar
{
public:
    ModuleRegistrar(std::string_view name, uint32_t baseCode)
        : module_(ErrorRegistry::instance().registerModule(name, baseCode)),
          is_registered_(module_ != nullptr) {}

    [[nodiscard]] bool is_registered() const { return is_registered_; }

    bool addError(uint32_t localCode, std::string_view description) {
        return module_ ? module_->addError(localCode, description) : false;
    }

    [[nodiscard]] uint32_t getFullCode(uint32_t localCode) const noexcept {
        return module_ ? module_->getFullCode(localCode) : 0;
    }

    // 新增：创建 ErrorCode
    [[nodiscard]] ErrorCode makeError(uint32_t localCode) const noexcept {
        return module_ ? static_cast<ErrorCode>(module_->getFullCode(localCode)) : ErrorCode();
    }

private:
    std::shared_ptr<ErrorModule> module_;
    bool is_registered_ = false;
};

class ErrorCodeService
{
public:
    static ErrorCodeService& instance() {
        static ErrorCodeService inst;
        return inst;
    }

    static std::string getErrorDescription(uint32_t fullCode) noexcept {
        return ErrorRegistry::instance().getErrorDescription(fullCode);
    }

    static std::shared_ptr<ErrorModule> registerModule(std::string_view name, uint32_t baseCode) {
        return ErrorRegistry::instance().registerModule(name, baseCode);
    }

    static std::shared_ptr<ErrorModule> getModule(std::string_view name) noexcept {
        return ErrorRegistry::instance().getModule(name);
    }

    ErrorCodeService(const ErrorCodeService&) = delete;
    ErrorCodeService& operator=(const ErrorCodeService&) = delete;
    ErrorCodeService(ErrorCodeService&&) = delete;
    ErrorCodeService& operator=(ErrorCodeService&&) = delete;

private:
    ErrorCodeService() = default;
    ~ErrorCodeService() = default;
};

inline bool sx::ErrorModule::addError(uint32_t localCode, std::string_view description) {
    if (localCode > 0xFFF) return false;
    std::unique_lock lock(mutex_);
    auto [_, inserted] = errorMap_.emplace(localCode, std::string(description));
    return inserted;
}

inline std::string sx::ErrorModule::getError(uint32_t localCode) const noexcept {
    if (localCode > 0xFFF) return "Invalid local code";
    std::shared_lock lock(mutex_);
    auto iter = errorMap_.find(localCode);
    return iter != errorMap_.end() ? iter->second : "Unknown error";
}

inline sx::ErrorRegistry& sx::ErrorRegistry::instance() {
    static ErrorRegistry inst;
    return inst;
}

inline std::shared_ptr<sx::ErrorModule> sx::ErrorRegistry::registerModule(std::string_view name, uint32_t baseCode) {
    if ((baseCode & 0xFFF) != 0) return nullptr;
    std::unique_lock lock(mutex_);
    assert(modules_.find(std::string(name)) == modules_.end() && "Module already exists");
    assert(baseIndex_.find(baseCode) == baseIndex_.end() && "Base code already exists");
    auto mod = std::make_shared<ErrorModule>(baseCode);
    modules_.emplace(std::string(name), mod);
    baseIndex_.emplace(baseCode, mod);
    return mod;
}

inline std::shared_ptr<sx::ErrorModule> sx::ErrorRegistry::getModule(std::string_view name) const noexcept {
    std::shared_lock lock(mutex_);
    auto iter = modules_.find(std::string(name));
    return iter != modules_.end() ? iter->second : nullptr;
}

inline std::string sx::ErrorRegistry::getErrorDescription(uint32_t fullCode) const noexcept {
    const uint32_t base = fullCode & 0xFFFFF000;
    const uint32_t local = fullCode & 0xFFF;
    std::shared_lock lock(mutex_);
    auto wit = baseIndex_.find(base);
    if (auto mod = wit == baseIndex_.end() ? nullptr : wit->second.lock())
        return mod->getError(local);
    return "Unknown error code";
}

} // namespace sx

#define SX_REGISTER_ERROR_MODULE(name, base) \
    static ::sx::ModuleRegistrar _module_##name { #name, base }

#define SX_ADD_MODULE_ERROR(module, code, desc) _module_##module.addError(code, desc)

#define SX_GET_FULL_ERROR_CODE(module, code) _module_##module.getFullCode(code)

#define SX_GET_ERROR_DESCRIPTION(code) ::sx::ErrorRegistry::instance().getErrorDescription(code)

#define SX_MAKE_ERROR_CODE(module, code) _module_##module.makeError(code)
