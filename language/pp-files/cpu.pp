import pp;

/* FIXME: re-enable this
// Register a discovery catch-all for the "cpu" driver.
DISCOVER("cpu", {
	int cpu = args[0];

	string scope_name = sprintf("cpu.%d", cpu);
	SCOPE(scope_name, {
		BOOKMARK("cpu");
		CPUID_SCOPE("cpuid", cpu, {});
		MSR_SCOPE("msr", cpu, {});

	});
	ALIAS("cpu[]", scope_name);
});
*/
