#pragma once
#include <aidl/vendor/hardware/carsenze/BnCarsenze.h>

namespace aidl {
    namespace vendor {
        namespace hardware {
            namespace carsenze {
                struct NetworkStats {
                    std::string interfaceName;
                    unsigned long long rxBytes;
                    unsigned long long txBytes;
                };

                class Carsenze : public BnCarsenze {
                public:
                    //String getMemoryStats();
                    ndk::ScopedAStatus getMemoryStats(std::string* _aidl_return);
                    //String getCpuStats();
                    ndk::ScopedAStatus getCpuStats(std::string* _aidl_return);
                    //String getNetworkStats();
                    ndk::ScopedAStatus getNetworkStats(std::string* _aidl_return);


                    private:
                    double calculateCpuUsage();
                    long long prevTotalTime = 0;
                    long long prevIdleTime = 0;
                    std::vector<NetworkStats> getNetworkStats();
                };

            }  // namespace carsenze
        }  // namespace hardware
    }  // namespace android
}  // namespace aidl
