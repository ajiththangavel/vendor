#define LOG_TAG "vendor.hardware.carsenze-service"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include "Carsenze.h"

using aidl::vendor::hardware::carsenze::Carsenze;
using std::string_literals::operator""s;

void logd(std::string msg) {
    std::cout << msg << std::endl;
    ALOGD("%s", msg.c_str());
}

void loge(std::string msg) {
    std::cout << msg << std::endl;
    ALOGE("%s", msg.c_str());
}

int main() {

    ABinderProcess_setThreadPoolMaxThreadCount(0);
    ALOGD("Carsenze Service main() Starts here");

    std::shared_ptr<Carsenze> carsenze = ndk::SharedRefBase::make<Carsenze>();
    const std::string name = std::string() + Carsenze::descriptor + "/default";

    

    binder_status_t status = AServiceManager_addService(carsenze->asBinder().get(), name.c_str());
    CHECK_EQ(status, STATUS_OK);
    logd("Carsenze service starts to join service pool");
    ABinderProcess_joinThreadPool();
    loge("Carsenze service Failed to join Thread Pool");
    return EXIT_FAILURE;  // should not reached
}
