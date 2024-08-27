
#define LOG_TAG "Carsenze_HAL_service"

#include <utils/Log.h>
#include <iostream>
#include <fstream>
#include <sys/sysinfo.h>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <future>
#include "Carsenze.h"

namespace aidl {
    namespace vendor {
        namespace hardware {
            namespace carsenze {
                //String getMemoryStats();
                ndk::ScopedAStatus Carsenze::getMemoryStats(std::string* _aidl_return) {
                    ALOGD("***-Memory Info is REQUESTED-***");
                    struct sysinfo memInfo;
                    sysinfo(&memInfo);
                     // Calculate total physical memory in bytes
                     long long int totalPhysMem = memInfo.totalram;
                     totalPhysMem *= memInfo.mem_unit;

                    // Calculate free physical memory in bytes
                    long long int freePhysMem = memInfo.freeram;
                    freePhysMem *= memInfo.mem_unit;
                    // Calculate used physical memory in bytes
                    long long int usedPhysMem = totalPhysMem - freePhysMem;

                    // Convert memory values to megabytes and format the output string
                    std::ostringstream oss;
                    oss << "Total RAM: " << (totalPhysMem / (1024 * 1024)) << " MB\n"
                    << "Free RAM: " << (freePhysMem / (1024 * 1024)) << " MB\n"
                    << "Used RAM: " << (usedPhysMem / (1024 * 1024)) << " MB";
                    *_aidl_return = oss.str();
                    return ndk::ScopedAStatus::ok();
                }
                //String getCpuStats();
                ndk::ScopedAStatus Carsenze::getCpuStats(std::string* _aidl_return) {
                    ALOGD("***-CPU Info is REQUESTED-***");

                    double usage = calculateCpuUsage();
                    *_aidl_return = "CPU Usage = " + std::to_string(usage) + " %";
                    return ndk::ScopedAStatus::ok();
                }
                //String getNetworkStats();
                ndk::ScopedAStatus Carsenze::getNetworkStats(std::string* _aidl_return) {
                    ALOGD("***-Network Info is REQUESTED-***");
                    
                    std::string path = "/sys/class/net/wlan0/operstate";
                    std::ifstream operstateFile(path);

                    if (!operstateFile.is_open()) {
                         ALOGD("***-Failed to open file /sys/class/net/wlan0/operstate-***");

                    }

                    std::string operstate;
                    operstateFile >> operstate;
                    operstateFile.close();

                    if (operstate == "up") {
                    const std::string interface = "wlan0"; // Change this to your network interface

                    // Launch a background thread to gather network statistics
                    auto future = std::async(std::launch::async, [this, interface]() {
                        auto stats1 = getNetworkStats();
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        auto stats2 = getNetworkStats();

                        std::ostringstream oss;

                        for (const auto& stat1 : stats1) {
                            for (const auto& stat2 : stats2) {
                                if (stat1.interfaceName == stat2.interfaceName && stat1.interfaceName == interface) {
                                    unsigned long long rxBytesPerSec = stat2.rxBytes - stat1.rxBytes;
                                    unsigned long long txBytesPerSec = stat2.txBytes - stat1.txBytes;

                                    double rxKbps = (rxBytesPerSec * 8.0) / 1024.0;
                                    double txKbps = (txBytesPerSec * 8.0) / 1024.0;

                                    oss << "Interface: " << stat1.interfaceName << "\n"
                                        << "   RX: " << rxKbps << " Kbps\n"
                                        << "   TX: " << txKbps << " Kbps\n";
                                }
                            }
                        }

                        return oss.str();
                    });

                    // Wait for the future to complete and get the result
                    *_aidl_return = "Connected\n" + future.get();
                    
                    } else if (operstate == "down") {
                         *_aidl_return = "Disconnected";
                    } else {
                         *_aidl_return = "Error";
                    }
                                
                    return ndk::ScopedAStatus::ok();
                }
                double Carsenze::calculateCpuUsage() {
                    std::ifstream file("/proc/stat");
                    
                     if(!file.is_open()) {
                    ALOGE("**-Failed to open /proc/stat-**");
                     }

                    std::string line;
                    std::getline(file, line);

                    std::istringstream ss(line);
                    std::string cpu;
                    ss >> cpu;

                    std::vector<long long> times;
                    long long time;
                    while (ss >> time) {
                        times.push_back(time);
                    }

                    if (times.size() < 4) {
                        return 0.0;
                    }

                    long long total = 0;
                    for (long long t : times) {
                        total += t;
                    }

                    long long idle = times[3];

                    long long totalDiff = total - prevTotalTime;
                    long long idleDiff = idle - prevIdleTime;

                    prevTotalTime = total;
                    prevIdleTime = idle;

                    if (totalDiff == 0) {
                        return 0.0;
                    }

                    return (double)(totalDiff - idleDiff) / totalDiff * 100.0;
                }
                std::vector<NetworkStats> Carsenze::getNetworkStats() {
                    std::vector<NetworkStats> stats;
                    std::ifstream file("/proc/net/dev");

                    if (!file.is_open()) {
                        ALOGE("**-Error opening /proc/net/dev-**");
                        return stats;
                    }

                    std::string line;
                    std::getline(file, line); // Skip the first header line
                    std::getline(file, line); // Skip the second header line

                    while (std::getline(file, line)) {
                        std::istringstream iss(line);
                        NetworkStats stat;
                        std::string dummy;
                        iss >> stat.interfaceName >> stat.rxBytes >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy >> stat.txBytes;
                        if (stat.interfaceName.back() == ':') {
                            stat.interfaceName.pop_back(); // Remove the trailing ':'
                        }
                        stats.push_back(stat);
                    }

                    file.close();
                    return stats;
                }

            }  // namespace carsenze
        }  // namespace hardwares
    }  // namespace vendor
}  // namespace aidl