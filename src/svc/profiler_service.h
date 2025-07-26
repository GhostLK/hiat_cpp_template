#pragma once

#include <cstring>

#ifdef SX_PROFILER_ENABLE
    #include "tracy/Tracy.hpp"
#endif

namespace sx
{

template <typename Derived>
class PerfServiceBase
{
public:
    void frame_mark() { static_cast<Derived*>(this)->frame_mark(); }

    void plot(const char* name, double value) { static_cast<Derived*>(this)->plot(name, value); }

    void message(const char* text) { static_cast<Derived*>(this)->message(text); }
};

#ifdef SX_PROFILER_ENABLE
class TracyPerfService : public PerfServiceBase<TracyPerfService>
{
public:
    static TracyPerfService& get_instance() {
        static TracyPerfService instance;
        return instance;
    }
    void frame_mark() { FrameMark; }

    void plot(const char* name, double value) { TracyPlot(name, value); }

    void message(const char* text) {
        if (text != nullptr) TracyMessage(text, std::strlen(text));
    }

    TracyPerfService(const TracyPerfService&) = delete;
    TracyPerfService& operator=(const TracyPerfService&) = delete;
    TracyPerfService(TracyPerfService&&) = delete;
    TracyPerfService& operator=(TracyPerfService&&) = delete;

private:
    TracyPerfService() = default;
    ~TracyPerfService() = default;
};
using PerfService = TracyPerfService;
#else
class NullPerfService : public PerfServiceBase<NullPerfService>
{
public:
    static NullPerfService& get_instance() {
        static NullPerfService instance;
        return instance;
    }
    void frame_mark() {}
    void plot(const char*, double) {}
    void message(const char*) {}
    NullPerfService(const NullPerfService&) = delete;
    NullPerfService& operator=(const NullPerfService&) = delete;
    NullPerfService(NullPerfService&&) = delete;
    NullPerfService& operator=(NullPerfService&&) = delete;

private:
    NullPerfService() = default;
    ~NullPerfService() = default;
};
using PerfService = NullPerfService;
#endif

}  // namespace sx

#ifdef SX_PROFILER_ENABLE
    #define SX_PROFILER_ZONE(name) ZoneScopedN(name)
    #define SX_PROFILER_ZONE_COLOR(name, color) ZoneScopedNC(name, color)
    #define SX_PROFILER_FUNC() ZoneScoped
    #define SX_PROFILER_FRAME_MARK() ::sx::PerfService::get_instance().frame_mark()
    #define SX_PROFILER_PLOT(name, value) ::sx::PerfService::get_instance().plot(name, value)
    #define SX_PROFILER_MSG(text) ::sx::PerfService::get_instance().message(text)
#else
    #define SX_PROFILER_ZONE(name)
    #define SX_PROFILER_ZONE_COLOR(name, color)
    #define SX_PROFILER_FUNC()
    #define SX_PROFILER_FRAME_MARK()
    #define SX_PROFILER_PLOT(name, value)
    #define SX_PROFILER_MSG(text)
#endif
