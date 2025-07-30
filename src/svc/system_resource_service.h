/**
 * @file system_resource_manager.h
 * @date 2025-07-21
 */
 #pragma once

 #include <algorithm>
 #include <bitset>
 #include <mutex>
 #include <thread>
 #include <vector>
 
 #ifdef __linux__
     #include <pthread.h>
     #include <sched.h>
 #endif
 
 // Use shared_mutex if available, otherwise fallback to mutex
 #if __cplusplus >= 201703L
     #include <shared_mutex>
     using shared_mutex_type = std::shared_mutex;
     using shared_lock_type = std::shared_lock<std::shared_mutex>;
     using unique_lock_type = std::unique_lock<std::shared_mutex>;
 #else
     using shared_mutex_type = std::mutex;
     using shared_lock_type = std::lock_guard<std::mutex>;
     using unique_lock_type = std::lock_guard<std::mutex>;
 #endif
 
 namespace sx
 {
 
 template <typename Derived>
 class SystemResourceManagerBase
 {
 public:
     /**
      * @brief Allocate CPU cores
      *
      * @param cpu_ids CPU IDs to allocate
      * @return true if allocation is successful
      * @return false if allocation is failed
      */
     bool allocate_cpu_cores(const std::vector<int>& cpu_ids) {
         return static_cast<Derived*>(this)->allocate_cpu_cores(cpu_ids);
     }
 
     /**
      * @brief Bind a thread to a CPU core
      *
      * @param thread_id Thread ID to bind
      * @param cpu_id CPU ID to bind
      * @return true if binding is successful
      * @return false if binding is failed
      */
     bool bind_to_cpu(std::thread::id thread_id, int cpu_id) {
         return static_cast<Derived*>(this)->bind_to_cpu(thread_id, cpu_id);
     }
 
     /**
      * @brief Check if a CPU core is allocated
      *
      * @param cpu_id CPU ID to check
      * @return true if the CPU core is allocated
      * @return false
      */
     bool is_cpu_allocated(int cpu_id) {
         return static_cast<Derived*>(this)->is_cpu_allocated(cpu_id);
     }
 
     /**
      * @brief Set the priority of a thread
      *
      * @param thread_id Thread ID to set priority
      * @param priority Priority to set
      * @return true if setting priority is successful
      * @return false if setting priority is failed
      */
     bool set_thread_priority(std::thread::id thread_id, int priority) {
         return static_cast<Derived*>(this)->set_thread_priority(thread_id, priority);
     }
 };
 
 class LinuxResourceManager : public SystemResourceManagerBase<LinuxResourceManager>
 {
 public:
     static LinuxResourceManager& get_instance() {
         static LinuxResourceManager instance;
         return instance;
     }
 
     /**
      * @brief Allocate CPU cores
      *
      * @param cpu_ids CPU IDs to allocate
      * @return true if allocation is successful
      * @return false if allocation is failed
      */
     bool allocate_cpu_cores(const std::vector<int>& cpu_ids) {
         unique_lock_type lock(resource_mutex_);
         if (!std::all_of(cpu_ids.begin(), cpu_ids.end(),
                          [this](int cpu_id) { return cpu_id >= 0 && cpu_id < kMaxCores; })) {
             return false;
         }
         for (int cpu_id : cpu_ids) {
             cpu_mask_[cpu_id] = true;
         }
         return true;
     }
 
     bool bind_to_cpu(std::thread::id thread_id, int cpu_id) {
         unique_lock_type lock(resource_mutex_);
         if (cpu_id < 0 || cpu_id >= kMaxCores || cpu_mask_[cpu_id]) {
             return false;
         }
 
         // bind thread to cpu
         // Use pthreads to set thread affinity on Linux
         // Note: std::thread::id cannot be directly converted to pthread_t,
         // so this function assumes the calling thread is the one to be bound.
         cpu_set_t cpuset;
         CPU_ZERO(&cpuset);
         CPU_SET(cpu_id, &cpuset);
 
         int result = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
         if (result == 0) {
             cpu_mask_[cpu_id] = true;
             return true;
         }
         return false;
     }
 
     bool is_cpu_allocated(int cpu_id) {
         if (cpu_id < 0 || cpu_id >= kMaxCores) return false;
         shared_lock_type lock(resource_mutex_);
         return cpu_mask_[cpu_id];
     }
 
     bool set_thread_priority(std::thread::id thread_id, int priority) {
         shared_lock_type lock(resource_mutex_);
         // set thread priority
         // Use pthreads to set thread priority on Linux
         // Note: std::thread::id cannot be directly converted to pthread_t,
         // so this function assumes the calling thread is the one to be bound.
         int result = pthread_setschedprio(pthread_self(), priority);
         return result == 0;
     }
 
     LinuxResourceManager(const LinuxResourceManager&) = delete;
     LinuxResourceManager& operator=(const LinuxResourceManager&) = delete;
     LinuxResourceManager(LinuxResourceManager&&) = delete;
     LinuxResourceManager& operator=(LinuxResourceManager&&) = delete;
 
 private:
     static constexpr int kMaxCores = 32;
     std::bitset<kMaxCores> cpu_mask_;
     shared_mutex_type resource_mutex_;
 
     LinuxResourceManager() = default;
     ~LinuxResourceManager() = default;
 };
 
 class NullResourceManager : public SystemResourceManagerBase<NullResourceManager>
 {
 public:
     static NullResourceManager& get_instance() {
         static NullResourceManager instance;
         return instance;
     }
 
     bool allocate_cpu_cores(const std::vector<int>& cpu_ids) {  // NOLINT
         (void)cpu_ids;
         return true;
     }
 
     bool bind_to_cpu(std::thread::id thread_id, int cpu_id) {  // NOLINT
         (void)thread_id;
         (void)cpu_id;
         return true;
     }
 
     bool is_cpu_allocated(int cpu_id) {  // NOLINT
         (void)cpu_id;
         return false;
     }
 
     bool set_thread_priority(std::thread::id thread_id, int priority) {  // NOLINT
         (void)thread_id;
         (void)priority;
         return true;
     }
 
     NullResourceManager(const NullResourceManager&) = delete;
     NullResourceManager& operator=(const NullResourceManager&) = delete;
     NullResourceManager(NullResourceManager&&) = delete;
     NullResourceManager& operator=(NullResourceManager&&) = delete;
 
 private:
     NullResourceManager() = default;
     ~NullResourceManager() = default;
 };
 
 #define SX_BIND_TO_CPU(cpu_id) \
     ::sx::LinuxResourceManager::get_instance().bind_to_cpu(std::this_thread::get_id(), cpu_id)
 
 // Helper macro to create vector from variadic arguments
 #define SX_MAKE_CPU_VECTOR(...) std::vector<int>{__VA_ARGS__}
 #define SX_ALLOCATE_CPU_CORES(...) \
     ::sx::LinuxResourceManager::get_instance().allocate_cpu_cores(SX_MAKE_CPU_VECTOR(__VA_ARGS__))
 
 #define SX_IS_CPU_ALLOCATED(cpu_id) \
     ::sx::LinuxResourceManager::get_instance().is_cpu_allocated(cpu_id)
 #define SX_SET_THREAD_PRIORITY(priority)                                                       \
     ::sx::LinuxResourceManager::get_instance().set_thread_priority(std::this_thread::get_id(), \
                                                                    priority)
 
 }  // namespace sx
 