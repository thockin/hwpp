//
// This file includes support for AMD K8 family processors.  The following
// revisions are supported:
// 	- Rev E
// 	- Rev F (NPT, Family 0F)
//
// This file has been developed purely from publicly available
// documentation.
//
// Tim Hockin
// May, 2008
//
#include "pp.h"
#include "fake_language.h"
#include "devices/pci/generic_device.h"
#include "devices/cpuid/generic_device.h"
#include "devices/msr/generic_device.h"


static void
k8_cpuid(const pp_value &cpu)
{
	CPUID_SCOPE("cpuid", cpu);

	//FIXME: more

	CLOSE_SCOPE(); // cpuid
}

static void
k8_msr(const pp_value &cpu)
{
	MSR_SCOPE("msr", cpu);

	OPEN_SCOPE("EFER");
	REG64("%EFER", 0xc0000080);
	FIELD("SYSCALL", "enabledisable_t", BITS("%EFER", 0));
	FIELD("LME", "enabledisable_t", BITS("%EFER", 8));
	FIELD("LMA", "yesno_t", BITS("%EFER", 10));
	FIELD("NXE", "enabledisable_t", BITS("%EFER", 11));
	FIELD("LMSLE", "enabledisable_t", BITS("%EFER", 13));
	FIELD("FFXSR", "enabledisable_t", BITS("%EFER", 14));
	CLOSE_SCOPE(); // EFER

	OPEN_SCOPE("SYSCFG");
	REG64("%SYSCFG", 0xc0010010);
	FIELD("SysAckLimit", "int_t", BITS("%SYSCFG", 4, 0));
	FIELD("SysVicLimit", "int_t", BITS("%SYSCFG", 7, 5));
	FIELD("SetDirtyEnE", "enabledisable_t", BITS("%SYSCFG", 8));
	FIELD("SetDirtyEnS", "enabledisable_t", BITS("%SYSCFG", 9));
	FIELD("SetDirtyEnO", "enabledisable_t", BITS("%SYSCFG", 10));
	if (FIELD_EQ("$core/k8_rev", "rev_e")) {
		FIELD("ClVicBlkEn", "enabledisable_t", BITS("%SYSCFG", 11));
	}
	FIELD("ChxToDirtyDis", "disableenable_t", BITS("%SYSCFG", 16));
	FIELD("SysUcLockEn", "enabledisable_t", BITS("%SYSCFG", 17));
	FIELD("MtrrFixDramEn", "enabledisable_t", BITS("%SYSCFG", 18));
	FIELD("MtrrFixDramModEn", "enabledisable_t", BITS("%SYSCFG", 19));
	FIELD("MtrrVarDramEn", "enabledisable_t", BITS("%SYSCFG", 20));
	FIELD("MtrrTom2En", "enabledisable_t", BITS("%SYSCFG", 21));
	if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		FIELD("Tom2ForceMemTypeWB", "yesno_t", BITS("%SYSCFG", 22));
	}
	CLOSE_SCOPE(); // SYSCFG

	OPEN_SCOPE("HWCR");
	HEX("fid_t", 8);
	REG64("%HWCR", 0xc0010015);
	FIELD("SMMLOCK", "yesno_t", BITS("%HWCR", 0));
	FIELD("SLOWFENCE", "enabledisable_t", BITS("%HWCR", 1));
	FIELD("TLBCACHEDIS", "disableenable_t", BITS("%HWCR", 3));
	FIELD("INVD_WBINVD", "yesno_t", BITS("%HWCR", 4));
	FIELD("FFDIS", "disableenable_t", BITS("%HWCR", 6));
	FIELD("DISLOCK", "disableenable_t", BITS("%HWCR", 7));
	FIELD("IGNNE_EM", "enabledisable_t", BITS("%HWCR", 8));
	FIELD("HLTXSPCYCEN", "enabledisable_t", BITS("%HWCR", 12));
	FIELD("SMISPCYCDIS", "disableenable_t", BITS("%HWCR", 13));
	FIELD("RSMSPCYCDIS", "disableenable_t", BITS("%HWCR", 14));
	FIELD("SSEDIS", "disableenable_t", BITS("%HWCR", 15));
	FIELD("WRAP32DIS", "disableenable_t", BITS("%HWCR", 17));
	FIELD("MCi_STATUS_WREN", "enabledisable_t", BITS("%HWCR", 18));
	FIELD("START_FID", "fid_t", BITS("%HWCR", 29, 24));
	CLOSE_SCOPE(); // HWCR

	OPEN_SCOPE("NB_CFG");
	REG64("%NB_CFG", 0xc001001f);
	if (FIELD_EQ("$core/k8_rev", "rev_e")) {
		FIELD("EnRefUseFreeBuf", "enabledisable_t",
				BITS("%NB_CFG", 9));
	} else if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		FIELD("DisRefUseFreeBuf", "disableenable_t",
				BITS("%NB_CFG", 9));
	}
	FIELD("DisCohLdtCfg", "disableenable_t", BITS("%NB_CFG", 31));
	FIELD("DisDatMask", "disableenable_t", BITS("%NB_CFG", 36));
	FIELD("DisThmlPfMonSmiIntr", "disableenable_t", BITS("%NB_CFG", 43));
	FIELD("DisUsSysMgtRqToNLdt", "disableenable_t", BITS("%NB_CFG", 45));
	FIELD("InitApicIdCpuIdLo", "yesno_t", BITS("%NB_CFG", 54));
	CLOSE_SCOPE(); // NB_CFG

	OPEN_SCOPE("id");

	REG64("%MANID", 0xc001001e);
	FIELD("MinorRev", "int_t", BITS("%MANID", 3, 0));
	FIELD("MajorRev", "int_t", BITS("%MANID", 7, 4));
	FIELD("ReticleSite", "int_t", BITS("%MANID", 9, 8));

	CLOSE_SCOPE(); // id

	OPEN_SCOPE("HTC");
	REG64("%HTC", 0xc001003e);
	FIELD("HtcEn", "enabledisable_t", BITS("%HTC", 0));
	FIELD("HtcSbcEn", "enabledisable_t", BITS("%HTC", 1));
	FIELD("HtcAct", "yesno_t", BITS("%HTC", 4));
	FIELD("HtcActSts", "yesno_t", BITS("%HTC", 5));
	CLOSE_SCOPE(); // htc

	OPEN_SCOPE("thermal_control");
	REG64("%ThermalControl", 0xc001003f);
	FIELD("StcSbcTmpHiEn", "enabledisable_t",
			BITS("%ThermalControl", 0));
	FIELD("StcSbcTmpLoEn", "enabledisable_t", 
			BITS("%ThermalControl", 1));
	FIELD("StcApcTmpHiEn", "enabledisable_t",
			BITS("%ThermalControl", 2));
	FIELD("StcApcTmpLoEn", "enabledisable_t",
			BITS("%ThermalControl", 3));
	if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		FIELD("StcHtcEn", "enabledisable_t",
				BITS("%ThermalControl", 4));
	}
	FIELD("StcTmpHiSts", "yesno_t",
			BITS("%ThermalControl", 6));
	FIELD("StcTmpLoSts", "yesno_t",
			BITS("%ThermalControl", 7));
	FIELD("StcTmpLmt", ANON_XFORM(ANON_INT("C"),
				LAMBDA(52 + (_1 * 2)),
				LAMBDA((_1 - 52) / 2)),
			BITS("%ThermalControl", 20, 16));
	FIELD("StcHystLmt", ANON_XFORM(ANON_INT("C"),
				LAMBDA(2 + (_1 * 2)),
				LAMBDA((_1 - 2) / 2)),
			BITS("%ThermalControl", 27, 24));
	CLOSE_SCOPE(); // thermal_control

	REG64("%TOP_MEM", 0xc001001a);
	REG64("%TOP_MEM2", 0xc001001d);
	FIELD("TOM", "addr64_t",
			BITS("%TOP_MEM", 39, 23) + BITS("%0", 22, 0));
	FIELD("TOM2", "addr64_t",
			BITS("%TOP_MEM2", 39, 23) + BITS("%0", 22, 0));

	for (int i = 0; i < 2; i++) {
		OPEN_SCOPE("iorr[]");

		REG64("%IORRBase", 0xc0010016 + (i * 0x02));
		FIELD("WrDram", "yesno_t", BITS("%IORRBase", 3));
		FIELD("RdDram", "yesno_t", BITS("%IORRBase", 4));
		FIELD("Base", "addr64_t",
				BITS("%IORRBase", 39, 12) + BITS("%0", 11, 0));

		REG64("%IORRMask", 0xc0010017 + (i * 0x02));
		FIELD("V", "enabledisable_t", BITS("%IORRMask", 11));
		FIELD("Mask", "addr64_t",
				BITS("%IORRMask", 39, 12) + BITS("%0", 11, 0));

		CLOSE_SCOPE(); // iorr[]
	}

	OPEN_SCOPE("STAR");
	REG64("%STAR", 0xc0000081);
	FIELD("Target", "addr32_t", BITS("%STAR", 31, 0));
	FIELD("SysCallSel", "hex16_t", BITS("%STAR", 47, 32));
	FIELD("SysRetSel", "hex16_t", BITS("%STAR", 63, 48));
	CLOSE_SCOPE(); // STAR

	FIELD("LSTAR", "addr64_t", REG64(0xc0000082));
	FIELD("CSTAR", "addr64_t", REG64(0xc0000082));

	REG64("%SF_MASK", 0xc0000084);
	FIELD("SF_MASK", "hex32_t", BITS("%SF_MASK", 31, 0));

	FIELD("FS_BASE", "addr64_t", REG64(0xc0000100));
	FIELD("GS_BASE", "addr64_t", REG64(0xc0000101)) ;
	FIELD("KernelGSBase", "addr64_t", REG64(0xc0000102));

	for (int i = 0; i < 4; i++) {
		OPEN_SCOPE("perf_ctr[]");

		REG64("%PerfEvtSel", 0xc0010000 + i);
		FIELD("EVENT_SELECT", ANON_ENUM(
			KV("dispatched_fpu_ops", 0x00),
			KV("cycles_with_no_fpu_ops_retired", 0x01),
			KV("dispatched_fast_flag_fpu_ops", 0x02),
			KV("segment_reg_loads", 0x20),
			KV("pipeline_restart_self_modifying_code", 0x21),
			KV("pipeline_restart_probe_hit", 0x22),
			KV("ls_buffer_2_full", 0x23),
			KV("locked_ops", 0x24),
			KV("mem_requests_by_type", 0x65),
			KV("data_cache_accesses", 0x40),
			KV("data_cache_misses", 0x41),
			KV("data_cache_refills_from_l2_or_system", 0x42),
			KV("data_cache_refills_from_system", 0x43),
			KV("data_cache_lines_evicted", 0x44),
			KV("l1_dtlb_miss_l2_dtlb_hit", 0x45),
			KV("l1_and_l2_dtlb_miss", 0x46),
			KV("misaligned_accesses", 0x47),
			KV("microarch_late_cancel_of_access", 0x48),
			KV("microarch_early_cancel_of_access", 0x49),
			KV("single_bit_ecc_err_recorded_by_scrubber", 0x4a),
			KV("prefetch_instrs_dispatched", 0x4b),
			KV("dcache_misses_by_locked_instrs", 0x4c),
			KV("data_prefetcher", 0x67),
			KV("system_read_responses_by_coherency_state", 0x6c),
			KV("quadwords_written_to_system", 0x6d),
			KV("requests_to_l2_cache", 0x7d),
			KV("l2_cache_misses", 0x7e),
			KV("l2_fill_or_writeback", 0x7f),
			KV("instr_cache_fetches", 0x80),
			KV("instr_cache_misses", 0x81),
			KV("instr_cache_refills_from_l2", 0x82),
			KV("instr_cache_refills_from_system", 0x83),
			KV("l1_itlb_miss_l2_itlb_hit", 0x84),
			KV("l1_and_l2_itlb_miss", 0x85),
			KV("pipeline_restart_due_to_instr_stream_probe", 0x86),
			KV("instr_fetch_stall", 0x87),
			KV("return_stack_hits", 0x88),
			KV("return_stack_overflows", 0x89),
			KV("retired_clflush_instrs", 0x26),
			KV("retired_cpuid_instrs", 0x27),
			KV("cpu_clocks_not_halted", 0x76),
			KV("retired_instrs", 0xc0),
			KV("retired_uops", 0xc1),
			KV("retired_branch_instrs", 0xc2),
			KV("retired_mispredicted_branch_instrs", 0xc3),
			KV("retired_taken_branch_instrs", 0xc4),
			KV("retired_taken_branch_instrs_mispredicted", 0xc5),
			KV("retired_far_control_transfers", 0xc6),
			KV("retired_branch_resyncs", 0xc7),
			KV("retired_near_returns", 0xc8),
			KV("retired_near_returns_mispredicted", 0xc9),
			KV("retired_indirect_branches_mispredicted", 0xca),
			KV("retired_mmx_or_fp_instrs", 0xcb),
			KV("retired_fastpath_double_op_instrs", 0xcc),
			KV("ints_masked_cycles", 0xcd),
			KV("ints_masked_cycles_with_int_pending", 0xce),
			KV("ints_taken", 0xcf),
			KV("decoder_empty", 0xd0),
			KV("dispatch_stalls", 0xd1),
			KV("dispatch_stall_for_branch_abort_to_retire", 0xd2),
			KV("dispatch_stall_for_serialization", 0xd3),
			KV("dispatch_stall_for_segment_load", 0xd4),
			KV("dispatch_stall_for_reorder_buffer_full", 0xd5),
			KV("dispatch_stall_for_reservation_station_full", 0xd6),
			KV("dispatch_stall_for_fpu_full", 0xd7),
			KV("dispatch_stall_for_ls_full", 0xd8),
			KV("dispatch_stall_waiting_for_all_quiet", 0xd9),
			KV("dispatch_stall_for_far_transfer", 0xda),
			KV("fpu_exceptions", 0xdb),
			KV("dr0_breakpoint_matches", 0xdc),
			KV("dr1_breakpoint_matches", 0xdd),
			KV("dr2_breakpoint_matches", 0xde),
			KV("dr3_breakpoint_matches", 0xdf),
			KV("dram_accesses", 0xe0),
			KV("mem_controller_page_table_overflows", 0xe1),
			KV("mem_controller_turnarounds", 0xe3),
			KV("mem_controller_bypass_counter_saturation", 0xe4),
			KV("sized_blocks", 0xe5),
			KV("ecc_errors", 0xe8),
			KV("cpu_or_io_requests_to_memory_or_io", 0xe9),
			KV("cache_block_commands", 0xea),
			KV("sized_commands", 0xeb),
			KV("probe_responses_and_upstream_requests", 0xec),
			KV("gart_events", 0xee),
			KV("ht_link_0_transmit_bandwidth", 0xf6),
			KV("ht_link_1_transmit_bandwidth", 0xf7),
			KV("ht_link_2_transmit_bandwidth", 0xf8)),
						BITS("%PerfEvtSel", 7, 0));
		if (FIELD_EQ("EVENT_SELECT", "dispatched_fpu_ops")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("add_pipe_ops", 0),
				KV("multiply_pipe_ops", 1),
				KV("store_pipe_ops", 2),
				KV("add_pipe_load_ops", 3),
				KV("multiply_pipe_load_ops", 4),
				KV("store_pipe_load_ops", 5)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "segment_reg_loads")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("es", 0),
				KV("cs", 1),
				KV("ss", 2),
				KV("ds", 3),
				KV("fs", 4),
				KV("gs", 5),
				KV("hs", 6)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "locked_ops")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("num_locked_instrs_executed", 0),
				KV("num_cycles_in_speculative_phase", 1),
				KV("num_cycles_in_nonspeculative_phase", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "mem_requests_by_type")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("requests_to_uc_mem", 0),
				KV("requests_to_wc_mem_or_"
				   "wc_buffer_flushes_to_wb_mem", 1),
				KV("ss_requests", 7)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "data_cache_refills_from_l2_or_system")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("refill_from_sys", 0),
				KV("shared_state_line_from_l2", 1),
				KV("exclusive_state_line_from_l2", 2),
				KV("owned_state_line_from_l2", 3),
				KV("modified_state_line_from_l2", 4)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "data_cache_refills_from_system") ||
			   FIELD_EQ("EVENT_SELECT",
				    "data_cache_lines_evicted")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("invalid", 0),
				KV("shared", 1),
				KV("exclusive", 2),
				KV("owned", 3),
				KV("modified", 4)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "single_bit_ecc_err_"
				    "recorded_by_scrubber")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("scrubber_error", 0),
				KV("piggyback_scrubber_errors", 1)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "prefetch_instrs_dispatched")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("load", 0),
				KV("store", 1),
				KV("nta", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "dcache_misses_by_locked_instrs")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("data_cache_misses_by_locked_instrs", 1)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "data_prefetcher")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("cancelled_prefetches", 0),
				KV("prefetch_attempts", 1)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "system_read_"
				    "responses_by_coherency_state")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("exclusive", 0),
				KV("modified", 1),
				KV("shared", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "quadwords_written_to_system")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("quadword_write_transfer", 0)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "requests_to_l2_cache")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("ic_fill", 0),
				KV("dc_fill", 1),
				KV("tlb_fill", 2),
				KV("tag_snoop_request", 3),
				KV("cancelled_request", 4)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "l2_cache_misses")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("ic_fill", 0),
				KV("dc_fill_incl_possible_replays", 1),
				KV("tlb_page_table_walk", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "l2_fill_or_writeback")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("l2_fills", 0)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "retired_mmx_or_fp_instrs")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("x87_instrs", 0),
				KV("mmx_and_3dnow_instrs", 1),
				KV("packed_sse_and_sse2_instrs", 2),
				KV("scalar_sse_and_sse2_instrs", 3)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "retired_fastpath_double_op_instrs")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("low_op_in_pos_0", 0),
				KV("low_op_in_pos_1", 1),
				KV("low_op_in_pos_2", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "fpu_exceptions")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("x87_reclass_microfaults", 0),
				KV("sse_retype_microfaults", 1),
				KV("sse_reclass_microfaults", 2),
				KV("sse_and_x87_microtraps", 3)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "dram_accesses")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("page_hit", 0),
				KV("page_miss", 1),
				KV("page_conflict", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "mem_controller_turnarounds")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("dimm_chip_select_turnaround", 0),
				KV("read_to_write_turnaround", 1),
				KV("write_to_read_turnaround", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "mem_controller_"
				    "bypass_counter_saturation")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("mem_controller_high_priority_bypass", 0),
				KV("mem_controller_low_priority_bypass", 1),
				KV("dram_controller_interface_bypass", 2),
				KV("dram_controller_queue_bypass", 3)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "sized_blocks")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("32_byte_sized_writes", 2),
				KV("64_byte_sized_writes", 3),
				KV("32_byte_sized_reads", 4),
				KV("64_byte_sized_reads", 5)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "ecc_errors")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("num_of_dram_ecc_errors", 7)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "cpu_or_io_requests_to_memory_or_io")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("io_to_io", 0),
				KV("io_to_mem", 1),
				KV("cpu_to_io", 2),
				KV("cpu_to_mem", 3),
				KV("to_remote_node", 4),
				KV("to_local_node", 5),
				KV("from_remote_node", 6),
				KV("from_local_node", 7)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "cache_block_commands")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("victim_block", 0),
				KV("read_block", 2),
				KV("read_block_shared", 3),
				KV("read_block_modified", 4),
				KV("change_to_dirty", 5)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "sized_commands")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("nonposted_szwr_byte", 0),
				KV("nonposted_szwr_dword", 1),
				KV("posted_szwr_byte", 2),
				KV("posted_szwr_dword", 3),
				KV("szrd_byte", 4),
				KV("szrd_dword", 5),
				KV("rdmodwr", 6)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "probe_responses_and_upstream_requests")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("probe_miss", 0),
				KV("probe_hit_clean", 1),
				KV("probe_hit_dirty_without_mem_cancel", 2),
				KV("probe_hit_dirty_with_mem_cancel", 3),
				KV("upstream_display_refresh_reads", 4),
				KV("upstream_non_display_refresh_reads", 5),
				KV("upstream_writes", 6)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT", "gart_events")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("gart_aperture_hit_on_cpu_access", 0),
				KV("gart_aperture_hit_on_io_access", 1),
				KV("gart_miss", 2)),
					BITS("%PerfEvtSel", 15, 8));
		} else if (FIELD_EQ("EVENT_SELECT",
				    "ht_link_0_transmit_bandwidth") ||
			   FIELD_EQ("EVENT_SELECT",
				    "ht_link_1_transmit_bandwidth") ||
			   FIELD_EQ("EVENT_SELECT",
				    "ht_link_2_transmit_bandwidth")) {
			FIELD("UINT_MASK", ANON_BITMASK(
				KV("command_dword_sent", 0),
				KV("data_dword_sent", 1),
				KV("buffer_release_dword_sent", 2),
				KV("nop_dword_sent", 3)),
					BITS("%PerfEvtSel", 15, 8));
		} else {
			// No special interpretation found, fall back to
			// generic bitmask
			FIELD("UNIT_MASK", "bitmask_t",
					BITS("%PerfEvtSel", 15, 8));
		}
		FIELD("USR", "yesno_t", BITS("%PerfEvtSel", 16));
		FIELD("OS", "yesno_t", BITS("%PerfEvtSel", 17));
		FIELD("E", ANON_BOOL("edge_detection", "level_detection"),
				BITS("%PerfEvtSel", 18));
		FIELD("PC", ANON_BOOL("toggle_on_overflow",
				      "toggle_on_increment"),
				BITS("%PerfEvtSel", 19));
		FIELD("INT", "enabledisable_t", BITS("%PerfEvtSel", 20));
		FIELD("EN", "enabledisable_t", BITS("%PerfEvtSel", 22));
		FIELD("INV", "yesno_t", BITS("%PerfEvtSel", 23));
		if (FIELD_EQ("INV", pp_value(0))) {
			FIELD("CNT_MASK",
				ANON_ENUM(KV("incr_always", 0),
					  KV("incr_if_ge_1", 1),
					  KV("incr_if_ge_2", 2),
					  KV("incr_if_ge_3", 3)),
				BITS("%PerfEvtSel", 31, 24));
		} else if (FIELD_EQ("INV", 1)) {
			FIELD("CNT_MASK",
				ANON_ENUM(KV("incr_always", 0),
					  KV("incr_if_lt_1", 1),
					  KV("incr_if_lt_2", 2),
					  KV("incr_if_lt_3", 3)),
				BITS("%PerfEvtSel", 31, 24));
		}

		REG64("%PerfCtr", 0xc0010004 + i);
		FIELD("CTR", "int_t", BITS("%PerfCtr", 47, 0));

		CLOSE_SCOPE(); // perf_ctr[]
	}

	OPEN_SCOPE("power_mgmt");

	REG64("%FIDVID_CTL", 0xc0010041);
	FIELD("NewFID", "fid_t", BITS("%FIDVID_CTL", 5, 0));
	if (FIELD_EQ("$core/k8_rev", "rev_e")) {
		FIELD("NewVID", "vid_t", BITS("%FIDVID_CTL", 12, 8));
	} else if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		FIELD("NewVID", "vid_t", BITS("%FIDVID_CTL", 13, 8));
	}
	FIELD("InitFidVid", "yesno_t", BITS("%FIDVID_CTL", 16));
	FIELD("StpGntTOCnt", ANON_INT("system clock cycles"),
			BITS("%FIDVID_CTL", 51, 32));

	REG64("%FIDVID_STATUS", 0xc0010042);
	FIELD("CurrFID", "fid_t", BITS("%FIDVID_STATUS", 5, 0));
	FIELD("StartFID", "fid_t", BITS("%FIDVID_STATUS", 13, 8));
	FIELD("MaxFID", "fid_t", BITS("%FIDVID_STATUS", 21, 16));
	FIELD("FidVidPending", "yesno_t", BITS("%FIDVID_STATUS", 31));
	if (FIELD_EQ("$core/k8_rev", "rev_e")) {
		FIELD("MaxRampVID", "vid_t", BITS("%FIDVID_STATUS", 28, 24));
		FIELD("CurrVID", "vid_t", BITS("%FIDVID_STATUS", 36, 32));
		FIELD("StartVID", "vid_t", BITS("%FIDVID_STATUS", 44, 40));
		FIELD("MaxVID", "vid_t", BITS("%FIDVID_STATUS", 52, 48));
		FIELD("MinVID", "vid_t", BITS("%FIDVID_STATUS", 60, 56));
	} else if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		FIELD("MaxRampVID", "vid_t", BITS("%FIDVID_STATUS", 29, 24));
		FIELD("CurrVID", "vid_t", BITS("%FIDVID_STATUS", 37, 32));
		FIELD("StartVID", "vid_t", BITS("%FIDVID_STATUS", 45, 40));
		FIELD("MaxVID", "vid_t", BITS("%FIDVID_STATUS", 53, 48));
		FIELD("PstateStep", ANON_ENUM(KV("mV_25_reduction", 0),
					      KV("mV_50_reduction", 1)),
				BITS("%FIDVID_STATUS", 56));
		FIELD("AltVidOffset", ANON_ENUM(KV("not_supported", 0),
						KV("mV_neg_50", 1),
						KV("mV_neg_100", 2),
						KV("mV_neg_125", 3),
						KV("mV_neg_150", 4),
						KV("mV_neg_175", 5),
						KV("mV_neg_200", 6),
						KV("mV_neg_225", 7)),
				BITS("%FIDVID_STATUS", 59, 57));
		FIELD("IntPstateSup", "yesno_t", BITS("%FIDVID_STATUS", 61));
	}

	CLOSE_SCOPE(); // power_mgmt

	OPEN_SCOPE("mc_ctl_mask");

	for (int i = 0; i < 5; i++) {
		REG64("%MC_CTL_MASK[]", 0xc0010044 + i);
		FIELD("MC_CTL_MASK[]", "bitmask_t", BITS("%MC_CTL_MASK[$]"));
	}

	CLOSE_SCOPE(); // mc_ctl_mask

	OPEN_SCOPE("io_trap");

	for (int i = 0; i < 4; i++) {
		OPEN_SCOPE("trap[]");
		REG64("%IOTRAP_ADDR", 0xc0010050 + i);
		FIELD("SmiAddr", "addr32_t", BITS("%IOTRAP_ADDR", 31, 0));
		FIELD("SmiMask", "addr32_t", BITS("%IOTRAP_ADDR", 55, 32));
		FIELD("ConfigSmi", "yesno_t", BITS("%IOTRAP_ADDR", 61));
		FIELD("SmiOnWrEn", "enabledisable_t",
				BITS("%IOTRAP_ADDR", 62));
		FIELD("SmiOnRdEn", "enabledisable_t",
				BITS("%IOTRAP_ADDR", 63));
		CLOSE_SCOPE(); // trap[]
		// FIXME: add an ALIAS to SmiEn_* and SmiSts_*
	}

	REG64("%IOTRAP_CTL", 0xc0010054);
	FIELD("SmiSts_0", "yesno_t", BITS("%IOTRAP_CTL", 0));
	FIELD("SmiEn_0", "enabledisable_t", BITS("%IOTRAP_CTL", 1));
	FIELD("SmiSts_1", "yesno_t", BITS("%IOTRAP_CTL", 2));
	FIELD("SmiEn_1", "enabledisable_t", BITS("%IOTRAP_CTL", 3));
	FIELD("SmiSts_2", "yesno_t", BITS("%IOTRAP_CTL", 4));
	FIELD("SmiEn_2", "enabledisable_t", BITS("%IOTRAP_CTL", 5));
	FIELD("SmiSts_3", "yesno_t", BITS("%IOTRAP_CTL", 6));
	FIELD("SmiEn_3", "enabledisable_t", BITS("%IOTRAP_CTL", 7));
	FIELD("IoTrapCtlRsmSpcEn", "enabledisable_t", BITS("%IOTRAP_CTL", 13));
	FIELD("IoTrapCtlSmiSpcEn", "enabledisable_t", BITS("%IOTRAP_CTL", 14));
	FIELD("IoTrapEn", "enabledisable_t", BITS("%IOTRAP_CTL", 15));

	CLOSE_SCOPE(); // io_trap

	OPEN_SCOPE("interrupt_pending_msg");

	REG64("%InterruptPendingMessage", 0xc0010055);
	FIELD("IOMsgAddr", "addr16_t", BITS("%InterruptPendingMessage", 15, 0));
	FIELD("IntrPndMsgDis", "disableenable_t",
			BITS("%InterruptPendingMessage", 24));
	FIELD("IntrPndMsg", ANON_BOOL("io_space_msg",
				      "ht_int_pending_msg"),
			BITS("%InterruptPendingMessage", 25));
	FIELD("IORd", ANON_BOOL("io_read", "io_write"),
			BITS("%InterruptPendingMessage", 26));
	if (FIELD_EQ("IORd", "io_write")) {
		FIELD("IOMsgData", "hex8_t",
				BITS("%InterruptPendingMessage", 23, 16));
	}
	if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		FIELD("SmiOnCmpHalt", "yesno_t",
				BITS("%InterruptPendingMessage", 27));
		FIELD("C1eOnCmpHalt", "yesno_t",
				BITS("%InterruptPendingMessage", 28));
	}

	CLOSE_SCOPE(); // interrupt_pending_msg

	OPEN_SCOPE("smm");

	REG64("%SMM_BASE", 0xc0010111);
	FIELD("SMM_BASE", "addr32_t", BITS("%SMM_BASE", 31, 0));

	REG64("%SMM_ADDR", 0xc0010112);
	FIELD("ADDR", "addr64_t",
			BITS("%SMM_ADDR", 39, 17) + BITS("%0", 16, 0));

	REG64("%SMM_MASK", 0xc0010113);
	FIELD("AValid", "enabledisable_t", BITS("%SMM_MASK", 0));
	FIELD("TValid", "enabledisable_t", BITS("%SMM_MASK", 1));
	FIELD("AClose", "yesno_t", BITS("%SMM_MASK", 2));
	FIELD("TClose", "yesno_t", BITS("%SMM_MASK", 3));
	FIELD("AMTypteIcWc", "yesno_t", BITS("%SMM_MASK", 4));
	FIELD("TMTypeIoWc", "yesno_t", BITS("%SMM_MASK", 5));
	FIELD("AMTypeDram", "mtrr_type_t", BITS("%SMM_MASK", 10, 8));
	FIELD("TMTypeDram", "mtrr_type_t", BITS("%SMM_MASK", 14, 12));
	FIELD("MASK", "addr64_t",
			BITS("%SMM_MASK", 39, 17) + BITS("%0", 16, 0));

	CLOSE_SCOPE(); // smm

	if (FIELD_EQ("$core/k8_rev", "rev_f")) {
		OPEN_SCOPE("vm");

		REG64("%VM_CR", 0xc0010114);
		FIELD("dpd", "disableenable_t", BITS("%VM_CR", 0));
		FIELD("r_init", ANON_BOOL("init_translated_to_sx_int",
					  "normal"),
				BITS("%VM_CR", 1));
		FIELD("dis_a20m", "disableenable_t", BITS("%VM_CR", 2));
		FIELD("LOCK", "yesno_t", BITS("%VM_CR", 3));
		FIELD("SVME_DISABLE", "disableenable_t", BITS("%VM_CR", 4));

		FIELD("VM_HSAVE_PA", "addr64_t", REG64(0xc0010117));

		CLOSE_SCOPE(); // vm
	}

	OPEN_SCOPE("mc");

	OPEN_SCOPE("global");

	REG64("%MCG_CAP", 0x179);
	REG64("%MCG_STATUS", 0x17A);
	REG64("%MCG_CTL", 0x17B);

	FIELD("Count", ANON_INT("banks"), BITS("%MCG_CAP", 7, 0));
	FIELD("MCG_CTL_P", "yesno_t", BITS("%MCG_CAP", 8, 0));
	FIELD("RIPV", "yesno_t", BITS("%MCG_STATUS", 0));
	FIELD("EIPV", "yesno_t", BITS("%MCG_STATUS", 1));
	FIELD("MCIP", "yesno_t", BITS("%MCG_STATUS", 2));
	FIELD("DCE", "enabledisable_t", BITS("%MCG_CTL", 0));
	FIELD("ICE", "enabledisable_t", BITS("%MCG_CTL", 1));
	FIELD("BUE", "enabledisable_t", BITS("%MCG_CTL", 2));
	FIELD("LSE", "enabledisable_t", BITS("%MCG_CTL", 3));
	FIELD("NBE", "enabledisable_t", BITS("%MCG_CTL", 4));

	CLOSE_SCOPE(); // global

	OPEN_SCOPE("data_cache");

	REG64("%MC0_CTL", 0x400);
	REG64("%MC0_STATUS", 0x401);
	REG64("%MC0_ADDR", 0X402);
	// REG64("%MC0_MISC", 0X403); // unsupported on AMD

	FIELD("ECCI", "yesno_t", BITS("%MC0_CTL", 0));
	FIELD("ECCM", "yesno_t", BITS("%MC0_CTL", 1));
	FIELD("DECC", "yesno_t", BITS("%MC0_CTL", 2));
	FIELD("DMTP", "yesno_t", BITS("%MC0_CTL", 3));
	FIELD("DSTP", "yesno_t", BITS("%MC0_CTL", 4));
	FIELD("L1TP", "yesno_t", BITS("%MC0_CTL", 5));
	FIELD("L2TP", "yesno_t", BITS("%MC0_CTL", 6));
	FIELD("ERR_CODE", "hex16_t", BITS("%MC0_STATUS", 15, 0));
	if (FIELD_TEST("ERR_CODE", 0x0800)) {
		// Bus error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC0_STATUS", 1, 0));
		FIELD("ERR_CODE_MemoryOrIO",
				ANON_ENUM(KV("memory_access", 0),
					  KV("io_access", 2),
					  KV("generic", 3)),
				BITS("%MC0_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC0_STATUS", 7, 4));
		FIELD("ERR_CODE_Timeout", "yesno_t",
				BITS("%MC0_STATUS", 8));
		FIELD("ERR_CODE_ParticipationProcessor",
				ANON_ENUM(KV("local_node_originated", 0),
					  KV("local_node_responded", 1),
					  KV("local_node_third_party", 2),
					  KV("generic", 3)),
				BITS("%MC0_STATUS", 10, 9));
	} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
		// Memory error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC0_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC0_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC0_STATUS", 7, 4));
	} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
		// TLB error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC0_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC0_STATUS", 3, 2));
	}
	FIELD("EXT_ERR_CODE", ANON_ENUM(KV("phys_array", 0),
					KV("virt_array", 1)),
			BITS("%MC0_STATUS", 19, 16));
	FIELD("SCRUB", "yesno_t", BITS("%MC0_STATUS", 40));
	FIELD("UECC", "yesno_t", BITS("%MC0_STATUS", 45));
	FIELD("CECC", "yesno_t", BITS("%MC0_STATUS", 46));
	FIELD("SYND", "hex8_t", BITS("%MC0_STATUS", 54, 47));
	FIELD("PCC", "yesno_t", BITS("%MC0_STATUS", 57));
	FIELD("ADDRV", "yesno_t", BITS("%MC0_STATUS", 58));
	FIELD("MISCV", "yesno_t", BITS("%MC0_STATUS", 59));
	FIELD("EN", "yesno_t", BITS("%MC0_STATUS", 60));
	FIELD("UC", "yesno_t", BITS("%MC0_STATUS", 61));
	FIELD("OVER", "yesno_t", BITS("%MC0_STATUS", 62));
	FIELD("VAL", "yesno_t", BITS("%MC0_STATUS", 63));
	if (FIELD_EQ("ADDRV", 1)) {
		// MC0_ADDR bits valid if ADDRV set
		// TODO: what datatype for all of these?
		if (FIELD_TEST("ERR_CODE", 0x0800)) {
			// Bus error
			// System Data / line-fill
			FIELD("Address", "addr32_t",
					BITS("%MC0_ADDR", 39, 6));
		} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
			// Memory error
			if (FIELD_EQ("ERR_CODE_CacheLevel", "level_2")) {
				// L2 Cache Data / line-fill
				FIELD("Address", "addr32_t",
						BITS("%MC0_ADDR", 39, 6));
			} else if (FIELD_EQ("ERR_CODE_MemoryTransactionType",
					    "generic_error")) {
				// Data Array / scrub
				FIELD("Address", "hex_t",
						BITS("%MC0_ADDR", 11, 3));
			} else if ((FIELD_EQ("ERR_CODE_MemoryTransactionType",
					     "data_read") ||
				    FIELD_EQ("ERR_CODE_MemoryTransactionType",
					     "data_write"))) {
				// Data Array / load/store
				// or, Data Tag Array / load/store
				FIELD("Address", "hex_t",
						BITS("%MC0_ADDR", 39, 3));
			} else if ((FIELD_EQ("ERR_CODE_MemoryTransactionType",
					     "evict") ||
				    FIELD_EQ("ERR_CODE_MemoryTransactionType",
					     "snoop"))) {
				// Data Array / snoop/victim
				// or, Snoop Tag Array / snoop/victim
				FIELD("Address", "hex_t",
						BITS("%MC0_ADDR", 11, 6));
				
			}
		} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
			// TLB error
			// L1 Data TLB / load/store
			// or, L2 Data TLB / load/store
			FIELD("Address", "hex_t", BITS("%MC0_ADDR", 47, 12));
		}

	}

	CLOSE_SCOPE(); // data_cache

	OPEN_SCOPE("instr_cache");

	REG64("%MC1_CTL", 0X404);
	REG64("%MC1_STATUS", 0X405);
	REG64("%MC1_ADDR", 0X406);
	// REG64("%MC1_MISC", 0X407); // unsupported on AMD

	FIELD("ECCI", "yesno_t", BITS("%MC1_CTL", 0));
	FIELD("ECCM", "yesno_t", BITS("%MC1_CTL", 1));
	FIELD("IDP", "yesno_t", BITS("%MC1_CTL", 2));
	FIELD("IMTP", "yesno_t", BITS("%MC1_CTL", 3));
	FIELD("ISTP", "yesno_t", BITS("%MC1_CTL", 4));
	FIELD("L1TP", "yesno_t", BITS("%MC1_CTL", 5));
	FIELD("L2TP", "yesno_t", BITS("%MC1_CTL", 6));
	FIELD("RDDE", "yesno_t", BITS("%MC1_CTL", 9));
	FIELD("ERR_CODE", "hex16_t", BITS("%MC1_STATUS", 15, 0));
	if (FIELD_TEST("ERR_CODE", 0x0800)) {
		// Bus error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC1_STATUS", 1, 0));
		FIELD("ERR_CODE_MemoryOrIO",
				ANON_ENUM(KV("memory_access", 0),
					  KV("io_access", 2),
					  KV("generic", 3)),
				BITS("%MC1_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC1_STATUS", 7, 4));
		FIELD("ERR_CODE_Timeout", "yesno_t",
				BITS("%MC1_STATUS", 8));
		FIELD("ERR_CODE_ParticipationProcessor",
				ANON_ENUM(KV("local_node_originated", 0),
					  KV("local_node_responded", 1),
					  KV("local_node_third_party", 2),
					  KV("generic", 3)),
				BITS("%MC1_STATUS", 10, 9));
	} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
		// Memory error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC1_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC1_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC1_STATUS", 7, 4));
	} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
		// TLB error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC1_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC1_STATUS", 3, 2));
	}
	FIELD("EXT_ERR_CODE", ANON_ENUM(KV("phys_array", 0),
					KV("virt_array", 1)),
			BITS("%MC1_STATUS", 19, 16));
	FIELD("SCRUB", "yesno_t", BITS("%MC1_STATUS", 40));
	FIELD("UECC", "yesno_t", BITS("%MC1_STATUS", 45));
	FIELD("CECC", "yesno_t", BITS("%MC1_STATUS", 46));
	FIELD("SYND", "hex8_t", BITS("%MC1_STATUS", 54, 47));
	FIELD("PCC", "yesno_t", BITS("%MC1_STATUS", 57));
	FIELD("ADDRV", "yesno_t", BITS("%MC1_STATUS", 58));
	FIELD("MISCV", "yesno_t", BITS("%MC1_STATUS", 59));
	FIELD("EN", "yesno_t", BITS("%MC1_STATUS", 60));
	FIELD("UC", "yesno_t", BITS("%MC1_STATUS", 61));
	FIELD("OVER", "yesno_t", BITS("%MC1_STATUS", 62));
	FIELD("VAL", "yesno_t", BITS("%MC1_STATUS", 63));
	if (FIELD_EQ("ADDRV", 1)) {
		// MC1_ADDR bits valid if ADDRV set
		// TODO: what datatype for all of these?
		if (FIELD_TEST("ERR_CODE", 0x0800)) {
			// Bus error
			// System Data / line-fill
			FIELD("Address", "addr32_t",
					BITS("%MC1_ADDR", 39, 6));
		} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
			// Memory error
			if (FIELD_EQ("ERR_CODE_CacheLevel", "level_2")) {
				// L2 Cache Data / line-fill
				FIELD("Address", "addr32_t",
						BITS("%MC1_ADDR", 39, 6));
			} else if (FIELD_EQ("UC", 1)) {
				// Snoop Tag Array / snoop/victim
				FIELD("Address", "hex_t",
						BITS("%MC1_ADDR", 39, 6));
			} 
			// TODO: no way to tell "Tag Load" and "IC Data Load"
			// apart, but they have different valid address
			// bits...how do I deal with this?
		} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
			// TLB error
			// L1 TLB or L2 TLB / code read, 4 KB page
			// TODO: how do I determine whether page size is
			// 4 KB or 2 MB?
			FIELD("Address", "hex_t", BITS("%MC1_ADDR", 47, 12));
		}
	}

	CLOSE_SCOPE(); // instr_cache

	OPEN_SCOPE("bus");

	REG64("%MC2_CTL", 0X408);
	REG64("%MC2_STATUS", 0X409);
	REG64("%MC2_ADDR", 0X40a);
	// REG64("%MC2_MISC", 0X40b); // unsupported

	FIELD("S_RDE_HP", "yesno_t", BITS("%MC2_CTL", 0));
	FIELD("S_RED_TLB", "yesno_t", BITS("%MC2_CTL", 1));
	FIELD("S_RDE_ALL", "yesno_t", BITS("%MC2_CTL", 2));
	FIELD("S_ECC1_TLB", "yesno_t", BITS("%MC2_CTL", 3));
	FIELD("S_ECC1_HP", "yesno_t", BITS("%MC2_CTL", 4));
	FIELD("S_ECCM_TLB", "yesno_t", BITS("%MC2_CTL", 5));
	FIELD("S_ECCM_HP", "yesno_t", BITS("%MC2_CTL", 6));
	FIELD("L2T_PAR_ICDC", "yesno_t", BITS("%MC2_CTL", 7));
	FIELD("L2T_PAR_TLB", "yesno_t", BITS("%MC2_CTL", 8));
	FIELD("L2T_PAR_SNP", "yesno_t", BITS("%MC2_CTL", 9));
	FIELD("L2T_PAR_CPB", "yesno_t", BITS("%MC2_CTL", 10));
	FIELD("L2T_PAR_SCR", "yesno_t", BITS("%MC2_CTL", 11));
	FIELD("L2D_ECC1_TLB", "yesno_t", BITS("%MC2_CTL", 12));
	FIELD("L2D_ECC1_SNP", "yesno_t", BITS("%MC2_CTL", 13));
	FIELD("L2D_ECC1_CPB", "yesno_t", BITS("%MC2_CTL", 14));
	FIELD("L2D_ECCM_TLB", "yesno_t", BITS("%MC2_CTL", 15));
	FIELD("L2D_ECCM_SNP", "yesno_t", BITS("%MC2_CTL", 16));
	FIELD("L2D_ECCM_CPB", "yesno_t", BITS("%MC2_CTL", 17));
	FIELD("L2T_ECC1_SCR", "yesno_t", BITS("%MC2_CTL", 18));
	FIELD("L2T_ECCM_SCR", "yesno_t", BITS("%MC2_CTL", 19));
	FIELD("ERR_CODE", "hex16_t", BITS("%MC2_STATUS", 15, 0));
	if (FIELD_TEST("ERR_CODE", 0x0800)) {
		// Bus error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC2_STATUS", 1, 0));
		FIELD("ERR_CODE_MemoryOrIO",
				ANON_ENUM(KV("memory_access", 0),
					  KV("io_access", 2),
					  KV("generic", 3)),
				BITS("%MC2_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC2_STATUS", 7, 4));
		FIELD("ERR_CODE_Timeout", "yesno_t",
				BITS("%MC2_STATUS", 8));
		FIELD("ERR_CODE_ParticipationProcessor",
				ANON_ENUM(KV("local_node_originated", 0),
					  KV("local_node_responded", 1),
					  KV("local_node_third_party", 2),
					  KV("generic", 3)),
				BITS("%MC2_STATUS", 10, 9));
	} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
		// Memory error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC2_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC2_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC2_STATUS", 7, 4));
	} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
		// TLB error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC2_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC2_STATUS", 3, 2));
	}
	FIELD("EXT_ERR_CODE", ANON_ENUM(KV("bus_or_cache_data_array_error", 0),
					KV("cache_tag_array_error", 2)),
			BITS("%MC2_STATUS", 19, 16));
	FIELD("SCRUB", "yesno_t", BITS("%MC2_STATUS", 40));
	FIELD("UECC", "yesno_t", BITS("%MC2_STATUS", 45));
	FIELD("CECC", "yesno_t", BITS("%MC2_STATUS", 46));
	FIELD("SYND", "hex8_t", BITS("%MC2_STATUS", 54, 47));
	FIELD("PCC", "yesno_t", BITS("%MC2_STATUS", 57));
	FIELD("ADDRV", "yesno_t", BITS("%MC2_STATUS", 58));
	FIELD("MISCV", "yesno_t", BITS("%MC2_STATUS", 59));
	FIELD("EN", "yesno_t", BITS("%MC2_STATUS", 60));
	FIELD("UC", "yesno_t", BITS("%MC2_STATUS", 61));
	FIELD("OVER", "yesno_t", BITS("%MC2_STATUS", 62));
	FIELD("VAL", "yesno_t", BITS("%MC2_STATUS", 63));
	if (FIELD_EQ("ADDRV", 1)) {
		// MC2_ADDR bits valid if ADDRV set
		// TODO: what datatype for all of these?
		if (FIELD_TEST("ERR_CODE", 0x0800 | 0x0100) &&
		    FIELD_EQ("ERR_CODE_CacheLevel", "level_2")) {
			// L2 Cache error
			if (FIELD_EQ("EXT_ERR_CODE", 
				     "bus_or_cache_data_array_error")) {
				FIELD("Address", "hex_t",
						BITS("%MC2_ADDR", 39, 6));
			} else if (FIELD_EQ("EXT_ERR_CODE",
					    "cache_tag_array_error")) {
				// TODO: how do I determine L2 cache size?
			}
		}
		// TODO: how do I detect a system address out of range error?
	}

	CLOSE_SCOPE(); // bus

	OPEN_SCOPE("ls");

	REG64("%MC3_CTL", 0X40c);
	REG64("%MC3_STATUS", 0X40d);
	REG64("%MC3_ADDR", 0X40e);
	// REG64("%MC3_MISC", 0X40f); // unsupported on AMD

	FIELD("S_RDE_L", "yesno_t", BITS("%MC3_CTL", 0));
	FIELD("S_RDE_S", "yesno_t", BITS("%MC3_CTL", 0));
	FIELD("ERR_CODE", "hex16_t", BITS("%MC3_STATUS", 15, 0));
	if (FIELD_TEST("ERR_CODE", 0x0800)) {
		// Bus error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC3_STATUS", 1, 0));
		FIELD("ERR_CODE_MemoryOrIO",
				ANON_ENUM(KV("memory_access", 0),
					  KV("io_access", 2),
					  KV("generic", 3)),
				BITS("%MC3_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC3_STATUS", 7, 4));
		FIELD("ERR_CODE_Timeout", "yesno_t",
				BITS("%MC3_STATUS", 8));
		FIELD("ERR_CODE_ParticipationProcessor",
				ANON_ENUM(KV("local_node_originated", 0),
					  KV("local_node_responded", 1),
					  KV("local_node_third_party", 2),
					  KV("generic", 3)),
				BITS("%MC3_STATUS", 10, 9));
	} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
		// Memory error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC3_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC3_STATUS", 3, 2));
		FIELD("ERR_CODE_MemoryTransactionType",
				ANON_ENUM(KV("generic_error", 0),
					  KV("generic_read", 1),
					  KV("generic_write", 2),
					  KV("data_read", 3),
					  KV("data_write", 4),
					  KV("instruction_fetch", 5),
					  KV("prefetch", 6),
					  KV("evict", 7),
					  KV("snoop", 8)),
				BITS("%MC3_STATUS", 7, 4));
	} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
		// TLB error
		FIELD("ERR_CODE_CacheLevel",
				ANON_ENUM(KV("level_0", 0),
					  KV("level_1", 1),
					  KV("level_2", 2),
					  KV("generic", 3)),
				BITS("%MC3_STATUS", 1, 0));
		FIELD("ERR_CODE_TransactionType",
				ANON_ENUM(KV("instruction", 0),
					  KV("data", 1),
					  KV("generic", 2)),
				BITS("%MC3_STATUS", 3, 2));
	}
	FIELD("SCRUB", "yesno_t", BITS("%MC3_STATUS", 40));
	FIELD("UECC", "yesno_t", BITS("%MC3_STATUS", 45));
	FIELD("CECC", "yesno_t", BITS("%MC3_STATUS", 46));
	FIELD("SYND", "hex8_t", BITS("%MC3_STATUS", 54, 47));
	FIELD("PCC", "yesno_t", BITS("%MC3_STATUS", 57));
	FIELD("ADDRV", "yesno_t", BITS("%MC3_STATUS", 58));
	FIELD("MISCV", "yesno_t", BITS("%MC3_STATUS", 59));
	FIELD("EN", "yesno_t", BITS("%MC3_STATUS", 60));
	FIELD("UC", "yesno_t", BITS("%MC3_STATUS", 61));
	FIELD("OVER", "yesno_t", BITS("%MC3_STATUS", 62));
	FIELD("VAL", "yesno_t", BITS("%MC3_STATUS", 63));
	if (FIELD_EQ("ADDRV", 1)) {
		// MC3_ADDR bits valid if ADDRV set
		// TODO: how do I detect a system address out of range error?
	}

	CLOSE_SCOPE(); // ls

	OPEN_SCOPE("nb");

	CLOSE_SCOPE(); // nb

	CLOSE_SCOPE(); // mc

	OPEN_SCOPE("debug");

	REG64("%DebugCtl", 0x1d9);
	BOOL("pb_t", "breakpoint_info", "performance_monitor_info");

	FIELD("LastBranchFromIP", "addr64_t", REG64(0x1db));
	FIELD("LastBranchToIP", "addr64_t", REG64(0x1dc));
	FIELD("LastExceptionFromIP", "addr64_t", REG64(0x1dd));
	FIELD("LastExceptionToIP", "addr64_t", REG64(0x1de));
	FIELD("LBR", "yesno_t", BITS("%DebugCtl", 0));
	FIELD("BTF", "yesno_t", BITS("%DebugCtl", 1));
	FIELD("PB0", "pb_t", BITS("%DebugCtl", 2));
	FIELD("PB1", "pb_t", BITS("%DebugCtl", 3));
	FIELD("PB2", "pb_t", BITS("%DebugCtl", 4));
	FIELD("PB3", "pb_t", BITS("%DebugCtl", 5));

	CLOSE_SCOPE(); // debug

	CLOSE_SCOPE(); // msr
}

static void
k8_cpu(const pp_value &node, const pp_value &ncores, const pp_value &cpu)
{
	OPEN_SCOPE("core[]");
	BOOKMARK("core");

	k8_cpuid(node * ncores + cpu);

	// add a field for k8 revision to each core
	ENUM("k8_rev_t",
		KV("rev_e", 'e'),
		KV("rev_f", 'f'),
		KV("unknown", 0));
	if (FIELD_EQ("cpuid/family", 0xf)) {
		if (FIELD_GE("cpuid/model", 0x40)) {
			FIELD("k8_rev", "k8_rev_t", 'f');
		} else if (FIELD_GE("cpuid/model", 0x20)) {
			FIELD("k8_rev", "k8_rev_t", 'e');
		} else {
			FIELD("k8_rev", "k8_rev_t", pp_value(0));
		}
	} else {
		FIELD("k8_rev", "k8_rev_t", pp_value(0));
	}

	k8_msr(node * ncores + cpu);

	CLOSE_SCOPE(); // core[]
}

static void
k8_ht_config(const pp_value &seg, const pp_value &bus,
             const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("ht_config", seg, bus, dev, func);

	//
	// HT routing table registers
	//

	OPEN_SCOPE("ht_route");

	// each HT route is a 4 bit bitmask
	BITMASK("ht_route_t",
			KV("self",  0),
			KV("link0", 1),
			KV("link1", 2),
			KV("link2", 3));

	for (int addr = 0x40; addr < 0x60; addr += 4) {
		OPEN_SCOPE("node[]");

		REG32("%ht_route", addr);
		FIELD("RQRoute", "ht_route_t", BITS("%ht_route", 3, 0));
		FIELD("RPRoute", "ht_route_t", BITS("%ht_route", 11, 8));
		FIELD("BCRoute", "ht_route_t", BITS("%ht_route", 19, 16));

		CLOSE_SCOPE();
	}

	CLOSE_SCOPE();

	//
	// Node ID register
	//
	REG32("%node_id", 0x60);
	FIELD("NodeId", "int_t", BITS("%node_id", 2, 0));
	FIELD("NodeCnt", ANON_XFORM("int_t", LAMBDA(_1+1), LAMBDA(_1-1)),
			BITS("%node_id", 6, 4));
	FIELD("SbNode", "int_t", BITS("%node_id", 10, 8));
	FIELD("LkNode", "int_t", BITS("%node_id", 14, 12));
	FIELD("CpuCnt", ANON_XFORM("int_t", LAMBDA(_1+1), LAMBDA(_1-1)),
			BITS("%node_id", 19, 16));

	//
	// Unit ID register
	//
	REG32("%unit_id", 0x64);
	FIELD("C0Unit", "int_t", BITS("%unit_id", 1, 0));
	FIELD("C1Unit", "int_t", BITS("%unit_id", 3, 2));
	FIELD("MctUnit", "int_t", BITS("%unit_id", 5, 4));
	FIELD("HbUnit", "int_t", BITS("%unit_id", 7, 6));
	FIELD("SbLink", "int_t", BITS("%unit_id", 10, 8));

	//
	// Link transaction control register
	//
	REG32("%link_txn_ctrl", 0x68);
	FIELD("DisRdBP", "yesno_t", BITS("%link_txn_ctrl", 0));
	FIELD("DisRdDwP", "yesno_t", BITS("%link_txn_ctrl", 1));
	FIELD("DisWrBP", "yesno_t", BITS("%link_txn_ctrl", 2));
	FIELD("DisWrDwP", "yesno_t", BITS("%link_txn_ctrl", 3));
	FIELD("DisMTS", "yesno_t", BITS("%link_txn_ctrl", 4));
	FIELD("Cpu1En", "yesno_t", BITS("%link_txn_ctrl", 5));
	FIELD("CPUReqPassPW", "yesno_t", BITS("%link_txn_ctrl", 6));
	FIELD("CPURdRspPassPW", "yesno_t", BITS("%link_txn_ctrl", 7));
	FIELD("DisPMemC", "yesno_t", BITS("%link_txn_ctrl", 8));
	FIELD("DisRmtPMemC", "yesno_t", BITS("%link_txn_ctrl", 9));
	FIELD("DisFillP", "yesno_t", BITS("%link_txn_ctrl", 10));
	FIELD("RespPassPW", "yesno_t", BITS("%link_txn_ctrl", 11));
	FIELD("BufRelPri", ANON_ENUM(
				KV("64", 0),
				KV("16", 1),
				KV("8",  2),
				KV("2",  3)),
			BITS("%link_txn_ctrl", 14, 13));
	FIELD("LimitCldtCfg", "yesno_t", BITS("%link_txn_ctrl", 15));
	FIELD("LintEn", "yesno_t", BITS("%link_txn_ctrl", 16));
	FIELD("ApicExtBrdCst", ANON_ENUM(
				KV("0x0f", 0),
				KV("0xff", 1)),
			BITS("%link_txn_ctrl", 17));
	FIELD("ApicExtId", "yesno_t", BITS("%link_txn_ctrl", 18));
	FIELD("ApicExtSpur", "yesno_t", BITS("%link_txn_ctrl", 19));
	FIELD("SeqIdSrcNodeEn", "yesno_t", BITS("%link_txn_ctrl", 20));
	FIELD("DsNpReqLmt", ANON_ENUM(
				KV("no limit", 0),
				KV("1", 1),
				KV("4", 2),
				KV("8", 3)),
			BITS("%link_txn_ctrl", 22, 21));
	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("InstallStateS", "yesno_t", BITS("%link_txn_ctrl", 23));
	}
	FIELD("DispRefModeEn", "yesno_t", BITS("%link_txn_ctrl", 24));
	FIELD("CHtExtAddrEn", "yesno_t", BITS("%link_txn_ctrl", 25));
	FIELD("EnPReqHiPriTblWlk", "yesno_t", BITS("%link_txn_ctrl", 31));

	//
	// Link initialization control register
	//
	REG32("%link_init_ctrl", 0x68);
	FIELD("RouteTblDis", "yesno_t", BITS("%link_init_ctrl", 0));
	FIELD("ReqDis", "yesno_t", BITS("%link_init_ctrl", 1));
	FIELD("DefLnk", ANON_ENUM(
				KV("link0", 0),
				KV("link1", 1),
				KV("link2", 2),
				KV("link3", 3),
				KV("self", 4)),
			BITS("%link_init_ctrl", 3, 2));
	FIELD("ColdRstDet", "yesno_t", BITS("%link_init_ctrl", 4));
	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("BiosRstDet", "hex_t",
				BITS("%link_init_ctrl", 5) +
				BITS("%link_init_ctrl", 10, 9));
	} else  {
		FIELD("BiosRstDet", "hex_t", BITS("%link_init_ctrl", 5));
	}
	FIELD("InitDet", "yesno_t", BITS("%link_init_ctrl", 6));
	FIELD("DefSubLnk", "int_t", BITS("%link_init_ctrl", 8));
	FIELD("Tr0Double", "yesno_t", BITS("%link_init_ctrl", 20));

	// The HyperTransport capability blocks are handled by the generic
	// PCI code.

	//
	// k8-specific HT registers
	//
	for (int addr = 0x90; addr < 0x100; addr += 0x20) {
		OPEN_SCOPE("ldt[]");

		//
		// HT buffer count
		//
		REG32("%ldt_buf_count", addr);
		FIELD("Req", "int_t", BITS("%ldt_buf_count", 3, 0));
		FIELD("PReq", "int_t", BITS("%ldt_buf_count", 7, 4));
		FIELD("Rsp", "int_t", BITS("%ldt_buf_count", 11, 8));
		FIELD("Probe", "int_t", BITS("%ldt_buf_count", 15, 12));
		FIELD("ReqD", "int_t", BITS("%ldt_buf_count", 18, 16));
		FIELD("PReqD", "int_t", BITS("%ldt_buf_count", 22, 20));
		FIELD("RspD", "int_t", BITS("%ldt_buf_count", 26, 24));

		//
		// HT bus nums
		//
		REG32("%ldt_bus_nums", addr+4);
		FIELD("PriBusNum", "int_t", BITS("%ldt_bus_nums", 7, 0));
		FIELD("SecBusNum", "int_t", BITS("%ldt_bus_nums", 15, 8));
		FIELD("SubBusNum", "int_t", BITS("%ldt_bus_nums", 23, 16));

		//
		// HT type
		//
		REG32("%ldt_type", addr+8);
		FIELD("LinkCon", "yesno_t", BITS("%ldt_type", 0));
		FIELD("InitComplete", "yesno_t", BITS("%ldt_type", 1));
		FIELD("NC", ANON_BOOL("noncoherent", "coherent"),
				BITS("%ldt_type", 2));
		FIELD("UniP_cLDT", ANON_BOOL("uniproc_coherent_nb", "normal"),
				BITS("%ldt_type", 3));
		FIELD("LinkConPend", "yesno_t", BITS("%ldt_type", 4));

		CLOSE_SCOPE();
	}

	CLOSE_SCOPE(); // ht_config
}

static void
k8_address_map(const pp_value &seg, const pp_value &bus,
               const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("address_map", seg, bus, dev, func+1);

	//
	// DRAM base/limit registers
	//
	for (int addr = 0x40; addr < 0x80; addr += 0x08) {
		OPEN_SCOPE("dram[]");

		REG32("%dram_base", addr);
		REG32("%dram_limit", addr + 0x04);

		FIELD("RE", "enabledisable_t", BITS("%dram_base", 0));
		FIELD("WE", "enabledisable_t", BITS("%dram_base", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("DRAMBase", "addr64_t",
			    BITS("%dram_base", 31, 16) + BITS("%0", 23, 0));
			FIELD("DRAMLimit", "addr64_t",
			    BITS("%dram_limit", 31, 16) + BITS("%1", 23, 0));
			FIELD("DstNode", "int_t", BITS("%dram_limit", 2, 0));
			FIELD("IntlvEn", ANON_ENUM(KV("none", 0),
					KV("a12", 1),
					KV("a12_a13", 3),
					KV("a12_a13_a14", 7)),
				BITS("%dram_base", 10, 8));
			FIELD("IntlvSel", ANON_BITMASK(
					KV("a12", 0),
					KV("a13", 1),
					KV("a14", 2)),
				BITS("%dram_limit", 10, 8));
		}

		CLOSE_SCOPE();
	}

	//
	// Memory-mapped I/O base/limit registers
	//
	for (int addr = 0x80; addr < 0xc0; addr += 0x08) {
		OPEN_SCOPE("mmio[]");

		REG32("%mmio_base", addr);
		REG32("%mmio_limit", addr + 0x04);

		FIELD("RE", "enabledisable_t", BITS("%mmio_base", 0));
		FIELD("WE", "enabledisable_t", BITS("%mmio_base", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("MMIOBase", "addr64_t",
			    BITS("%mmio_base", 31, 8) + BITS("%0", 15, 0));
			FIELD("MMIOLimit", "addr64_t",
			    BITS("%mmio_limit", 31, 8) + BITS("%1", 15, 0));
			FIELD("DstNode", "int_t", BITS("%mmio_limit", 2, 0));
			FIELD("DstLink", "int_t", BITS("%mmio_limit", 5, 4));
			FIELD("NP", "yesno_t", BITS("%mmio_limit", 7));
			FIELD("CpuDis", "yesno_t", BITS("%mmio_base", 2));
			FIELD("Lock", "yesno_t", BITS("%mmio_base", 3));
		}

		CLOSE_SCOPE();
	}

	//
	// PCI I/O base/limit registers
	//
	for (int addr = 0xc0; addr < 0xe0; addr += 0x08) {
		OPEN_SCOPE("pciio[]");

		REG32("%pciio_base", addr);
		REG32("%pciio_limit", addr + 0x04);

		FIELD("RE", "enabledisable_t", BITS("%pciio_base", 0));
		FIELD("WE", "enabledisable_t", BITS("%pciio_base", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("PCIIOBase", "addr64_t",
			    BITS("%pciio_base", 24, 12) + BITS("%0", 11, 0));
			FIELD("PCIIOLimit", "addr64_t",
			    BITS("%pciio_limit", 24, 12) + BITS("%1", 11, 0));
			FIELD("DstNode", "int_t", BITS("%pciio_limit", 2, 0));
			FIELD("DstLink", "int_t", BITS("%pciio_limit", 5, 4));
			FIELD("VE", "enabledisable_t", BITS("%pciio_base", 4));
			FIELD("IE", "enabledisable_t", BITS("%pciio_base", 5));
		}

		CLOSE_SCOPE();
	}

	//
	// Configuration base/limit registers
	//
	for (int addr = 0xe0; addr < 0xf0; addr += 0x04) {
		OPEN_SCOPE("config[]");

		REG32("%config", addr);
		FIELD("RE", "enabledisable_t", BITS("%config", 0));
		FIELD("WE", "enabledisable_t", BITS("%config", 1));
		if (FIELD_BOOL("RE") || FIELD_BOOL("WE")) {
			FIELD("DstNode", "int_t", BITS("%config", 6, 4));
			FIELD("DstLink", "int_t", BITS("%config", 9, 8));
			FIELD("BusNumBase", "int_t", BITS("%config", 23, 16));
			FIELD("BusNumLimit", "int_t", BITS("%config", 31, 24));
			FIELD("DevCmpEn", "enabledisable_t",
					BITS("%config", 2));
		}

		CLOSE_SCOPE();
	}

	//
	// DRAM hole address register
	//
	REG32("%DRAMHole", 0xf0);
	OPEN_SCOPE("dram_hole");

	FIELD("DramHoleValid", "truefalse_t", BITS("../%DRAMHole", 0));
	if (FIELD_BOOL("DramHoleValid")) {
		FIELD("DramHoleOffset", "hex32_t",
			BITS("../%DRAMHole", 15, 8) + BITS("%0", 23, 0));
		FIELD("DramHoleBase", "hex32_t",
			BITS("../%DRAMHole", 31, 24) + BITS("%0", 23, 0));
	}

	CLOSE_SCOPE();

	CLOSE_SCOPE(); // address_map
}

class dram_addtional_data_procs: public pp_rwprocs
{
    private:
	pp_value m_index;
	static const uint32_t DctAccessWrite = (1<<30);

    public:
	explicit dram_addtional_data_procs(pp_value index)
	    : m_index(index & pp_value(0x3fffffff))
	{
	}

	pp_value
	read(void) const
	{
		WRITE("../%dram_additional_data_offset", m_index);
		while (READ(BITS("../%dram_additional_data_offset", 31)) == 0)
			; // do nothing
		return READ("../%dram_additional_data_port");
	}

	void
	write(const pp_value &value) const
	{
		WRITE("../%dram_additional_data_port", value);
		WRITE("../%dram_additional_data_offset",
		      m_index | pp_value(DctAccessWrite));
		while (READ(BITS("../%dram_additional_data_offset", 31)) == 0)
			; // do nothing
	}
};

static void
dram_additional_data(const string &name, const pp_value &address)
{
	OPEN_SCOPE(name);

	//
	// Output driver compensation control
	//

	REG32("%output_driver_compensation_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x00)));
	FIELD("CkeDreStren", ANON_ENUM(
				KV("1.0x", 0),
				KV("1.25x", 1),
				KV("1.5x", 2),
				KV("2.0x", 3)),
			BITS("%output_driver_compensation_ctrl", 1, 0));
	FIELD("CsOdtDrvStren", ANON_ENUM(
				KV("1.0x", 0),
				KV("1.25x", 1),
				KV("1.5x", 2),
				KV("2.0x", 3)),
			BITS("%output_driver_compensation_ctrl", 5, 4));
	FIELD("AddrCmdDrvStren", ANON_ENUM(
				KV("1.0x", 0),
				KV("1.25x", 1),
				KV("1.5x", 2),
				KV("2.0x", 3)),
			BITS("%output_driver_compensation_ctrl", 9, 8));
	FIELD("ClkDrvStren", ANON_ENUM(
				KV("0.75x", 0),
				KV("1.0x", 1),
				KV("1.25x", 2),
				KV("1.5x", 3)),
			BITS("%output_driver_compensation_ctrl", 13, 12));
	FIELD("DataDrvStren", ANON_ENUM(
				KV("0.75x", 0),
				KV("1.0x", 1),
				KV("1.25x", 2),
				KV("1.5x", 3)),
			BITS("%output_driver_compensation_ctrl", 17, 16));
	FIELD("DqsDrvStren", ANON_ENUM(
				KV("0.75x", 0),
				KV("1.0x", 1),
				KV("1.25x", 2),
				KV("1.5x", 3)),
			BITS("%output_driver_compensation_ctrl", 21, 20));
	FIELD("ProcOdt", ANON_ENUM(
				KV("300 ohms", 0),
				KV("150 ohms", 1),
				KV("75 ohms", 2)),
			BITS("%output_driver_compensation_ctrl", 29, 28));

	//
	// Write data timing control
	//

	REG32("%write_data_timing_lo_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x01)));
	REG32("%write_data_timing_hi_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x02)));

	FIELD("WrDatTimeByte0", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_lo_ctrl", 5, 0));
	FIELD("WrDatTimeByte1", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_lo_ctrl", 13, 8));
	FIELD("WrDatTimeByte2", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_lo_ctrl", 21, 16));
	FIELD("WrDatTimeByte3", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_lo_ctrl", 29, 24));
	FIELD("WrDatTimeByte4", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_hi_ctrl", 5, 0));
	FIELD("WrDatTimeByte5", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_hi_ctrl", 13, 8));
	FIELD("WrDatTimeByte6", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_hi_ctrl", 21, 16));
	FIELD("WrDatTimeByte7", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_timing_hi_ctrl", 29, 24));

	REG32("%write_data_ecc_timing_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x03)));
	FIELD("WrChkTime", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_ecc_timing_ctrl", 5, 0));

	//
	// Address timing control
	//

	REG32("%address_timing_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x04)));
	FIELD("CkeFineDelay", ANON_INT("/64 MEMCLK"),
	      BITS("%address_timing_ctrl", 4, 0));
	FIELD("CkeSetup", ANON_BOOL("1 MEMCLK", "1/2 MEMCLK"),
	      BITS("%address_timing_ctrl", 5));
	FIELD("CsOdtFineDelay", ANON_INT("/64 MEMCLK"),
	      BITS("%address_timing_ctrl", 12, 8));
	FIELD("CsOdtSetup", ANON_BOOL("1 MEMCLK", "1/2 MEMCLK"),
	      BITS("%address_timing_ctrl", 13));
	FIELD("AddrCmdFineDelay", ANON_INT("/64 MEMCLK"),
	      BITS("%address_timing_ctrl", 20, 16));
	FIELD("AddrCmdSetup", ANON_BOOL("1 MEMCLK", "1/2 MEMCLK"),
	      BITS("%address_timing_ctrl", 21));
	FIELD("AtcDllMaxPhases", ANON_BOOL("64 phases", "32 phases"),
	      BITS("%address_timing_ctrl", 28));

	//
	// Read DQS timing control
	//

	REG32("%read_dqs_timing_lo_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x05)));
	REG32("%read_dqs_timing_hi_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x06)));

	FIELD("RdDqsTimeByte0", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_lo_ctrl", 5, 0));
	FIELD("RdDqsTimeByte1", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_lo_ctrl", 13, 8));
	FIELD("RdDqsTimeByte2", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_lo_ctrl", 21, 16));
	FIELD("RdDqsTimeByte3", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_lo_ctrl", 29, 24));
	FIELD("RdDqsTimeByte4", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_hi_ctrl", 5, 0));
	FIELD("RdDqsTimeByte5", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_hi_ctrl", 13, 8));
	FIELD("RdDqsTimeByte6", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_hi_ctrl", 21, 16));
	FIELD("RdDqsTimeByte7", ANON_INT("/96 MEMCLK"),
	      BITS("%read_dqs_timing_hi_ctrl", 29, 24));

	REG32("%read_dqs_ecc_timing_ctrl",
	      PROCS(dram_addtional_data_procs(address + 0x07)));
	FIELD("RdDqsTimeCheck", ANON_INT("/96 MEMCLK"),
	      BITS("%write_data_ecc_timing_ctrl", 5, 0));

	//
	// DQS receiver enable timing control
	//

	REG32("%dqs_receiver_enable_timing_ctrl[]",
	      PROCS(dram_addtional_data_procs(address + 0x10)));
	FIELD("DqsRcvEnDelay[]", ANON_XFORM(ANON_INT("ps"),
				LAMBDA(_1 * 50), LAMBDA(_1)),
			BITS("%dqs_receiver_enable_timing_ctrl[$]", 7, 0));
	REG32("%dqs_receiver_enable_timing_ctrl[]",
	      PROCS(dram_addtional_data_procs(address + 0x13)));
	FIELD("DqsRcvEnDelay[]", ANON_XFORM(ANON_INT("ps"),
				LAMBDA(_1 * 50), LAMBDA(_1)),
			BITS("%dqs_receiver_enable_timing_ctrl[$]", 7, 0));
	REG32("%dqs_receiver_enable_timing_ctrl[]",
	      PROCS(dram_addtional_data_procs(address + 0x16)));
	FIELD("DqsRcvEnDelay[]", ANON_XFORM(ANON_INT("ps"),
				LAMBDA(_1 * 50), LAMBDA(_1)),
			BITS("%dqs_receiver_enable_timing_ctrl[$]", 7, 0));
	REG32("%dqs_receiver_enable_timing_ctrl[]",
	      PROCS(dram_addtional_data_procs(address + 0x19)));
	FIELD("DqsRcvEnDelay[]", ANON_XFORM(ANON_INT("ps"),
				LAMBDA(_1 * 50), LAMBDA(_1)),
			BITS("%dqs_receiver_enable_timing_ctrl[$]", 7, 0));

	CLOSE_SCOPE();
}

static void
k8_dram_controller(const pp_value &seg, const pp_value &bus,
                   const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("dram_ctrl", seg, bus, dev, func+2);

	//
	// DRAM CS Base registers
	//

	for (int addr = 0x40; addr < 0x60; addr += 0x04) {
		OPEN_SCOPE("cs_base[]");

		REG32("%cs_base", addr);

		if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
			FIELD("CSBE", "enabledisable_t",
					BITS("%cs_base", 0));
			FIELD("BaseAddr", "addr64_t",
					BITS("%cs_base", 31, 21) +
					BITS("%0", 24, 20) +
					BITS("%cs_base", 15, 9) +
					BITS("%0", 12, 0));
		} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
			FIELD("CSEnable", "enabledisable_t",
					BITS("%cs_base", 0));
			FIELD("Spare", "yesno_t",
					BITS("%cs_base", 1));
			FIELD("TestFail", "yesno_t",
					BITS("%cs_base", 2));
			FIELD("BaseAddr", "addr64_t",
					BITS("%cs_base", 28, 19) +
					BITS("%0", 26, 22) +
					BITS("%cs_base", 13, 5) +
					BITS("%0", 12, 0));
		}

		CLOSE_SCOPE();
	}

	//
	// DRAM CS Mask registers
	//
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		for (int addr = 0x60; addr < 0x80; addr += 0x04) {
			OPEN_SCOPE("cs_mask[]");
			REG32("%cs_mask", addr);
			FIELD("AddrMask", "hex64_t",
					BITS("%cs_mask", 29, 21) +
					BITS("%0", 24, 20) +
					BITS("%cs_mask", 15, 9) +
					BITS("%0", 12, 0)); 
			CLOSE_SCOPE();
		}
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		for (int addr = 0x60; addr <= 0x6C; addr += 0x04) {
			OPEN_SCOPE("cs_mask[]");
			REG32("%cs_mask", addr);
			FIELD("AddrMask", "hex64_t",
					BITS("%cs_mask", 28, 19) +
					BITS("%0", 26, 22) +
					BITS("%cs_mask", 13, 5) +
					BITS("%0", 12, 0));
			CLOSE_SCOPE();
		}
	}

	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {

		//
		// DRAM Control register
		//

		REG32("%dram_control", 0x78);
		OPEN_SCOPE("dram_control");

		FIELD("RdPtrInit", "hex_t", BITS("../%dram_control", 3, 0));
		// FIXME: this field would benefit from a fixed-point datatype
		FIELD("RdPadRcvFifoDly", ANON_ENUM(KV("memory_clocks_0_5", 0),
						   KV("memory_clocks_1_0", 1),
						   KV("memory_clocks_1_5", 2),
						   KV("memory_clocks_2_0", 3),
						   KV("memory_clocks_2_5", 4),
						   KV("memory_clocks_3_0", 5),
						   KV("memory_clocks_3_5", 6),
						   KV("memory_clocks_4_0", 7)),
				BITS("../%dram_control", 6, 4));
		FIELD("AltVidC3MemClkTriEn", "enabledisable_t",
				BITS("../%dram_control", 16));
		FIELD("DllTempAdjTime", ANON_ENUM(KV("ms_5", 0),
						  KV("ms_1", 1)),
				BITS("../%dram_control", 17));
		FIELD("DqsRcvEnTrain", ANON_ENUM(KV("normal_mode", 0),
						 KV("training_mode", 1)),
				BITS("../%dram_control", 18));

		CLOSE_SCOPE();

		//
		// DRAM Initialization register
		//

		REG32("%dram_init", 0x7C);
		OPEN_SCOPE("dram_init");

		BOOL("send_t", "sending", "not_sending");

		FIELD("MrsAddress", "hex16_t", 
				BITS("../%dram_init", 15, 0));
		FIELD("MrsBank", "hex_t",
				BITS("../%dram_init", 18, 16));
		FIELD("SendPchgAll", "send_t",
				BITS("../%dram_init", 24));
		FIELD("SendAutoRefresh", "send_t",
				BITS("../%dram_init", 25));
		FIELD("SendMrsCmd", "send_t",
				BITS("../%dram_init", 26));
		FIELD("DeassertMemRstX", "yesno_t",
				BITS("../%dram_init", 27));
		FIELD("AssertCke", "yesno_t",
				BITS("../%dram_init", 28));
		FIELD("EnDramInit", "enabledisable_t",
				BITS("../%dram_init", 31));

		CLOSE_SCOPE();
	}

	//
	// DRAM Timing Low/High registers
	//

	INT("busclocks_t", "bus clock(s)");

	REG32("%dram_timing_low", 0x88);
	REG32("%dram_timing_high", 0x8C);
	OPEN_SCOPE("dram_timing");

	// dram_timing_low
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("Tcl", ANON_ENUM(KV("cl_2", 1),
				       KV("cl_3", 2),
				       KV("cl_2.5", 5)),
				BITS("../%dram_timing_low", 2, 0));
		FIELD("Trc", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 7), LAMBDA(_1 - 7)),
				BITS("../%dram_timing_low", 7, 4));
		FIELD("Trfc", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 9), LAMBDA(_1 - 9)),
				BITS("../%dram_timing_low", 11, 8));
		FIELD("Trcd", "busclocks_t",
				BITS("../%dram_timing_low", 14, 12));
		FIELD("Trrd", "busclocks_t",
				BITS("../%dram_timing_low", 18, 16));
		FIELD("Tras", "busclocks_t",
				BITS("../%dram_timing_low", 23, 20));
		FIELD("Trp", "busclocks_t",
				BITS("../%dram_timing_low", 26, 24));
		FIELD("Twr", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
				BITS("../%dram_timing_low", 28));
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("Tcl", ANON_XFORM("int_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				BITS("../%dram_timing_low", 2, 0));
		FIELD("Trcd", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
				BITS("../%dram_timing_low", 5, 4));
		FIELD("Trp", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
				BITS("../%dram_timing_low", 9, 8));
		// FIXME: depends on BurstLength32
		FIELD("Trtp", ANON_BOOL(
			"3_clocks_32_byte_burst_or_5_clocks_64_byte_burst",
			"2_clocks_32_byte_burst_or_4_clocks_64_byte_burst"),
				BITS("../%dram_timing_low", 11));
		FIELD("Tras", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
				BITS("../%dram_timing_low", 15, 12));
		FIELD("Trc", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 11), LAMBDA(_1 - 11)),
				BITS("../%dram_timing_low", 19, 16));
		FIELD("Twr", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
				BITS("../%dram_timing_low", 21, 20));
		FIELD("Trrd", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
				BITS("../%dram_timing_low", 23, 22));
		// FIXME: this could be better if we knew the package type
		FIELD("MemClkDis", ANON_BITMASK(
			KV("am2_ma1_clk1", 0),
			KV("am2_ma0_clk1_s1g1_ma0_clk1", 1),
			KV("f1207_ma3_clk", 2),
			KV("f1207_ma2_clk", 3),
			KV("f1207_ma1_clk_am2_ma1_clk0", 4),
			KV("f1207_ma0_clk_am2_ma0_clk0_s1g1_ma0_clk0", 5),
			KV("am2_ma1_clk2", 6),
			KV("am2_ma0_clk2_s1g1_ma0_clk2", 7)),
				BITS("../%dram_timing_low", 31, 24));
	}

	// dram_timing_high
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("Twtr", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				BITS("../%dram_timing_high", 0));
		FIELD("Trwt", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				BITS("../%dram_timing_high", 6, 4));
		FIELD("Tref", ANON_ENUM(KV("100MHz_15.6us", 0),
					KV("133MHz_15.6us", 1),
					KV("166MHz_15.6us", 2),
					KV("200MHz_15.6us", 3),
					KV("100MHz_7.8us", 8),
					KV("133MHz_7.8us", 9),
					KV("166MHz_7.8us", 10),
					KV("200MHz_7.8us", 11),
					KV("100MHz_3.9us", 16),
					KV("133MHz_3.9us", 17),
					KV("166MHz_3.9us", 18),
					KV("200MHz_3.9us", 19)),
				BITS("../%dram_timing_high", 12, 8));
		FIELD("Twcl", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				BITS("../%dram_timing_high", 22, 20));
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("TrwtTO", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
				BITS("../%dram_timing_high", 6, 4));
		FIELD("Twtr", "busclocks_t",
				BITS("../%dram_timing_high", 9, 8));
		FIELD("Twrrd", "busclocks_t",
				BITS("../%dram_timing_high", 11, 10));
		FIELD("Twrwr", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
				BITS("../%dram_timing_high", 13, 12));
		FIELD("Trdrd", ANON_XFORM("busclocks_t",
					LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
				BITS("../%dram_timing_high", 15, 14));
		FIELD("Tref", ANON_ENUM(KV("undefined", 0),
					KV("refresh_us_7_8", 2),
					KV("refresh_us_3_9", 3)),
				BITS("../%dram_timing_high", 17, 16));
		FIELD("DisAutoRefresh", ANON_BOOL("disabled", "enabled"),
				BITS("../%dram_timing_high", 18));
		for (int i = 0; i < 4; i++) {
			FIELD("Trfc[]", ANON_ENUM(KV("ns_75", 0),
						 KV("ns_105", 1),
						 KV("ns_127", 2),
						 KV("ns_195", 3),
						 KV("ns_327", 4)),
				BITS("../%dram_timing_high", 3*i+22, 3*i+20));
		}
		ENUM("trfc_t", KV("ns_75", 0), KV("ns_105", 1), KV("ns_127", 2),
			       KV("ns_195", 3), KV("ns_327", 4));
		FIELD("Trfc[]", "trfc_t",
			BITS("../%dram_timing_high", 22, 20));
		FIELD("Trfc[]", "trfc_t",
			BITS("../%dram_timing_high", 25, 23));
		FIELD("Trfc[]", "trfc_t",
			BITS("../%dram_timing_high", 28, 26));
		FIELD("Trfc[]", "trfc_t",
			BITS("../%dram_timing_high", 31, 29));
	}

	CLOSE_SCOPE();

	//
	// DRAM Configuration Low/High registers
	//

	REG32("%dram_config_low", 0x90);
	REG32("%dram_config_high", 0x94);
	OPEN_SCOPE("dram_config");

	INT("ns_t", "ns");

	// dram_config_low
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("DLL_Dis", ANON_BOOL("disabled", "enabled"),
				BITS("../%dram_config_low", 0));
		FIELD("D_DRV", ANON_BOOL("weak_drive", "normal_drive"),
				BITS("../%dram_config_low", 1));
		FIELD("QFC_EN", "enabledisable_t",
				BITS("../%dram_config_low", 2));
		FIELD("DisDqsHys", "yesno_t", BITS("../%dram_config_low", 3));
		FIELD("Burst2Opt", "yesno_t", BITS("../%dram_config_low", 5));
		FIELD("Mod64BitMux", "yesno_t", BITS("../%dram_config_low", 6));
		// rev D and earlier
		// FIELD("ScratchBit", "hex_t", BITS("../%dram_config_low", 7));
		FIELD("PwrDwnTriEn", "enabledisable_t",
				BITS("../%dram_config_low", 7));
		FIELD("DramInit", "yesno_t", BITS("../%dram_config_low", 8));
		FIELD("DualDimmEn", "enabledisable_t",
				BITS("../%dram_config_low", 9));
		FIELD("DramEn", "enabledisable_t",
				BITS("../%dram_config_low", 10));
		FIELD("MemClrStatus", ANON_BOOL("complete", "incomplete"),
				BITS("../%dram_config_low", 11));
		FIELD("ESR", "yesno_t", BITS("../%dram_config_low", 12));
		FIELD("SR_S", ANON_BOOL("active", "normal"),
				BITS("../%dram_config_low", 13));
		FIELD("RdWrQByp", ANON_ENUM(KV("2", 0),
					    KV("4", 1),
					    KV("8", 2),
					    KV("16", 3)),
				BITS("../%dram_config_low", 15, 14));
		FIELD("Width128", ANON_BOOL("bits_128", "bits_64"),
				BITS("../%dram_config_low", 16));
		FIELD("DimmEcEn", "enabledisable_t",
				BITS("../%dram_config_low", 17));
		FIELD("UnBuffDimm", ANON_BOOL("unbuffered", "buffered"),
				BITS("../%dram_config_low", 18));
		FIELD("x4DIMMS[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 20));
		FIELD("x4DIMMS[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 21));
		FIELD("x4DIMMS[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 22));
		FIELD("x4DIMMS[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 23));
		FIELD("DisInRcvrs", ANON_BOOL("disabled", "enabled"),
				BITS("../%dram_config_low", 24));
		FIELD("BypMax", "int_t", BITS("../%dram_config_low", 27, 25));
		FIELD("En2T", "yesno_t", BITS("../%dram_config_low", 28));
		FIELD("UpperCSMap", "yesno_t",
				BITS("../%dram_config_low", 29));
		// PowerDownCtl is different in rev. D
		FIELD("PwrDownCtl", ANON_ENUM(KV("power_down_disabled", 0),
					      KV("alternating_DIMM_clock", 1),
					      KV("same_DIMM_clock", 2),
					      KV("independent_DIMM_clock", 3)),
				BITS("../%dram_config_low", 31, 30));
		if (FIELD_EQ("Burst2Opt", pp_value(1)) &&
				FIELD_EQ("UnBuffDimm", pp_value(1)) &&
				FIELD_EQ("En2T", pp_value(0))) {
			FIELD("Bytes32En", ANON_ENUM(KV("8beat_64byte", 0),
						     KV("4beat_64byte", 1),
						     KV("4beat_32byte", 2),
						     KV("2beat_32byte", 3)),
					BITS("../%dram_config_low", 19) +
					BITS("../%dram_config_low", 16));
		} else {
			FIELD("Bytes32En", ANON_ENUM(KV("8beat_64byte", 0),
						     KV("4beat_64byte", 1),
						     KV("4beat_32byte", 2),
			// FIXME: what to do about duplicate enum keys?
						     KV("_4beat_64byte", 3)),
					BITS("../%dram_config_low", 19) +
					BITS("../%dram_config_low", 16));
		}
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("InitDram", "yesno_t", BITS("../%dram_config_low", 0));
		FIELD("ExitSelfRef", "yesno_t", BITS("../%dram_config_low", 1));
		FIELD("DramTerm", ANON_ENUM(KV("disabled", 0),
					    KV("ohms_75", 1),
					    KV("ohms_150", 2),
					    KV("ohms_50", 3)),
				BITS("../%dram_config_low", 5, 4));
		FIELD("DramDrvWeak", ANON_BOOL("weak_mode", "normal_mode"),
				BITS("../%dram_config_low", 7));
		FIELD("ParEn", "enabledisable_t",
				BITS("../%dram_config_low", 8));
		FIELD("SelfRefRateEn", "enabledisable_t",
				BITS("../%dram_config_low", 9));
		FIELD("BurstLength32", ANON_BOOL("bytes_32", "bytes_64"),
				BITS("../%dram_config_low", 10));
		FIELD("Width128", ANON_BOOL("bits_128", "bits_64"),
				BITS("../%dram_config_low", 11));
		FIELD("x4Dimm[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 12));
		FIELD("x4Dimm[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 13));
		FIELD("x4Dimm[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 14));
		FIELD("x4Dimm[]", ANON_BOOL("x4_dimm", "not_x4"),
				BITS("../%dram_config_low", 15));
		FIELD("UnBuffDimm", ANON_BOOL("unbuffered", "buffered"),
				BITS("../%dram_config_low", 16));
		FIELD("DimmEccEn", "enabledisable_t",
				BITS("../%dram_config_low", 19));
	}

	// dram_config_high
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("AsyncLat", "ns_t", BITS("../%dram_config_high", 3, 0));
		// FIXME: this field could benefit from a fixed-point datatype
		FIELD("RdPreamble", ANON_ENUM(KV("ns_2_0", 0),
					      KV("ns_2_5", 1),
					      KV("ns_3_0", 2),
					      KV("ns_3_5", 3),
					      KV("ns_4_0", 4),
					      KV("ns_4_5", 5),
					      KV("ns_5_0", 6),
					      KV("ns_5_5", 7),
					      KV("ns_6_0", 8),
					      KV("ns_6_5", 9),
					      KV("ns_7_0", 10),
					      KV("ns_7_5", 11),
					      KV("ns_8_0", 12),
					      KV("ns_8_5", 13),
					      KV("ns_9_0", 14),
					      KV("ns_9_5", 15)),
				BITS("../%dram_config_high", 11, 8));
		FIELD("MemDQDrvStren", ANON_ENUM(
					KV("reduction_none", 0),
					 KV("reduction_15_percent", 1),
					 KV("reduction_30_percent", 2),
					 KV("reduction_50_percent", 3)),
				BITS("../%dram_config_high", 14, 13));
		FIELD("DisableJitter", "yesno_t",
				BITS("../%dram_config_high", 15));
		FIELD("ILD_lmt", ANON_ENUM(KV("cycles_0", 0),
					   KV("cycles_4", 1),
					   KV("cycles_8", 2),
					   KV("cycles_16", 3),
					   KV("cycles_32", 4),
					   KV("cycles_64", 5),
					   KV("cycles_128", 6),
					   KV("cycles_256", 7)),
				BITS("../%dram_config_high", 18, 16));
		FIELD("DCC_EN", "enabledisable_t",
				BITS("../%dram_config_high", 19));
		FIELD("MemClk", ANON_ENUM(KV("MHz_100", 0),
					  KV("MHz_133", 2),
					  KV("MHz_166", 5),
					  KV("MHz_200", 7)),
				BITS("../%dram_config_high", 22, 20));
		FIELD("MCR", "enabledisable_t",
				BITS("../%dram_config_high", 25));
		FIELD("MC_EN[]", "enabledisable_t",
				BITS("../%dram_config_high", 26));
		FIELD("MC_EN[]", "enabledisable_t",
				BITS("../%dram_config_high", 27));
		FIELD("MC_EN[]", "enabledisable_t",
				BITS("../%dram_config_high", 28));
		FIELD("MC_EN[]", "enabledisable_t",
				BITS("../%dram_config_high", 29));
		FIELD("OddDivisorCorrect", "enabledisable_t",
				BITS("../%dram_config_high", 31));
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("MemClkFreq", ANON_ENUM(KV("MHz_200", 0),
					      KV("MHz_266", 1),
					      KV("MHz_333", 2),
					      KV("MHz_400", 3)),
				BITS("../%dram_config_high", 2, 0));
		FIELD("MemClkFreqVal", "yesno_t", 
				BITS("../%dram_config_high", 3));
		FIELD("MaxAsyncLat", "ns_t",
				BITS("../%dram_config_high", 7, 4));
		FIELD("RDqsEn", ANON_BOOL("read_DQS_pins", "data_mask_pins"),
				BITS("../%dram_config_high", 12));
		FIELD("DisDramInterface", ANON_BOOL("disabled", "enabled"),
				BITS("../%dram_config_high", 14));
		FIELD("PowerDownEn", "enabledisable_t",
				BITS("../%dram_config_high", 15));
		FIELD("PowerDownMode", ANON_BOOL("chip_select_cke_control",
						 "channel_cke_control"),
				BITS("../%dram_config_high", 16));
		FIELD("FourRankSODimm", "truefalse_t",
				BITS("../%dram_config_high", 17));
		FIELD("FourRankRDimm", "truefalse_t",
				BITS("../%dram_config_high", 18));
		FIELD("SlowAccessMode", "enabledisable_t",
				BITS("../%dram_config_high", 20));
		FIELD("BankSwizzleMode", "enabledisable_t",
				BITS("../%dram_config_high", 22));
		FIELD("DcqBypassMax", ANON_INT("bypass(es)"),
				BITS("../%dram_config_high", 27, 24));
		FIELD("FourActWindow", ANON_ENUM(
					KV("no_tFAW_window_restriction", 0),
					KV("MEMCLK_cycles_8", 1),
					KV("MEMCLK_cycles_9", 2),
					KV("MEMCLK_cycles_10", 3), 
					KV("MEMCLK_cycles_11", 4),
					KV("MEMCLK_cycles_12", 5),
					KV("MEMCLK_cycles_13", 6),
					KV("MEMCLK_cycles_14", 7),
					KV("MEMCLK_cycles_15", 8),
					KV("MEMCLK_cycles_16", 9),
					KV("MEMCLK_cycles_17", 10),
					KV("MEMCLK_cycles_18", 11),
					KV("MEMCLK_cycles_19", 12),
					KV("MEMCLK_cycles_20", 13)),
				BITS("../%dram_config_high", 31, 28));
	}

	CLOSE_SCOPE(); // dram_config

	//
	// DRAM Bank Address Mapping register
	//

	REG32("%bank_address_mapping", 0x80);
	OPEN_SCOPE("bank_address_mapping");

	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		ENUM("cs_size_t",
			KV("MB_32_12x8", 0),
			KV("MB_64_12x9", 1),
			KV("MB_128_13x9", 2),
			KV("MB_128_12x10", 3),
			KV("MB_256_13x10", 4),
			KV("MB_512_14x10", 5),
			KV("MB_256_12x11", 6),
			KV("MB_512_13x11", 7),
			KV("MB_1024_14x11", 8),
			KV("MB_1024_13x12", 9),
			KV("MB_2048_14x12", 10));
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		if (FIELD_EQ("../dram_config/Width128", "bits_64")) {
			ENUM("cs_size_t",
				KV("MB_128_13x9x2", 0),
				KV("MB_256_13x10x2", 1),
				KV("MB_512_14x10x2", 2),
				KV("MB_512_13x11x2", 3),
				KV("MB_512_13x10x3", 4),
				KV("MB_1024_14x10x3", 5),
				KV("MB_1024_14x11x2", 6),
				KV("MB_2048_15x10x3", 7),
				KV("MB_2048_14x11x3", 8),
				KV("MB_4096_15x11x3", 9),
				KV("MB_4096_16x10x3", 10),
				KV("MB_8192_16x11x3", 11));
		} else { // Width128 == bits_128
			ENUM("cs_size_t",
				KV("MB_256_13x9x2", 0),
				KV("MB_512_13x10x2", 1),
				KV("MB_1024_14x10x2", 2),
				KV("MB_1024_13x11x2", 3),
				KV("MB_1024_13x10x3", 4),
				KV("MB_2048_14x10x3", 5),
				KV("MB_2048_14x11x2", 6),
				KV("MB_4096_15x10x3", 7),
				KV("MB_4096_14x11x3", 8),
				KV("MB_8192_15x11x3", 9),
				KV("MB_8192_16x10x3", 10),
				KV("MB_16384_16x11x3", 11));
		}
	}

	// these could be "disabled" or hidden if cs[x].CSEnable == false
	FIELD("CS1_0", "cs_size_t", BITS("../%bank_address_mapping", 3, 0));
	FIELD("CS3_2", "cs_size_t", BITS("../%bank_address_mapping", 7, 4));
	FIELD("CS5_4", "cs_size_t", BITS("../%bank_address_mapping", 11, 8));
	FIELD("CS7_6", "cs_size_t", BITS("../%bank_address_mapping", 15, 12));
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("BankSwizzleMode", "enabledisable_t",
				BITS("../%bank_address_mapping", 30));
	}

	CLOSE_SCOPE();


	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		//
		// DRAM Delay Line register
		//

		REG32("%dram_delay_line", 0x98);
		OPEN_SCOPE("dram_delay_line");

		FIELD("Adj", "int_t", BITS("../%dram_delay_line", 23, 16));
		FIELD("AdjSlower", "yesno_t", BITS("../%dram_delay_line", 24));
		FIELD("AdjFaster", "yesno_t", BITS("../%dram_delay_line", 25));
		FIELD("AltVidC3MemClkTriEn", "enabledisable_t",
				BITS("../%dram_delay_line", 27));
		FIELD("CompPwrSaveEn", "enabledisable_t",
				BITS("../%dram_delay_line", 28));
		FIELD("DllSpeed", ANON_ENUM(KV("speed_low", 0),
					    KV("speed_high", 1)),
				BITS("../%dram_delay_line", 29));
		FIELD("DllSpeedOverride", "yesno_t",
				BITS("../%dram_delay_line", 30));

		CLOSE_SCOPE();

		//
		// Scratch Register
		//

		FIELD("ScratchData", "hex32_t", REG32(0x9C));

	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		//
		// DRAM Controller Additional Data registers
		//
		// There are a bunch of registers behind the index/data
		// pair at 0x98/0x9c
		//

		REG32("%dram_additional_data_offset", 0x98);
		REG32("%dram_additional_data_port", 0x9c);

		dram_additional_data("channel_a", 0x0);
		dram_additional_data("channel_b", 0x20);

		//
		// DRAM Controller Miscellaneous register
		//

		REG32("%dram_controller_misc", 0xA0);
		OPEN_SCOPE("dram_misc");

		FIELD("DisableJitter", "yesno_t",
				BITS("../%dram_controller_misc", 1));
		FIELD("RdWrQByp", ANON_ENUM(KV("2", 0), KV("4", 1),
					    KV("8", 2), KV("16", 3)),
				BITS("../%dram_controller_misc", 3, 2));
		FIELD("Mod64BitMux", "enabledisable_t",
				BITS("../%dram_controller_misc", 4));
		FIELD("DCC_EN", "enabledisable_t",
				BITS("../%dram_controller_misc", 5));
		FIELD("ILD_lmt", ANON_ENUM(KV("cycles_0", 0),
					   KV("cycles_4", 1),
					   KV("cycles_8", 2),
					   KV("cycles_16", 3),
					   KV("cycles_32", 4),
					   KV("cycles_64", 5),
					   KV("cycles_128", 6),
					   KV("cycles_256", 7)),
				BITS("../%dram_controller_misc", 8, 6));
		FIELD("DramEnabled", "enabledisable_t",
				BITS("../%dram_controller_misc", 9));
		FIELD("PwrSavingsEn", "enabledisable_t",
				BITS("../%dram_controller_misc", 10));
		// FIXME: this could be better if we knew the package type
		FIELD("MemClkDis", ANON_BITMASK(
			KV("am2_mb1_clk1", 0),
			KV("am2_mb0_clk1_s1g1_mb0_clk1", 1),
			KV("f1207_mb3_clk", 2),
			KV("f1207_mb2_clk", 3),
			KV("f1207_mb1_clk_am2_mb1_clk0", 4),
			KV("f1207_mb0_clk_am2_mb0_clk0_s1g1_mb0_clk0", 5),
			KV("am2_mb1_clk2", 6),
			KV("am2_mb0_clk2_s1g1_mb0_clk2", 7)),
				BITS("../%dram_controller_misc", 31, 24));

		CLOSE_SCOPE(); // dram_misc

	}

	CLOSE_SCOPE(); // dram_ctrl
}

static void
k8_misc_control(const pp_value &seg, const pp_value &bus,
                const pp_value &dev, const pp_value &func)
{
	PCI_SCOPE("misc_ctrl", seg, bus, dev, func+3);

	OPEN_SCOPE("mca_northbridge");

	//
	// MCA Northbridge Control/Configuration registers
	//
	REG32("%control", 0x40);
	REG32("%config", 0x44);
	OPEN_SCOPE("control_config");

	// control
	FIELD("CorrEccEn", "enabledisable_t", BITS("../%control", 0));
	FIELD("UnCorrEccEn", "enabledisable_t", BITS("../%control", 1));
	FIELD("CrcErr0En", "enabledisable_t", BITS("../%control", 2));
	FIELD("CrcErr1En", "enabledisable_t", BITS("../%control", 3));
	FIELD("CrcErr2En", "enabledisable_t", BITS("../%control", 4));
	FIELD("SyncPkt0En", "enabledisable_t", BITS("../%control", 5));
	FIELD("SyncPkt1En", "enabledisable_t", BITS("../%control", 6));
	FIELD("SyncPkt2En", "enabledisable_t", BITS("../%control", 7));
	FIELD("MstrAbrtEn", "enabledisable_t", BITS("../%control", 8));
	FIELD("TgtAbrtEn", "enabledisable_t", BITS("../%control", 9));
	FIELD("GartTblWkEn", "enabledisable_t", BITS("../%control", 10));
	FIELD("AtomicRMWEn", "enabledisable_t", BITS("../%control", 11));
	FIELD("WchDogTmrEn", "enabledisable_t", BITS("../%control", 12));
	if (FIELD_EQ("$k8/k8_rev", "rev_f"))
		FIELD("DramParEn", "enabledisable_t",
				BITS("../%control", 18));

	// config
	FIELD("CpuEccErrEn", "enabledisable_t", BITS("../%config", 0));
	FIELD("CpuRdDatErrEn", "enabledisable_t", BITS("../%config", 1));
	FIELD("SyncOnUcEccEn", "enabledisable_t", BITS("../%config", 2));
	FIELD("SyncPktGenDis", "disableenable_t", BITS("../%config", 3));
	FIELD("SyncPktPropDis", "disableenable_t", BITS("../%config", 4));
	FIELD("IoMstAbortDis", "disableenable_t", BITS("../%config", 5));
	FIELD("CpuErrDis", "disableenable_t", BITS("../%config", 6));
	FIELD("IoErrDis", "disableenable_t", BITS("../%config", 7));
	FIELD("WdogTmrDis", "disableenable_t", BITS("../%config", 8));
	FIELD("WdogTmrCntSel", ANON_ENUM(KV("4095", 0),
				         KV("2047", 1),
					 KV("1023", 2),
					 KV("511", 3),
					 KV("255", 4),
					 KV("127", 5),
					 KV("63", 6),
					 KV("31", 7)),
			BITS("../%config", 11, 9));
	FIELD("WdogTmrBaseSel", ANON_ENUM(KV("ms_1", 0),
					  KV("us_1", 1),
					  KV("ns_5", 2)),
			BITS("../%config", 13, 12));
	FIELD("LdtLinkSel", ANON_ENUM(KV("ht_link_0", 0),
				      KV("ht_link_1", 1),
				      KV("ht_link_2", 2)),
			BITS("../%config", 15, 14));
	FIELD("GenCrcErrByte0", ANON_BOOL("generating", "not_generating"),
			BITS("../%config", 16));
	FIELD("GenCrcErrByte1", ANON_BOOL("generating", "not_generating"),
			BITS("../%config", 17));
	FIELD("SyncOnWdogEn", "enabledisable_t", BITS("../%config", 20));
	FIELD("SyncOnAnyErrEn", "enabledisable_t", BITS("../%config", 21));
	FIELD("EccEn", "enabledisable_t", BITS("../%config", 22));
	FIELD("ChipKillEccEn", "enabledisable_t", BITS("../%config", 23));
	FIELD("IoRdDatErrEn", "enabledisable_t", BITS("../%config", 24));
	FIELD("DisPciCfgCpuErrRsp", "yesno_t", BITS("../%config", 25));
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("NbMcaToMstExcEn", "enabledisable_t",
				BITS("../%config", 27));
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("NbMcaToMstCpuEn", "enabledisable_t",
				BITS("../%config", 27));
	}
	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("DisTgtAbtCpuErrRsp", "yesno_t",
				BITS("../%config", 28));
		FIELD("DisMstAbtCpuErrRsp", "yesno_t",
				BITS("../%config", 29));
		FIELD("SyncOnDramAdrParErrEn", "enabledisable_t",
				BITS("../%config", 30));
	}

	CLOSE_SCOPE(); // control_config

	//
	// MCA Northbridge Status registers
	//
	REG32("%status_low", 0x48);
	REG32("%status_high", 0x4C);
	OPEN_SCOPE("status");

	// Error code types
	ENUM("cachelevel_t",
			KV("level_0", 0),
			KV("level_1", 1),
			KV("level_2", 2),
			KV("generic", 3));
	ENUM("memorio_t",
			KV("memory_access", 0),
			KV("io_access", 2),
			KV("generic", 3));
	ENUM("mtt_t",
			KV("generic_error", 0),
			KV("generic_read", 1),
			KV("generic_write", 2),
			KV("data_read", 3),
			KV("data_write", 4),
			KV("instruction_fetch", 5),
			KV("prefetch", 6),
			KV("evict", 7),
			KV("snoop", 8));
	ENUM("participationproc_t",
			KV("local_node_originated", 0),
			KV("local_node_responded", 1),
			KV("local_node_observed", 2),
			KV("generic", 3));
	ENUM("transtype_t",
			KV("instruction", 0),
			KV("data", 1),
			KV("generic", 2));

	FIELD("ErrorCode", "hex16_t", BITS("../%status_low", 15, 0));
	if (FIELD_TEST("ErrorCode", 0x0800)) {
		// Bus error
		FIELD("CacheLevel", "cachelevel_t",
				BITS("../%status_low", 1, 0));
		FIELD("MemoryOrIO", "memorio_t",
				BITS("../%status_low", 3, 2));
		FIELD("MemoryTransactionType", "mtt_t",
				BITS("../%status_low", 7, 4));
		FIELD("Timeout", "yesno_t",
				BITS("../%status_low", 8));
		FIELD("ParticipationProcessor", "participationproc_t",
				BITS("../%status_low", 10, 9));
		FIELD("ErrorCodeExt", ANON_ENUM(KV("ecc_error", 0),
						KV("crc_error", 1),
						KV("sync_error", 2),
						KV("mst_abort", 3),
						KV("tgt_abort", 4),
						KV("rmw_error", 6),
						KV("wdog_error", 7),
						KV("chipkill_ecc_error", 8),
						KV("dev_error", 9),
						KV("dram_parity_error", 13)),
				BITS("../%status_low", 19, 16));
		if (FIELD_EQ("ErrorCodeExt", "chipkill_ecc_error"))
			FIELD("Syndrome", "hex8_t",
					BITS("../%status_low", 31, 24));
	} else if (FIELD_TEST("ErrorCode", 0x0100)) {
		// Memory error
		FIELD("CacheLevel", "cachelevel_t",
				BITS("../%status_low", 1, 0));
		FIELD("TransactionType", "transtype_t",
				BITS("../%status_low", 3, 2));
		FIELD("MemoryTransactionType", "mtt_t",
				BITS("../%status_low", 7, 4));
	} else if (FIELD_TEST("ErrorCode", 0x0010)) {
		// TLB error
		FIELD("CacheLevel", "cachelevel_t",
				BITS("../%status_low", 1, 0));
		FIELD("TransactionType", "transtype_t",
				BITS("../%status_low", 3, 2));
		FIELD("ErrorCodeExt", ANON_ENUM(KV("gart_error", 5)),
				BITS("../%status_low", 19, 16));
	}

	FIELD("ErrCPU[]", "yesno_t", BITS("../%status_high", 0));
	FIELD("ErrCPU[]", "yesno_t", BITS("../%status_high", 1));
	FIELD("LDTLink[]", ANON_BOOL("associated", "not_associated"),
			BITS("../%status_high", 4));
	FIELD("LDTLink[]", ANON_BOOL("associated", "not_associated"),
			BITS("../%status_high", 5));
	FIELD("LDTLink[]", ANON_BOOL("associated", "not_associated"),
			BITS("../%status_high", 6));
	FIELD("ErrScrub", "yesno_t", BITS("../%status_high", 8));
	if (FIELD_EQ("$k8/k8_rev", "rev_f"))
		FIELD("DramChannel", ANON_ENUM(KV("A", 0), KV("B", 1)),
				BITS("../%status_high", 9));
	FIELD("UnCorrECC", "yesno_t", BITS("../%status_high", 13));
	FIELD("CorrECC", "yesno_t", BITS("../%status_high", 14));
	FIELD("ECC_Synd", "hex8_t", BITS("../%status_high", 22, 15));
	FIELD("PCC", "yesno_t", BITS("../%status_high", 25));
	FIELD("ErrAddrVal", ANON_BOOL("valid", "invalid"),
			BITS("../%status_high", 26));
	FIELD("ErrMiscVal", "yesno_t", BITS("../%status_high", 27));
	FIELD("ErrEn", "enabledisable_t", BITS("../%status_high", 28));
	FIELD("ErrUnCorr", "yesno_t", BITS("../%status_high", 29));
	FIELD("ErrOver", "yesno_t", BITS("../%status_high", 30));
	FIELD("ErrValid", "yesno_t", BITS("../%status_high", 31));

	CLOSE_SCOPE();

	//
	// MCA Northbridge Address registers
	//
	REG32("%address_low", 0x50);
	REG32("%address_high", 0x54);

	if (DEFINED("ErrorCodeExt") && FIELD_EQ("ErrorCodeExt", "wdog_error")) {
		OPEN_SCOPE("wdog_error");

		FIELD("LdtCmd", "hex_t", BITS("../%address_low", 8, 3));
		FIELD("OpType", ANON_ENUM(KV("normal_op", 0),
					  KV("bus_lock", 1),
					  KV("local_apic_access", 2),
					  KV("interrupt_request", 3),
					  KV("system_mgmt_request", 4),
					  KV("interrupt_broadcast", 5),
					  KV("stop_grant", 6),
					  KV("smi_ack", 7)),
				BITS("../%address_low", 11, 9));
		FIELD("NextAction", 
			ANON_ENUM(KV("complete", 0),
				  KV("send_request", 2),
				  KV("send_second_request", 3),
				  KV("send_final_response_back", 4),
				  KV("send_initial_response_back", 5),
				  KV("send_final_response_to_mem", 6),
				  KV("send_initial_response_to_mem", 7)),
			BITS("../%address_low", 14, 12));
		FIELD("SrcPtr", ANON_ENUM(KV("host_bridge_local_node", 0),
					  KV("cpu_local_node", 1),
					  KV("mem_controller_local_node", 2),
					  KV("ht_link_0", 5),
					  KV("ht_link_1", 6),
					  KV("ht_link_2", 7)),
				BITS("../%address_low", 17, 15));
		FIELD("SrcUnit", "int_t", BITS("../%address_low", 19, 18));
		FIELD("SrcNode", "int_t", BITS("../%address_low", 22, 20));
		FIELD("DstUnit", "int_t", BITS("../%address_low", 24, 23));
		FIELD("DstNode", "int_t", BITS("../%address_low", 27, 25));
		FIELD("WaitPW", "yesno_t", BITS("../%address_low", 29));
		if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
			FIELD("WaitCode", "bitfield_t",
					BITS("../%address_high", 2, 0) +
					BITS("../%address_low", 31, 30));
		} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
			FIELD("WaitCode", ANON_ENUM(
				KV("no_wait_cond", 0),
				KV("xbar_wait_for_probe_response_"
				   "from_cpu_0", 4),
				KV("xbar_wait_for_probe_response_"
				   "from_cpu_1", 5),
				KV("xbar_wait_because_of_gart_or_dev_miss", 8),
				KV("xbar_wait_for_gart_table_walk_"
				   "state_machine_to_free", 9),
				KV("xbar_or_cpu_wait_for_bus_to_unlock", 10),
				KV("sri_wait_for_avail_xbar_"
				   "upstream_req_buf", 16),
				KV("sri_wait_for_avail_xbar_"
				   "upstream_req_buf", 16 + 8),
				KV("sri_wait_for_avail_xbar_"
				   "upstream_posted_req_buf", 17),
				KV("sri_wait_for_avail_xbar_"
				   "upstream_posted_req_buf", 17 + 8),
				KV("sri_wait_for_avail_xbar_"
				   "downstream_req_buf", 18),
				KV("sri_wait_for_avail_xbar_"
				   "downstream_req_buf", 18 + 8),
				KV("sri_wait_for_avail_xbar_"
				   "downstream_posted_req_buf", 19),
				KV("sri_wait_for_avail_xbar_"
				   "downstream_posted_req_buf", 19 + 8),
				KV("sri_wait_for_avail_xbar_"
				   "upstream_resp_buf", 20),
				KV("sri_wait_for_avail_xbar_"
				   "upstream_resp_buf", 20 + 8),
				KV("sri_wait_for_avail_xbar_"
				   "downstream_resp_buf", 21),
				KV("sri_wait_for_avail_xbar_"
				   "downstream_resp_buf", 21 + 8),
				KV("sri_wait_for_avail_xbar_"
				   "display_refresh_buf", 22),
				KV("sri_wait_for_avail_xbar_"
				   "display_refresh_buf", 22+8)),
					BITS("../%address_high", 2, 0) +
					BITS("../%address_low", 31, 30));
		}
		FIELD("RspCnt", ANON_INT("responses pending"),
				BITS("../%address_high", 6, 3));
		FIELD("GartTblWkInProg", "yesno_t",
				BITS("../%address_high", 7));

		CLOSE_SCOPE();
	} else {
		FIELD("ErrAddr", "addr64_t", 
				BITS("%address_high", 7, 0) +
				BITS("%address_low", 31, 3) +
				BITS("%0", 2, 0));
	}

	CLOSE_SCOPE(); // mca_northbridge

	//
	// Scrub Control register
	//
	OPEN_SCOPE("scrubbers");

	REG32("%scrub_control", 0x58);
	ENUM("scrub_rate_t", KV("off", 0),
			     KV("ns_40", 1),
			     KV("ns_80", 2),
			     KV("ns_160", 3),
			     KV("ns_320", 4),
			     KV("ns_640", 5),
			     KV("us_1_28", 6),
			     KV("us_2_56", 7),
			     KV("us_5_12", 8),
			     KV("us_10_2", 9),
			     KV("us_20_5", 10),
			     KV("us_41_0", 11),
			     KV("us_81_9", 12),
			     KV("us_163_8", 13),
			     KV("us_327_7", 14),
			     KV("us_655_4", 15),
			     KV("ms_1_31", 16),
			     KV("ms_2_62", 17),
			     KV("ms_5_24", 18),
			     KV("ms_10_49", 19),
			     KV("ms_20_97", 20),
			     KV("ms_42_00", 21),
			     KV("ms_84_00", 22));

	FIELD("DramScrub", "scrub_rate_t", BITS("%scrub_control", 4, 0));
	FIELD("L2Scrub", "scrub_rate_t", BITS("%scrub_control", 12, 8));
	FIELD("DcacheScrub", "scrub_rate_t", BITS("%scrub_control", 20, 16));

	//
	// DRAM Scrub Address registers
	//
	REG32("%dram_scrub_address_low", 0x5C);
	REG32("%dram_scrub_address_high", 0x60);

	FIELD("ScrubReDirEn", "yesno_t", BITS("%dram_scrub_address_low", 0));
	FIELD("ScrubAddr", "addr64_t",
			BITS("%dram_scrub_address_high", 7, 0) +
			BITS("%dram_scrub_address_low", 31, 6) +
			BITS("%0", 5, 0));

	CLOSE_SCOPE(); // scrubbers

	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		//
		// HTC register
		//
		REG32("%htc", 0x64);
		OPEN_SCOPE("htc");

		FIELD("HtcEn", "enabledisable_t", BITS("../%htc", 0));
		FIELD("HtcSbcEn", "enabledisable_t", BITS("../%htc", 1));
		FIELD("HtcAct", "yesno_t", BITS("../%htc", 4));
		FIELD("HtcActSts", "yesno_t", BITS("../%htc", 5));

		CLOSE_SCOPE();

		//
		// Thermal Control register
		//
		REG32("%ThermalControl", 0x68);
		OPEN_SCOPE("thermal_control");

		FIELD("StcSbcTmpHiEn", "enabledisable_t",
				BITS("../%ThermalControl", 0));
		FIELD("StcSbcTmpLoEn", "enabledisable_t", 
				BITS("../%ThermalControl", 1));
		FIELD("StcApcTmpHiEn", "enabledisable_t",
				BITS("../%ThermalControl", 2));
		FIELD("StcApcTmpLoEn", "enabledisable_t",
				BITS("../%ThermalControl", 3));
		FIELD("StcTmpHiSts", "yesno_t",
				BITS("../%ThermalControl", 6));
		FIELD("StcTmpLoSts", "yesno_t",
				BITS("../%ThermalControl", 7));
		FIELD("StcTmpLmt", ANON_XFORM(ANON_INT("C"),
					LAMBDA(52 + (_1 * 2)),
					LAMBDA((_1 - 52) / 2)),
				BITS("../%ThermalControl", 20, 16));
		FIELD("StcHystLmt", ANON_XFORM(ANON_INT("C"),
					LAMBDA(2 + (_1 * 2)),
					LAMBDA((_1 - 2) / 2)),
				BITS("../%ThermalControl", 27, 24));

		CLOSE_SCOPE();
	}

	OPEN_SCOPE("buffer_counts");

	//
	// SRI-to-XBAR Buffer Counts register
	//
	REG32("%sri_to_xbar", 0x70);
	OPEN_SCOPE("sri_to_xbar");

	FIELD("UReq", "int_t", BITS("../%sri_to_xbar", 1, 0));
	FIELD("UPReq", "int_t", BITS("../%sri_to_xbar", 5, 4));
	FIELD("URsp", "int_t", BITS("../%sri_to_xbar", 9, 8));
	FIELD("ReqD", "int_t", BITS("../%sri_to_xbar", 17, 16));
	FIELD("DispRefReq", "int_t", BITS("../%sri_to_xbar", 21, 20));
	FIELD("URspD", "int_t", BITS("../%sri_to_xbar", 25, 24));
	FIELD("DReq", "int_t", BITS("../%sri_to_xbar", 29, 28));
	FIELD("DPReq", "int_t", BITS("../%sri_to_xbar", 31, 30));

	CLOSE_SCOPE();

	//
	// XBAR-to-SRI Buffer Counts register
	//
	REG32("%xbar_to_sri", 0x74);
	OPEN_SCOPE("xbar_to_sri");

	FIELD("UReq", "int_t", BITS("../%xbar_to_sri", 2, 0));
	FIELD("UPReq", "int_t", BITS("../%xbar_to_sri", 6, 4));
	FIELD("Prb", "int_t", BITS("../%xbar_to_sri", 15, 12));
	FIELD("DispRefReq", "int_t", BITS("../%xbar_to_sri", 22, 20));
	FIELD("DReq", "int_t", BITS("../%xbar_to_sri", 29, 28));
	FIELD("DPReq", "int_t", BITS("../%xbar_to_sri", 31, 30));

	CLOSE_SCOPE();

	//
	// MCT-to-XBAR Buffer Counts register
	//
	REG32("%mct_to_xbar", 0x78);
	OPEN_SCOPE("mct_to_xbar");

	FIELD("Rsp", "int_t", BITS("../%mct_to_xbar", 11, 8));
	FIELD("Prb", "int_t", BITS("../%mct_to_xbar", 14, 12));
	FIELD("RspD", "int_t", BITS("../%mct_to_xbar", 27, 24));

	CLOSE_SCOPE();

	//
	// Free List Buffer Counts register
	//
	REG32("%free_list", 0x7C);
	OPEN_SCOPE("free_list");

	FIELD("FreeCmd", "int_t", BITS("../%free_list", 3, 0));
	FIELD("FReq", "int_t", BITS("../%free_list", 5, 4));
	FIELD("FRsp", "int_t", BITS("../%free_list", 10, 8));
	FIELD("FRspD", "int_t", BITS("../%free_list", 13, 12));

	CLOSE_SCOPE();

	CLOSE_SCOPE(); // buffer_counts

	//
	// Power Management Control registers
	//
	REG32("%power_mgmt_control_low", 0x80);
	REG32("%power_mgmt_control_high", 0x84);

	ENUM("clksel_t", KV("divide_by_8", 0),
			 KV("divide_by_16", 1),
			 KV("divide_by_32", 2),
			 KV("divide_by_64", 3),
			 KV("divide_by_128", 4),
			 KV("divide_by_256", 5),
			 KV("divide_by_512", 6));

	// Low
	for (int i = 0; i < 32; i += 8) {
		OPEN_SCOPE("PMM[]");

		FIELD("CPULowPrEn", "enabledisable_t",
				BITS("../%power_mgmt_control_low", 0 + i));
		FIELD("NBLowPwrEn", "enabledisable_t",
				BITS("../%power_mgmt_control_low", 1 + i));
		FIELD("FidVidEn", "enabledisable_t",
				BITS("../%power_mgmt_control_low", 2 + i));
		FIELD("AltVidEn", "enabledisable_t",
				BITS("../%power_mgmt_control_low", 3 + i));
		FIELD("ClkSel", "clksel_t",
				BITS("../%power_mgmt_control_low",
				     6 + i, 4 + i));

		CLOSE_SCOPE();
	}

	// High
	for (int i = 0; i < 32; i += 8) {
		OPEN_SCOPE("PMM[]");

		FIELD("CPULowPrEn", "enabledisable_t",
				BITS("../%power_mgmt_control_high", 0 + i));
		FIELD("NBLowPwrEn", "enabledisable_t",
				BITS("../%power_mgmt_control_high", 1 + i));
		FIELD("FidVidEn", "enabledisable_t",
				BITS("../%power_mgmt_control_high", 2 + i));
		FIELD("AltVidEn", "enabledisable_t",
				BITS("../%power_mgmt_control_high", 3 + i));
		FIELD("ClkSel", "clksel_t",
				BITS("../%power_mgmt_control_high",
					6 + i, 4 + i));

		CLOSE_SCOPE();
	}

	OPEN_SCOPE("gart");

	//
	// GART Aperture Control register
	//
	REG32("%aperture_control", 0x90);
	OPEN_SCOPE("aperture_control");

	FIELD("GartEn", "enabledisable_t", BITS("../%aperture_control", 0));
	FIELD("GartSize", ANON_ENUM(KV("MB_32", 0),
				    KV("MB_64", 1),
				    KV("MB_128", 2),
				    KV("MB_256", 3),
				    KV("MB_512", 4),
				    KV("MB_1024", 5),
				    KV("MB_2048", 6)),
			BITS("../%aperture_control", 3, 1));
	FIELD("DisGartCpu", "yesno_t", BITS("../%aperture_control", 4));
	FIELD("DisGartIo", "yesno_t", BITS("../%aperture_control", 5));
	FIELD("DisGartTblWlkPrb", "yesno_t", BITS("../%aperture_control", 6));

	CLOSE_SCOPE();

	//
	// GART Aperture Base register
	//
	REG32("%aperture_base", 0x94);
	FIELD("GartAperBaseAddr", "addr64_t",
			BITS("%aperture_base", 14, 0) +
			BITS("%0", 24, 0));

	//
	// GART Table Base register
	//
	REG32("%table_base", 0x98);
	FIELD("GartTblBaseAddr", "addr64_t",
			BITS("%table_base", 31, 4) +
			BITS("%0", 11, 0));

	//
	// GART Cache Control register
	//
	REG32("%cache_control", 0x9C);
	OPEN_SCOPE("cache_control");

	FIELD("InvGart", ANON_BOOL("invalidating", "not_invalidating"),
			BITS("../%cache_control", 0));
	FIELD("GartPteErr", "yesno_t", BITS("../%cache_control", 1));

	CLOSE_SCOPE();

	CLOSE_SCOPE(); // gart

	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		//
		// Power Control Miscellaneous register
		//
		REG32("%power_control_misc", 0xA0);
		OPEN_SCOPE("power_control_misc");

		FIELD("PsiVid", "hex_t", BITS("../%power_control_misc", 5, 0));
		FIELD("PsiVidEn", "enabledisable_t",
				BITS("../%power_control_misc", 7));

		CLOSE_SCOPE();

		//
		// On-Line Spare Control register
		//
		REG32("%on_line_spare_control", 0xB0);
		OPEN_SCOPE("on_line_spare_control");

		FIELD("SwapEn", "yesno_t",
				BITS("../%on_line_spare_control", 0));
		FIELD("SwapDone", "yesno_t",
				BITS("../%on_line_spare_control", 1));
		FIELD("BadDramCs", "int_t",
				BITS("../%on_line_spare_control", 6, 4));
		FIELD("SwapDoneInt", ANON_ENUM(KV("none", 0),
					       KV("smi", 2)),
				BITS("../%on_line_spare_control", 13, 12));
		FIELD("EccErrInt", ANON_ENUM(KV("none", 0),
					     KV("smi", 2)),
				BITS("../%on_line_spare_control", 15, 14));
		FIELD("EccErrCntDramCs", "int_t",
				BITS("../%on_line_spare_control", 18, 16));
		FIELD("EccErrCntDramChan", "int_t", 
				BITS("../%on_line_spare_control", 20));
		FIELD("EccErrCntWrEn", "enabledisable_t",
				BITS("../%on_line_spare_control", 23));
		FIELD("EccErrCnt", "int_t",
				BITS("../%on_line_spare_control", 27, 24));

		CLOSE_SCOPE();
	}

	//
	// Clock Power/Timing registers
	//
	REG32("%clock_power_timing_low", 0xD4);
	REG32("%clock_power_timing_high", 0xD8);
	OPEN_SCOPE("clock_power_timing");

	FIELD("GPE", ANON_ENUM(KV("sys_clocks_200", 1),
			       KV("sys_clocks_400", 2),
			       KV("sys_clocks_600", 3),
			       KV("sys_clocks_800", 4),
			       KV("sys_clocks_1600", 5),
			       KV("sys_clocks_3000", 6),
			       KV("sys_clocks_20000", 7)),
			BITS("../%clock_power_timing_low", 2, 0));
	FIELD("ClkRampHyst", ANON_ENUM(KV("ns_0", 0),
				       KV("ns_125", 1),
				       KV("ns_250", 2),
				       KV("ns_375", 3),
				       KV("ns_500", 4),
				       KV("ns_750", 5),
				       KV("ns_1000", 6),
				       KV("ns_2000", 7),
				       KV("us_4", 8),
				       KV("us_8", 9),
				       KV("us_16", 10),
				       KV("us_32", 11)),
			BITS("../%clock_power_timing_low", 11, 8));
	FIELD("ReConDel", ANON_INT("us"),
			BITS("../%clock_power_timing_low", 15, 12));
	FIELD("LClkPLLLock", ANON_INT("system bus clocks to wait"),
			BITS("../%clock_power_timing_low", 31, 16) +
			BITS("%0", 3, 0));
	FIELD("VSTime", ANON_INT("system clock cycles"),
			BITS("../%clock_power_timing_high", 19, 0));
	if (FIELD_EQ("$k8/k8_rev", "rev_e"))
		FIELD("AltVid", "hex_t",
				BITS("../%clock_power_timing_high", 24, 20));
	else if (FIELD_EQ("$k8/k8_rev", "rev_f"))
		FIELD("AltVid", "hex_t",
				BITS("../%clock_power_timing_high", 25, 20));
	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("AltVidTriEn", "enabledisable_t",
				BITS("../%clock_power_timing_high", 26));
		FIELD("GfxMode", ANON_BOOL("uma", "discrete"),
				BITS("../%clock_power_timing_high", 27));
	}
	FIELD("RampVIDOff", ANON_ENUM(KV("mV_0", 0),
				      KV("mV_25", 1),
				      KV("mV_50", 2),
				      KV("mV_75", 3),
				      KV("mV_100", 4),
				      KV("mV_125", 5),
				      KV("mV_150", 6),
				      KV("mV_175", 7)),
			BITS("../%clock_power_timing_high", 30, 28));

	CLOSE_SCOPE();

	//
	// HyperTransport Read Pointer Optimization register
	//
	REG32("%ht_read_ptr_optimization", 0xDC);
	OPEN_SCOPE("ht_read_pointer_optimiation");
	INT("ht_clocks", "HT clock(s)");

	FIELD("RcvRdPtrLdt[]", "ht_clocks",
			BITS("../%ht_read_ptr_optimization", 2, 0));
	FIELD("XmtRdPtrLdt[]", "ht_clocks",
			BITS("../%ht_read_ptr_optimization", 5, 4));
	FIELD("RcvRdPtrLdt[]", "ht_clocks",
			BITS("../%ht_read_ptr_optimization", 10, 8));
	FIELD("XmtRdPtrLdt[]", "ht_clocks",
			BITS("../%ht_read_ptr_optimization", 13, 12));
	FIELD("RcvRdPtrLdt[]", "ht_clocks",
			BITS("../%ht_read_ptr_optimization", 18, 16));
	FIELD("XmtRdPtrLdt[]", "ht_clocks",
			BITS("../%ht_read_ptr_optimization", 21, 20));

	CLOSE_SCOPE();

	//
	// Thermtrip Status register
	//
	REG32("%thermtrip_status", 0xE4);
	OPEN_SCOPE("thermtrip_status");

	FIELD("Thermtp", "yesno_t", BITS("../%thermtrip_status", 1));
	if (FIELD_EQ("$k8/k8_rev", "rev_f"))
		FIELD("ThermSenseCoreSel", ANON_BOOL("cpu0", "cpu1"),
				BITS("../%thermtrip_status", 2));
	FIELD("ThermtpSense0", "yesno_t", BITS("../%thermtrip_status", 3));
	FIELD("ThermtpSense1", "yesno_t", BITS("../%thermtrip_status", 4));
	FIELD("ThermtpEn", "enabledisable_t", BITS("../%thermtrip_status", 5));
	if (FIELD_EQ("$k8/k8_rev", "rev_f"))
		FIELD("ThermSenseSel", "int_t",
				BITS("../%thermtrip_status", 6));
	if (FIELD_EQ("$k8/k8_rev", "rev_e")) {
		FIELD("DiodeOffsetSignBit", ANON_BOOL("negative", "positive"),
				BITS("../%thermtrip_status", 24));
		if (FIELD_EQ("DiodeOffsetSignBit", "negative")) {
			FIELD("DiodeOffset", ANON_XFORM(ANON_INT("C"),
						LAMBDA(-1 * _1),
						LAMBDA(-1 * _1)),
					BITS("../%thermtrip_status", 13, 8));
		} else { // positive
			FIELD("DiodeOffset", ANON_INT("C"),
					BITS("../%thermtrip_status", 13, 8));
		}
		FIELD("TCaseMax", ANON_XFORM(ANON_INT("degrees C"),
					LAMBDA(_1 + 51), LAMBDA(_1 - 51)),
				BITS("../%thermtrip_status", 28, 25));
	} else if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		FIELD("DiodeOffset", ANON_XFORM(ANON_INT("degrees C"),
						LAMBDA(11 - _1),
						LAMBDA((-1 * _1) + 11)),
				BITS("../%thermtrip_status", 13, 8));
		FIELD("CurTmp", ANON_XFORM(ANON_INT("degrees C"),
					    LAMBDA(_1 - 49),
					    LAMBDA(_1 + 49)),
				BITS("../%thermtrip_status", 23, 14));
		FIELD("TjOffset", "int_t",
				BITS("../%thermtrip_status", 28, 24));
	}
	FIELD("SwThermtp", "onoff_t", BITS("../%thermtrip_status", 31));

	CLOSE_SCOPE();

	//
	// Northbridge Capabilities register
	//
	REG32("%northbridge_capabilities", 0xE8);
	OPEN_SCOPE("northbridge_capabilities");

	// FIXME: what to do about field names starting with a number?
	FIELD("_128BitCap", "yesno_t", BITS("../%northbridge_capabilities", 0));
	FIELD("MPCap", "yesno_t", BITS("../%northbridge_capabilities", 1));
	FIELD("BigMPCap", "yesno_t", BITS("../%northbridge_capabilities", 2));
	FIELD("EccCap", "yesno_t", BITS("../%northbridge_capabilities", 3));
	FIELD("ChipKillEccCap", "yesno_t",
			BITS("../%northbridge_capabilities", 4));
	FIELD("DramFreq", ANON_ENUM(KV("no_limit", 0),
				    KV("MHz_166", 1),
				    KV("MHz_133", 2),
				    KV("MHz_100", 3)),
			BITS("../%northbridge_capabilities", 6, 5));
	FIELD("MemCntCap", "yesno_t", BITS("../%northbridge_capabilities", 8));
	if (FIELD_EQ("$k8/k8_rev", "rev_f"))
		FIELD("HtcCap", "yesno_t",
				BITS("../%northbridge_capabilities", 10));
	FIELD("CmpCap", ANON_ENUM(KV("single_core", 0),
				  KV("dual_Core", 1)),
			BITS("../%northbridge_capabilities", 13, 12));

	CLOSE_SCOPE();

	if (FIELD_EQ("$k8/k8_rev", "rev_f")) {
		//
		// CPUID Family Model register
		//

		// Don't bother separating out the fields of this register
		// because we already handle it in CPUID.
		FIELD("cpuid_family_model", "hex32_t", REG32(0xFC));
	}

	CLOSE_SCOPE(); //misc_ctrl
}

static void
k8_discovered(const std::vector<pp_value> &args)
{
	pp_value seg = args[0];
	pp_value bus = args[1];
	pp_value dev = args[2];
	pp_value func = args[3];

	pp_value node = dev-24;
	pp_value ncores;

	OPEN_SCOPE("k8[]");
	BOOKMARK("k8");

	// some k8-global types
	// FIXME: this could benefit from a fixed-point type
	HEX("fid_t", 8);
	HEX("vid_t", 8);

	// Figure out how many cores on on each node, and explore CPUID
	// for each core.
	//
	// FIXME: This is kind of fugly.  It might be better to move CPUID
	// out of the k8 device itself?  We need to know
	// family/model/stepping for PCI stuff, though.
	if (node == 0) {
		k8_cpu(node, 1, 0);
		ncores = READ("core[0]/cpuid/logical_proc_count");
	} else {
		//FIXME: this assumes all k8s are the same
		//FIXME: uggh.  Maybe allow ALIAS in the node0 case?
		ncores = READ("/k8[0]/core[0]/cpuid/logical_proc_count");
		k8_cpu(node, ncores, 0);
	}

	for (unsigned i = 1; i < ncores; i++) {
		k8_cpu(node, ncores, i);
	}

	// for simplicity
	ENUM("k8_rev_t",
		KV("rev_e", 'e'),
		KV("rev_f", 'f'),
		KV("unknown", 0));
	// FIXME: when aliases are added, change to alias to "core[0]/k8_rev"
	if (FIELD_EQ("core[0]/cpuid/family", 0xf)) {
		if (FIELD_GE("core[0]/cpuid/model", 0x40)) {
			FIELD("k8_rev", "k8_rev_t", 'f');
		} else if (FIELD_GE("core[0]/cpuid/model", 0x20)) {
			FIELD("k8_rev", "k8_rev_t", 'e');
		} else {
			FIELD("k8_rev", "k8_rev_t", pp_value(0));
		}
	} else {
		FIELD("k8_rev", "k8_rev_t", pp_value(0));
	}

	// function 0
	k8_ht_config(seg, bus, dev, func);

	// function 1
	k8_address_map(seg, bus, dev, func);

	// function 2
	k8_dram_controller(seg, bus, dev, func);

	// function 3
	k8_misc_control(seg, bus, dev, func);

	CLOSE_SCOPE(); // k8[]
}

class k8_discovery {
    public:
	explicit
	k8_discovery()
	{
		// register our 'keystone' device
		pp_register_discovery("pci", ARGS(0x1022, 0x1100),
				k8_discovered);
		// ignore other sub-devices in this same device, we'll
		// handle them in k8_discovered()
		pp_register_discovery("pci", ARGS(0x1022, 0x1101));
		pp_register_discovery("pci", ARGS(0x1022, 0x1102));
		pp_register_discovery("pci", ARGS(0x1022, 0x1103));
		pp_register_discovery("cpuid",     // rev E
				ARGS(pp_value("0x444d416369746e6568747541"),
				     0x0f, 0x0f,   // family range
				     0x20, 0x39,   // model range
				     0x00, 0xFF)); // stepping range
		pp_register_discovery("cpuid",     // rev F
				ARGS(pp_value("0x444d416369746e6568747541"),
				     0x0f, 0x0f,   // family range
				     0x40, 0xFF,   // model range
				     0x00, 0xFF)); // stepping range
		//FIXME: register other devices this owns
	}
};

static k8_discovery the_k8_discovery;
