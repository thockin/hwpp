#include "pp/pp.h"
#include "pp/drivers/cpu/cpu_driver.h"
#include "pp/util/test.h"

namespace pp {

TEST(test_enumerate)
{
	try {
		std::vector<CpuAddress> addresses;
		CpuDriver::enumerate("/does.not.exist", &addresses);
		TEST_ERROR("CpuDriver::enumerate()");
	} catch (std::exception &e) {
	}

	try {
		system("rm -rf " TEST_TMP_DIR "/*");
		system("mkdir " TEST_TMP_DIR "/cpu0");
		system("mkdir " TEST_TMP_DIR "/cpu1");
		system("mkdir " TEST_TMP_DIR "/cpu2");
		system("mkdir " TEST_TMP_DIR "/cpu3");

		std::vector<CpuAddress> addresses;
		CpuDriver::enumerate(TEST_TMP_DIR, &addresses);
		if (addresses.size() != 4) {
			TEST_FAIL("CpuDriver::enumerate()");
		} else {
			TEST_ASSERT(addresses[0].cpu == 0);
			TEST_ASSERT(addresses[1].cpu == 1);
			TEST_ASSERT(addresses[2].cpu == 2);
			TEST_ASSERT(addresses[3].cpu == 3);
		}
	} catch (std::exception &e) {
		TEST_ERROR("CpuDriver::enumerate()");
	}

	try {
		system("rm -rf " TEST_TMP_DIR "/*");
		system("mkdir " TEST_TMP_DIR "/cpu4");
		system("mkdir " TEST_TMP_DIR "/cpu2");
		system("mkdir " TEST_TMP_DIR "/cpu0");

		std::vector<CpuAddress> addresses;
		CpuDriver::enumerate(TEST_TMP_DIR, &addresses);
		if (addresses.size() != 3) {
			TEST_FAIL("CpuDriver::enumerate()");
		} else {
			TEST_ASSERT(addresses[0].cpu == 0);
			TEST_ASSERT(addresses[1].cpu == 2);
			TEST_ASSERT(addresses[2].cpu == 4);
		}
	} catch (std::exception &e) {
		TEST_ERROR("CpuDriver::enumerate()");
	}

	try {
		system("rm -rf " TEST_TMP_DIR "/*");
		system("mkdir " TEST_TMP_DIR "/cpu0");
		system("mkdir " TEST_TMP_DIR "/cpu1");
		system("mkdir " TEST_TMP_DIR "/cpu_info");

		std::vector<CpuAddress> addresses;
		CpuDriver::enumerate(TEST_TMP_DIR, &addresses);
		if (addresses.size() != 2) {
			TEST_FAIL("CpuDriver::enumerate()");
		} else {
			TEST_ASSERT(addresses[0].cpu == 0);
			TEST_ASSERT(addresses[1].cpu == 1);
		}
	} catch (std::exception &e) {
		TEST_ERROR("CpuDriver::enumerate()");
	}

	try {
		system("rm -rf " TEST_TMP_DIR "/*");
		system("mkdir " TEST_TMP_DIR "/cpu0");
		system("mkdir " TEST_TMP_DIR "/cpu0_info");
		system("mkdir " TEST_TMP_DIR "/cpu1");
		system("mkdir " TEST_TMP_DIR "/cpu1_info");

		std::vector<CpuAddress> addresses;
		CpuDriver::enumerate(TEST_TMP_DIR, &addresses);
		if (addresses.size() != 2) {
			TEST_FAIL("CpuDriver::enumerate()");
		} else {
			TEST_ASSERT(addresses[0].cpu == 0);
			TEST_ASSERT(addresses[1].cpu == 1);
		}
	} catch (std::exception &e) {
		TEST_ERROR("CpuDriver::enumerate()");
	}
}

}  // namespace pp
