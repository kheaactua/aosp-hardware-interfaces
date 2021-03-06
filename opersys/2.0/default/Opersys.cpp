#define LOG_TAG "android.hardware.opersys@2.0-impl"

#include <log/log.h>

#include <hardware/hardware.h>
#include <hardware/opersyshw.h>

#include "Opersys.h"

namespace android {
namespace hardware {
namespace opersys {
namespace V2_0 {
namespace implementation {

Opersys::Opersys(opersyshw_device_t *device) : mDevice(device) {
    if (mDevice)
      ALOGI("Default Opersys HW HIDL implementation contstructor");
}

Opersys::~Opersys() {
    delete(mDevice);
}

// Methods from ::android::hardware::opersys::V2_0::IOpersys follow.
Return<void> Opersys::read(uint32_t size, read_cb _hidl_cb) {
    char* intermediate_buffer;
    int retval;

    ALOGI("read()");

    if ((intermediate_buffer = (char*) malloc(size * sizeof(char))) == NULL)
        _hidl_cb(Result::PERMISSION_DENIED, "");

    if ((retval = mDevice->read(intermediate_buffer, size)) < 0)
        _hidl_cb(Result::PERMISSION_DENIED, "");

    ALOGI("read() successfull");

    _hidl_cb(Result::OK, hidl_string(intermediate_buffer, retval));

    free(intermediate_buffer);

    return Void();
}

Return<Result> Opersys::write(const hidl_string& buffer) {
    int retval;

    ALOGI("write()");

    if ((retval = mDevice->write(buffer.c_str(), buffer.size())) < 0)
        return Result::PERMISSION_DENIED; // We're assuming this is the error, we could be wrong

    ALOGI("write() succcessfull");

    return Result::OK;
}

Return<int32_t> Opersys::test(int32_t valueIn) {
    ALOGI("test()");

    // Pass the test down to the legacy module
    return mDevice->test(valueIn);
}

Return<void> Opersys::clear_buffer() {
    mDevice->zero_content(valueIn);

    return Void();
}

Return<bool> Opersys::is_there_content() {
    return mDevice->is_there_content();
}

Return<int64_t> Opersys::get_last_write_time() {
    return mDevice->write_ts();
}

Return<int32_t> Opersys::get_read_stat() {
    return mDevice->get_read_n();
}

Return<int32_t> Opersys::get_write_stat() {
    return mDevice->get_write_n();
}

Return<void> Opersys::set_buffer_to_char(hidl_string const& ch) {
    mDevice->set_buffer_char(ch);

    return Void();
}

// Methods from ::android::hidl::base::V1_0::IBase follow.

IOpersys* HIDL_FETCH_IOpersys(const char* /* name */) {
    const hw_module_t* hw_module = nullptr;
    opersyshw_device_t* opersys_device = nullptr;

    ALOGI("HIDL_FETCH_IOpersys()");

    int err = hw_get_module(OPERSYSHW_HARDWARE_MODULE_ID, &hw_module);
    if (err) {
        ALOGE("hw_get_module %s failed: %d", OPERSYSHW_HARDWARE_MODULE_ID, err);
        return nullptr;
    }

    if (!hw_module->methods || !hw_module->methods->open) {
        opersys_device = reinterpret_cast<opersyshw_device_t*>(
            const_cast<hw_module_t*>(hw_module));
    } else {
        err = hw_module->methods->open(
            hw_module, OPERSYSHW_HARDWARE_MODULE_ID,
            reinterpret_cast<hw_device_t**>(&opersys_device));
        if (err) {
            ALOGE("Passthrough Opersys HW HIDL failed to load legacy HAL.");
            return nullptr;
        }
    }

    ALOGI("HIDL_FETCH_IOpersys() successfull");

    return new Opersys(opersys_device);
}

}  // namespace implementation
}  // namespace V2_0
}  // namespace opersys
}  // namespace hardware
}  // namespace android
