#include "pp/pp.h"
#include "pp/device_init.h"
#include "pp/fake_language.h"

namespace pp {
namespace device {

void
msr_datatypes_init()
{
	ENUM("mtrr_type_t",
		KV("UC", 0),
		KV("WC", 1),
		KV("WT", 4),
		KV("WP", 5),
		KV("WB", 6));

	ENUM("pat_type_t",
		KV("UC", 0),
		KV("WC", 1),
		KV("WT", 4),
		KV("WP", 5),
		KV("WB", 6),
		KV("UC-", 7));
}

}  // namespace device
}  // namespace pp
