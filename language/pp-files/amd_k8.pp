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

import pp;

// FIXME: this could benefit from a fixed-point type and a table
type fid_t = HEX(8);
type vid_t = HEX(8);

k8_cpuid(int cpu)
{
	CPUID_SCOPE("cpuid", cpu, {
		//FIXME: more
	}); // cpuid
}

k8_msr(int cpu)
{
	MSR_SCOPE("msr", cpu, {
		SCOPE("EFER", {
			REG64("%EFER", 0xc0000080);
			FIELD("SYSCALL", enabledisable_t, BITS("%EFER", 0));
			FIELD("LME", enabledisable_t, BITS("%EFER", 8));
			FIELD("LMA", yesno_t, BITS("%EFER", 10));
			FIELD("NXE", enabledisable_t, BITS("%EFER", 11));
			FIELD("LMSLE", enabledisable_t, BITS("%EFER", 13));
			FIELD("FFXSR", enabledisable_t, BITS("%EFER", 14));
		}); // EFER

		SCOPE("SYSCFG", {
			REG64("%SYSCFG", 0xc0010010);
			FIELD("SysAckLimit", int_t, BITS("%SYSCFG", 4, 0));
			FIELD("SysVicLimit", int_t, BITS("%SYSCFG", 7, 5));
			FIELD("SetDirtyEnE", enabledisable_t, BITS("%SYSCFG", 8));
			FIELD("SetDirtyEnS", enabledisable_t, BITS("%SYSCFG", 9));
			FIELD("SetDirtyEnO", enabledisable_t, BITS("%SYSCFG", 10));
			if (EVAL("$cpu/k8_rev") == "rev_e") {
				FIELD("ClVicBlkEn", enabledisable_t, BITS("%SYSCFG", 11));
			}
			FIELD("ChxToDirtyDis", disableenable_t, BITS("%SYSCFG", 16));
			FIELD("SysUcLockEn", enabledisable_t, BITS("%SYSCFG", 17));
			FIELD("MtrrFixDramEn", enabledisable_t, BITS("%SYSCFG", 18));
			FIELD("MtrrFixDramModEn", enabledisable_t, BITS("%SYSCFG", 19));
			FIELD("MtrrVarDramEn", enabledisable_t, BITS("%SYSCFG", 20));
			FIELD("MtrrTom2En", enabledisable_t, BITS("%SYSCFG", 21));
			if (EVAL("$cpu/k8_rev") == "rev_f") {
				FIELD("Tom2ForceMemTypeWB", yesno_t, BITS("%SYSCFG", 22));
			}
		}); // SYSCFG

		SCOPE("HWCR", {
			HEX(fid_t, 8);
			REG64("%HWCR", 0xc0010015);
			FIELD("SMMLOCK", yesno_t, BITS("%HWCR", 0));
			FIELD("SLOWFENCE", enabledisable_t, BITS("%HWCR", 1));
			FIELD("TLBCACHEDIS", disableenable_t, BITS("%HWCR", 3));
			FIELD("INVD_WBINVD", yesno_t, BITS("%HWCR", 4));
			FIELD("FFDIS", disableenable_t, BITS("%HWCR", 6));
			FIELD("DISLOCK", disableenable_t, BITS("%HWCR", 7));
			FIELD("IGNNE_EM", enabledisable_t, BITS("%HWCR", 8));
			FIELD("HLTXSPCYCEN", enabledisable_t, BITS("%HWCR", 12));
			FIELD("SMISPCYCDIS", disableenable_t, BITS("%HWCR", 13));
			FIELD("RSMSPCYCDIS", disableenable_t, BITS("%HWCR", 14));
			FIELD("SSEDIS", disableenable_t, BITS("%HWCR", 15));
			FIELD("WRAP32DIS", disableenable_t, BITS("%HWCR", 17));
			FIELD("MCi_STATUS_WREN", enabledisable_t, BITS("%HWCR", 18));
			FIELD("START_FID", fid_t, BITS("%HWCR", 29, 24));
		}); // HWCR

		SCOPE("NB_CFG", {
			REG64("%NB_CFG", 0xc001001f);
			if (EVAL("$cpu/k8_rev") == "rev_e") {
				FIELD("EnRefUseFreeBuf", enabledisable_t,
						BITS("%NB_CFG", 9));
			} else if (EVAL("$cpu/k8_rev") == "rev_f") {
				FIELD("DisRefUseFreeBuf", disableenable_t,
						BITS("%NB_CFG", 9));
			}
			FIELD("DisCohLdtCfg", disableenable_t, BITS("%NB_CFG", 31));
			FIELD("DisDatMask", disableenable_t, BITS("%NB_CFG", 36));
			FIELD("DisThmlPfMonSmiIntr", disableenable_t, BITS("%NB_CFG", 43));
			FIELD("DisUsSysMgtRqToNLdt", disableenable_t, BITS("%NB_CFG", 45));
			FIELD("InitApicIdCpuIdLo", yesno_t, BITS("%NB_CFG", 54));
		}); // NB_CFG

		SCOPE("id", {
			REG64("%MANID", 0xc001001e);
			FIELD("MinorRev", int_t, BITS("%MANID", 3, 0));
			FIELD("MajorRev", int_t, BITS("%MANID", 7, 4));
			FIELD("ReticleSite", int_t, BITS("%MANID", 9, 8));
		}); // id

		SCOPE("HTC", {
			REG64("%HTC", 0xc001003e);
			FIELD("HtcEn", enabledisable_t, BITS("%HTC", 0));
			FIELD("HtcSbcEn", enabledisable_t, BITS("%HTC", 1));
			FIELD("HtcAct", yesno_t, BITS("%HTC", 4));
			FIELD("HtcActSts", yesno_t, BITS("%HTC", 5));
		}); // htc

		SCOPE("thermal_control", {
			REG64("%ThermalControl", 0xc001003f);
			FIELD("StcSbcTmpHiEn", enabledisable_t,
					BITS("%ThermalControl", 0));
			FIELD("StcSbcTmpLoEn", enabledisable_t, 
					BITS("%ThermalControl", 1));
			FIELD("StcApcTmpHiEn", enabledisable_t,
					BITS("%ThermalControl", 2));
			FIELD("StcApcTmpLoEn", enabledisable_t,
					BITS("%ThermalControl", 3));
			if (EVAL("$cpu/k8_rev") == "rev_f") {
				FIELD("StcHtcEn", enabledisable_t,
						BITS("%ThermalControl", 4));
			}
			FIELD("StcTmpHiSts", yesno_t,
					BITS("%ThermalControl", 6));
			FIELD("StcTmpLoSts", yesno_t,
					BITS("%ThermalControl", 7));
			FIELD("StcTmpLmt", XFORM(INT("C"),
						LAMBDA(52 + (_1 * 2)),
						LAMBDA((_1 - 52) / 2)),
					BITS("%ThermalControl", 20, 16));
			FIELD("StcHystLmt", XFORM(INT("C"),
						LAMBDA(2 + (_1 * 2)),
						LAMBDA((_1 - 2) / 2)),
					BITS("%ThermalControl", 27, 24));
		}); // thermal_control

		REG64("%TOP_MEM", 0xc001001a);
		REG64("%TOP_MEM2", 0xc001001d);
		FIELD("TOM", addr64_t,
				BITS("%TOP_MEM", 39, 23) + BITS("%0", 22, 0));
		FIELD("TOM2", addr64_t,
				BITS("%TOP_MEM2", 39, 23) + BITS("%0", 22, 0));

		for (int i = 0; i < 2; i++) {
			SCOPE("iorr[]", {
				REG64("%IORRBase", 0xc0010016 + (i * 0x02));
				FIELD("WrDram", yesno_t, BITS("%IORRBase", 3));
				FIELD("RdDram", yesno_t, BITS("%IORRBase", 4));
				FIELD("Base", addr64_t,
						BITS("%IORRBase", 39, 12) + BITS("%0", 11, 0));

				REG64("%IORRMask", 0xc0010017 + (i * 0x02));
				FIELD("V", enabledisable_t, BITS("%IORRMask", 11));
				FIELD("Mask", addr64_t,
						BITS("%IORRMask", 39, 12) + BITS("%0", 11, 0));
			}); // iorr[]
		}

		SCOPE("STAR", {
			REG64("%STAR", 0xc0000081);
			FIELD("Target", addr32_t, BITS("%STAR", 31, 0));
			FIELD("SysCallSel", hex16_t, BITS("%STAR", 47, 32));
			FIELD("SysRetSel", hex16_t, BITS("%STAR", 63, 48));
		}); // STAR

		FIELD("LSTAR", addr64_t, REG64(0xc0000082));
		FIELD("CSTAR", addr64_t, REG64(0xc0000082));

		REG64("%SF_MASK", 0xc0000084);
		FIELD("SF_MASK", hex32_t, BITS("%SF_MASK", 31, 0));

		FIELD("FS_BASE", addr64_t, REG64(0xc0000100));
		FIELD("GS_BASE", addr64_t, REG64(0xc0000101)) ;
		FIELD("KernelGSBase", addr64_t, REG64(0xc0000102));

		for (int i = 0; i < 4; i++) {
			SCOPE("perf_ctr[]", {
				REG64("%PerfEvtSel", 0xc0010000 + i);
				FIELD("EVENT_SELECT", ENUM(
					[0x00, "dispatched_fpu_ops"],
					[0x01, "cycles_with_no_fpu_ops_retired"],
					[0x02, "dispatched_fast_flag_fpu_ops"],
					[0x20, "segment_reg_loads"],
					[0x21, "pipeline_restart_self_modifying_code"],
					[0x22, "pipeline_restart_probe_hit"],
					[0x23, "ls_buffer_2_full"],
					[0x24, "locked_ops"],
					[0x65, "mem_requests_by_type"],
					[0x40, "data_cache_accesses"],
					[0x41, "data_cache_misses"],
					[0x42, "data_cache_refills_from_l2_or_system"],
					[0x43, "data_cache_refills_from_system"],
					[0x44, "data_cache_lines_evicted"],
					[0x45, "l1_dtlb_miss_l2_dtlb_hit"],
					[0x46, "l1_and_l2_dtlb_miss"],
					[0x47, "misaligned_accesses"],
					[0x48, "microarch_late_cancel_of_access"],
					[0x49, "microarch_early_cancel_of_access"],
					[0x4a, "single_bit_ecc_err_recorded_by_scrubber"],
					[0x4b, "prefetch_instrs_dispatched"],
					[0x4c, "dcache_misses_by_locked_instrs"],
					[0x67, "data_prefetcher"],
					[0x6c, "system_read_responses_by_coherency_state"],
					[0x6d, "quadwords_written_to_system"],
					[0x7d, "requests_to_l2_cache"],
					[0x7e, "l2_cache_misses"],
					[0x7f, "l2_fill_or_writeback"],
					[0x80, "instr_cache_fetches"],
					[0x81, "instr_cache_misses"],
					[0x82, "instr_cache_refills_from_l2"],
					[0x83, "instr_cache_refills_from_system"],
					[0x84, "l1_itlb_miss_l2_itlb_hit"],
					[0x85, "l1_and_l2_itlb_miss"],
					[0x86, "pipeline_restart_due_to_instr_stream_probe"],
					[0x87, "instr_fetch_stall"],
					[0x88, "return_stack_hits"],
					[0x89, "return_stack_overflows"],
					[0x26, "retired_clflush_instrs"],
					[0x27, "retired_cpuid_instrs"],
					[0x76, "cpu_clocks_not_halted"],
					[0xc0, "retired_instrs"],
					[0xc1, "retired_uops"],
					[0xc2, "retired_branch_instrs"],
					[0xc3, "retired_mispredicted_branch_instrs"],
					[0xc4, "retired_taken_branch_instrs"],
					[0xc5, "retired_taken_branch_instrs_mispredicted"],
					[0xc6, "retired_far_control_transfers"],
					[0xc7, "retired_branch_resyncs"],
					[0xc8, "retired_near_returns"],
					[0xc9, "retired_near_returns_mispredicted"],
					[0xca, "retired_indirect_branches_mispredicted"],
					[0xcb, "retired_mmx_or_fp_instrs"],
					[0xcc, "retired_fastpath_double_op_instrs"],
					[0xcd, "ints_masked_cycles"],
					[0xce, "ints_masked_cycles_with_int_pending"],
					[0xcf, "ints_taken"],
					[0xd0, "decoder_empty"],
					[0xd1, "dispatch_stalls"],
					[0xd2, "dispatch_stall_for_branch_abort_to_retire"],
					[0xd3, "dispatch_stall_for_serialization"],
					[0xd4, "dispatch_stall_for_segment_load"],
					[0xd5, "dispatch_stall_for_reorder_buffer_full"],
					[0xd6, "dispatch_stall_for_reservation_station_full"],
					[0xd7, "dispatch_stall_for_fpu_full"],
					[0xd8, "dispatch_stall_for_ls_full"],
					[0xd9, "dispatch_stall_waiting_for_all_quiet"],
					[0xda, "dispatch_stall_for_far_transfer"],
					[0xdb, "fpu_exceptions"],
					[0xdc, "dr0_breakpoint_matches"],
					[0xdd, "dr1_breakpoint_matches"],
					[0xde, "dr2_breakpoint_matches"],
					[0xdf, "dr3_breakpoint_matches"],
					[0xe0, "dram_accesses"],
					[0xe1, "mem_controller_page_table_overflows"],
					[0xe3, "mem_controller_turnarounds"],
					[0xe4, "mem_controller_bypass_counter_saturation"],
					[0xe5, "sized_blocks"],
					[0xe8, "ecc_errors"],
					[0xe9, "cpu_or_io_requests_to_memory_or_io"],
					[0xea, "cache_block_commands"],
					[0xeb, "sized_commands"],
					[0xec, "probe_responses_and_upstream_requests"],
					[0xee, "gart_events"],
					[0xf6, "ht_link_0_transmit_bandwidth"],
					[0xf7, "ht_link_1_transmit_bandwidth"],
					[0xf8, "ht_link_2_transmit_bandwidth"]),
								BITS("%PerfEvtSel", 7, 0));
				if (EVAL("EVENT_SELECT") == "dispatched_fpu_ops") {
					FIELD("UINT_MASK", BITMASK(
						[0, "add_pipe_ops"],
						[1, "multiply_pipe_ops"],
						[2, "store_pipe_ops"],
						[3, "add_pipe_load_ops"],
						[4, "multiply_pipe_load_ops"],
						[5, "store_pipe_load_ops"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "segment_reg_loads") {
					FIELD("UINT_MASK", BITMASK(
						[0, "es"],
						[1, "cs"],
						[2, "ss"],
						[3, "ds"],
						[4, "fs"],
						[5, "gs"],
						[6, "hs"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "locked_ops") {
					FIELD("UINT_MASK", BITMASK(
						[0, "num_locked_instrs_executed"],
						[1, "num_cycles_in_speculative_phase"],
						[2, "num_cycles_in_nonspeculative_phase"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "mem_requests_by_type") {
					FIELD("UINT_MASK", BITMASK(
						[0, "requests_to_uc_mem"],
						[1, "requests_to_wc_mem_or_wc_buffer_flushes_to_wb_mem"],
						[7, "ss_requests"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "data_cache_refills_from_l2_or_system") {
					FIELD("UINT_MASK", BITMASK(
						[0, "refill_from_sys"],
						[1, "shared_state_line_from_l2"],
						[2, "exclusive_state_line_from_l2"],
						[3, "owned_state_line_from_l2"],
						[4, "modified_state_line_from_l2"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "data_cache_refills_from_system"
				        || EVAL("EVENT_SELECT") == "data_cache_lines_evicted") {
					FIELD("UINT_MASK", BITMASK(
						[0, "invalid"],
						[1, "shared"],
						[2, "exclusive"],
						[3, "owned"],
						[4, "modified"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "single_bit_ecc_err_recorded_by_scrubber") {
					FIELD("UINT_MASK", BITMASK(
						[0, "scrubber_error"],
						[1, "piggyback_scrubber_errors"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "prefetch_instrs_dispatched") {
					FIELD("UINT_MASK", BITMASK(
						[0, "load"],
						[1, "store"],
						[2, "nta"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "dcache_misses_by_locked_instrs")) {
					FIELD("UINT_MASK", BITMASK(
						[1, "data_cache_misses_by_locked_instrs"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "data_prefetcher") {
					FIELD("UINT_MASK", BITMASK(
						[0, "cancelled_prefetches"],
						[1, "prefetch_attempts"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
				                    "system_read_responses_by_coherency_state")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "exclusive"],
						[1, "modified"],
						[2, "shared"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
				                    "quadwords_written_to_system")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "quadword_write_transfer"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "requests_to_l2_cache") {
					FIELD("UINT_MASK", BITMASK(
						[0, "ic_fill"],
						[1, "dc_fill"],
						[2, "tlb_fill"],
						[3, "tag_snoop_request"],
						[4, "cancelled_request"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "l2_cache_misses") {
					FIELD("UINT_MASK", BITMASK(
						[0, "ic_fill"],
						[1, "dc_fill_incl_possible_replays"],
						[2, "tlb_page_table_walk"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "l2_fill_or_writeback") {
					FIELD("UINT_MASK", BITMASK(
						[0, "l2_fills"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "retired_mmx_or_fp_instrs")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "x87_instrs"],
						[1, "mmx_and_3dnow_instrs"],
						[2, "packed_sse_and_sse2_instrs"],
						[3, "scalar_sse_and_sse2_instrs"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "retired_fastpath_double_op_instrs")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "low_op_in_pos_0"],
						[1, "low_op_in_pos_1"],
						[2, "low_op_in_pos_2"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "fpu_exceptions") {
					FIELD("UINT_MASK", BITMASK(
						[0, "x87_reclass_microfaults"],
						[1, "sse_retype_microfaults"],
						[2, "sse_reclass_microfaults"],
						[3, "sse_and_x87_microtraps"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "dram_accesses") {
					FIELD("UINT_MASK", BITMASK(
						[0, "page_hit"],
						[1, "page_miss"],
						[2, "page_conflict"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "mem_controller_turnarounds")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "dimm_chip_select_turnaround"],
						[1, "read_to_write_turnaround"],
						[2, "write_to_read_turnaround"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
				                    "mem_controller_bypass_counter_saturation")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "mem_controller_high_priority_bypass"],
						[1, "mem_controller_low_priority_bypass"],
						[2, "dram_controller_interface_bypass"],
						[3, "dram_controller_queue_bypass"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "sized_blocks") {
					FIELD("UINT_MASK", BITMASK(
						[2, "32_byte_sized_writes"],
						[3, "64_byte_sized_writes"],
						[4, "32_byte_sized_reads"],
						[5, "64_byte_sized_reads"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "ecc_errors") {
					FIELD("UINT_MASK", BITMASK(
						[7, "num_of_dram_ecc_errors"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "cpu_or_io_requests_to_memory_or_io")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "io_to_io"],
						[1, "io_to_mem"],
						[2, "cpu_to_io"],
						[3, "cpu_to_mem"],
						[4, "to_remote_node"],
						[5, "to_local_node"],
						[6, "from_remote_node"],
						[7, "from_local_node"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "cache_block_commands") {
					FIELD("UINT_MASK", BITMASK(
						[0, "victim_block"],
						[2, "read_block"],
						[3, "read_block_shared"],
						[4, "read_block_modified"],
						[5, "change_to_dirty"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "sized_commands") {
					FIELD("UINT_MASK", BITMASK(
						[0, "nonposted_szwr_byte"],
						[1, "nonposted_szwr_dword"],
						[2, "posted_szwr_byte"],
						[3, "posted_szwr_dword"],
						[4, "szrd_byte"],
						[5, "szrd_dword"],
						[6, "rdmodwr"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "probe_responses_and_upstream_requests")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "probe_miss"],
						[1, "probe_hit_clean"],
						[2, "probe_hit_dirty_without_mem_cancel"],
						[3, "probe_hit_dirty_with_mem_cancel"],
						[4, "upstream_display_refresh_reads"],
						[5, "upstream_non_display_refresh_reads"],
						[6, "upstream_writes"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (EVAL("EVENT_SELECT") == "gart_events") {
					FIELD("UINT_MASK", BITMASK(
						[0, "gart_aperture_hit_on_cpu_access"],
						[1, "gart_aperture_hit_on_io_access"],
						[2, "gart_miss"]),
							BITS("%PerfEvtSel", 15, 8));
				} else if (FIELD_EQ("EVENT_SELECT",
						    "ht_link_0_transmit_bandwidth") ||
					   FIELD_EQ("EVENT_SELECT",
						    "ht_link_1_transmit_bandwidth") ||
					   FIELD_EQ("EVENT_SELECT",
						    "ht_link_2_transmit_bandwidth")) {
					FIELD("UINT_MASK", BITMASK(
						[0, "command_dword_sent"],
						[1, "data_dword_sent"],
						[2, "buffer_release_dword_sent"],
						[3, "nop_dword_sent"]),
							BITS("%PerfEvtSel", 15, 8));
				} else {
					// No special interpretation found, fall back to
					// generic bitmask
					FIELD("UNIT_MASK", bitmask_t,
							BITS("%PerfEvtSel", 15, 8));
				}
				FIELD("USR", yesno_t, BITS("%PerfEvtSel", 16));
				FIELD("OS", yesno_t, BITS("%PerfEvtSel", 17));
				FIELD("E", BOOL("edge_detection", "level_detection"),
						BITS("%PerfEvtSel", 18));
				FIELD("PC", BOOL("toggle_on_overflow",
						      "toggle_on_increment"),
						BITS("%PerfEvtSel", 19));
				FIELD("INT", enabledisable_t, BITS("%PerfEvtSel", 20));
				FIELD("EN", enabledisable_t, BITS("%PerfEvtSel", 22));
				FIELD("INV", yesno_t, BITS("%PerfEvtSel", 23));
				if (FIELD_EQ("INV", 0)) {
					FIELD("CNT_MASK",
						ENUM([0, "incr_always"],
							  [1, "incr_if_ge_1"],
							  [2, "incr_if_ge_2"],
							  [3, "incr_if_ge_3"]),
						BITS("%PerfEvtSel", 31, 24));
				} else if (EVAL("INV") == 1) {
					FIELD("CNT_MASK",
						ENUM([0, "incr_always"],
							  [1, "incr_if_lt_1"],
							  [2, "incr_if_lt_2"],
							  [3, "incr_if_lt_3"]),
						BITS("%PerfEvtSel", 31, 24));
				}

				REG64("%PerfCtr", 0xc0010004 + i);
				FIELD("CTR", int_t, BITS("%PerfCtr", 47, 0));
			}); // perf_ctr[]
		}

		SCOPE("power_mgmt", {
			REG64("%FIDVID_CTL", 0xc0010041);
			FIELD("NewFID", fid_t, BITS("%FIDVID_CTL", 5, 0));
			if (EVAL("$cpu/k8_rev") == "rev_e") {
				FIELD("NewVID", vid_t, BITS("%FIDVID_CTL", 12, 8));
			} else if (EVAL("$cpu/k8_rev") == "rev_f") {
				FIELD("NewVID", vid_t, BITS("%FIDVID_CTL", 13, 8));
			}
			FIELD("InitFidVid", yesno_t, BITS("%FIDVID_CTL", 16));
			FIELD("StpGntTOCnt", INT("system clock cycles"),
					BITS("%FIDVID_CTL", 51, 32));

			REG64("%FIDVID_STATUS", 0xc0010042);
			FIELD("CurrFID", fid_t, BITS("%FIDVID_STATUS", 5, 0));
			FIELD("StartFID", fid_t, BITS("%FIDVID_STATUS", 13, 8));
			FIELD("MaxFID", fid_t, BITS("%FIDVID_STATUS", 21, 16));
			FIELD("FidVidPending", yesno_t, BITS("%FIDVID_STATUS", 31));
			if (EVAL("$cpu/k8_rev") == "rev_e") {
				FIELD("MaxRampVID", vid_t, BITS("%FIDVID_STATUS", 28, 24));
				FIELD("CurrVID", vid_t, BITS("%FIDVID_STATUS", 36, 32));
				FIELD("StartVID", vid_t, BITS("%FIDVID_STATUS", 44, 40));
				FIELD("MaxVID", vid_t, BITS("%FIDVID_STATUS", 52, 48));
				FIELD("MinVID", vid_t, BITS("%FIDVID_STATUS", 60, 56));
			} else if (EVAL("$cpu/k8_rev") == "rev_f") {
				FIELD("MaxRampVID", vid_t, BITS("%FIDVID_STATUS", 29, 24));
				FIELD("CurrVID", vid_t, BITS("%FIDVID_STATUS", 37, 32));
				FIELD("StartVID", vid_t, BITS("%FIDVID_STATUS", 45, 40));
				FIELD("MaxVID", vid_t, BITS("%FIDVID_STATUS", 53, 48));
				FIELD("PstateStep", ENUM([0, "mV_25_reduction"],
							      [1, "mV_50_reduction"]),
						BITS("%FIDVID_STATUS", 56));
				FIELD("AltVidOffset", ENUM([0, "not_supported"],
								[1, "mV_neg_50"],
								[2, "mV_neg_100"],
								[3, "mV_neg_125"],
								[4, "mV_neg_150"],
								[5, "mV_neg_175"],
								[6, "mV_neg_200"],
								[7, "mV_neg_225"]),
						BITS("%FIDVID_STATUS", 59, 57));
				FIELD("IntPstateSup", yesno_t, BITS("%FIDVID_STATUS", 61));
			}
		}); // power_mgmt

		SCOPE("mc_ctl_mask", {
			for (int i = 0; i < 5; i++) {
				REG64("%MC_CTL_MASK[]", 0xc0010044 + i);
				FIELD("MC_CTL_MASK[]", bitmask_t, BITS("%MC_CTL_MASK[-1]"));
			}
		}); // mc_ctl_mask

		SCOPE("io_trap", {
			REG64("%IOTRAP_CTL", 0xc0010054);
			for (int i = 0; i < 4; i++) {
				SCOPE("trap[]", {
					REG64("%IOTRAP_ADDR", 0xc0010050 + i);
					FIELD("SmiAddr", addr32_t, BITS("%IOTRAP_ADDR", 31, 0));
					FIELD("SmiMask", addr32_t, BITS("%IOTRAP_ADDR", 55, 32));
					FIELD("ConfigSmi", yesno_t, BITS("%IOTRAP_ADDR", 61));
					FIELD("SmiOnWrEn", enabledisable_t,
							BITS("%IOTRAP_ADDR", 62));
					FIELD("SmiOnRdEn", enabledisable_t,
							BITS("%IOTRAP_ADDR", 63));
					FIELD("SmiSts", yesno_t, BITS("../%IOTRAP_CTL", (2*i)));
					FIELD("SmiEn", enabledisable_t,
							BITS("../%IOTRAP_CTL", (2*i)+1));
				}); // trap[]
			}

			ALIAS("SmiSts_0", "trap[0]/SmiSts");
			ALIAS("SmiEn_0", "trap[0]/SmiEn");
			ALIAS("SmiSts_1", "trap[1]/SmiSts");
			ALIAS("SmiEn_1", "trap[1]/SmiEn");
			ALIAS("SmiSts_2", "trap[2]/SmiSts");
			ALIAS("SmiEn_2", "trap[2]/SmiEn");
			ALIAS("SmiSts_3", "trap[3]/SmiSts");
			ALIAS("SmiEn_3", "trap[3]/SmiEn");
			FIELD("IoTrapCtlRsmSpcEn", enabledisable_t, BITS("%IOTRAP_CTL", 13));
			FIELD("IoTrapCtlSmiSpcEn", enabledisable_t, BITS("%IOTRAP_CTL", 14));
			FIELD("IoTrapEn", enabledisable_t, BITS("%IOTRAP_CTL", 15));
		}); // io_trap

		SCOPE("interrupt_pending_msg", {
			REG64("%InterruptPendingMessage", 0xc0010055);
			FIELD("IOMsgAddr", addr16_t, BITS("%InterruptPendingMessage", 15, 0));
			FIELD("IntrPndMsgDis", disableenable_t,
					BITS("%InterruptPendingMessage", 24));
			FIELD("IntrPndMsg", BOOL("io_space_msg",
						      "ht_int_pending_msg"),
					BITS("%InterruptPendingMessage", 25));
			FIELD("IORd", BOOL("io_read", "io_write"),
					BITS("%InterruptPendingMessage", 26));
			if (EVAL("IORd") == "io_write") {
				FIELD("IOMsgData", hex8_t,
						BITS("%InterruptPendingMessage", 23, 16));
			}
			if (EVAL("$cpu/k8_rev") == "rev_f") {
				FIELD("SmiOnCmpHalt", yesno_t,
						BITS("%InterruptPendingMessage", 27));
				FIELD("C1eOnCmpHalt", yesno_t,
						BITS("%InterruptPendingMessage", 28));
			}
		}); // interrupt_pending_msg

		SCOPE("smm", {
			REG64("%SMM_BASE", 0xc0010111);
			FIELD("SMM_BASE", addr32_t, BITS("%SMM_BASE", 31, 0));

			REG64("%SMM_ADDR", 0xc0010112);
			FIELD("ADDR", addr64_t,
					BITS("%SMM_ADDR", 39, 17) + BITS("%0", 16, 0));

			REG64("%SMM_MASK", 0xc0010113);
			FIELD("AValid", enabledisable_t, BITS("%SMM_MASK", 0));
			FIELD("TValid", enabledisable_t, BITS("%SMM_MASK", 1));
			FIELD("AClose", yesno_t, BITS("%SMM_MASK", 2));
			FIELD("TClose", yesno_t, BITS("%SMM_MASK", 3));
			FIELD("AMTypteIcWc", yesno_t, BITS("%SMM_MASK", 4));
			FIELD("TMTypeIoWc", yesno_t, BITS("%SMM_MASK", 5));
			FIELD("AMTypeDram", mtrr_type_t, BITS("%SMM_MASK", 10, 8));
			FIELD("TMTypeDram", mtrr_type_t, BITS("%SMM_MASK", 14, 12));
			FIELD("MASK", addr64_t,
					BITS("%SMM_MASK", 39, 17) + BITS("%0", 16, 0));
		}); // smm

		if (EVAL("$cpu/k8_rev") == "rev_f") {
			SCOPE("vm", {
				REG64("%VM_CR", 0xc0010114);
				FIELD("dpd", disableenable_t, BITS("%VM_CR", 0));
				FIELD("r_init", BOOL("init_translated_to_sx_int",
							  "normal"),
						BITS("%VM_CR", 1));
				FIELD("dis_a20m", disableenable_t, BITS("%VM_CR", 2));
				FIELD("LOCK", yesno_t, BITS("%VM_CR", 3));
				FIELD("SVME_DISABLE", disableenable_t, BITS("%VM_CR", 4));

				FIELD("VM_HSAVE_PA", addr64_t, REG64(0xc0010117));
			}); // vm
		}

		SCOPE("mc", {
			SCOPE("global", {
				REG64("%MCG_CAP", 0x179);
				REG64("%MCG_STATUS", 0x17A);
				REG64("%MCG_CTL", 0x17B);

				FIELD("Count", INT("banks"), BITS("%MCG_CAP", 7, 0));
				FIELD("MCG_CTL_P", yesno_t, BITS("%MCG_CAP", 8, 0));
				FIELD("RIPV", yesno_t, BITS("%MCG_STATUS", 0));
				FIELD("EIPV", yesno_t, BITS("%MCG_STATUS", 1));
				FIELD("MCIP", yesno_t, BITS("%MCG_STATUS", 2));
				FIELD("DCE", enabledisable_t, BITS("%MCG_CTL", 0));
				FIELD("ICE", enabledisable_t, BITS("%MCG_CTL", 1));
				FIELD("BUE", enabledisable_t, BITS("%MCG_CTL", 2));
				FIELD("LSE", enabledisable_t, BITS("%MCG_CTL", 3));
				FIELD("NBE", enabledisable_t, BITS("%MCG_CTL", 4));

			}); // global

			SCOPE("data_cache", {
				REG64("%MC0_CTL", 0x400);
				REG64("%MC0_STATUS", 0x401);
				REG64("%MC0_ADDR", 0X402);
				// REG64("%MC0_MISC", 0X403); // unsupported on AMD

				FIELD("ECCI", yesno_t, BITS("%MC0_CTL", 0));
				FIELD("ECCM", yesno_t, BITS("%MC0_CTL", 1));
				FIELD("DECC", yesno_t, BITS("%MC0_CTL", 2));
				FIELD("DMTP", yesno_t, BITS("%MC0_CTL", 3));
				FIELD("DSTP", yesno_t, BITS("%MC0_CTL", 4));
				FIELD("L1TP", yesno_t, BITS("%MC0_CTL", 5));
				FIELD("L2TP", yesno_t, BITS("%MC0_CTL", 6));
				FIELD("ERR_CODE", hex16_t, BITS("%MC0_STATUS", 15, 0));
				if (FIELD_TEST("ERR_CODE", 0x0800)) {
					// Bus error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC0_STATUS", 1, 0));
					FIELD("ERR_CODE_MemoryOrIO",
							ENUM([0, "memory_access"],
								  [2, "io_access"],
								  [3, "generic"]),
							BITS("%MC0_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC0_STATUS", 7, 4));
					FIELD("ERR_CODE_Timeout", yesno_t,
							BITS("%MC0_STATUS", 8));
					FIELD("ERR_CODE_ParticipationProcessor",
							ENUM([0, "local_node_originated"],
								  [1, "local_node_responded"],
								  [2, "local_node_third_party"],
								  [3, "generic"]),
							BITS("%MC0_STATUS", 10, 9));
				} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
					// Memory error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC0_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC0_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC0_STATUS", 7, 4));
				} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
					// TLB error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC0_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC0_STATUS", 3, 2));
				}
				FIELD("EXT_ERR_CODE", ENUM([0, "phys_array"],
								[1, "virt_array"]),
						BITS("%MC0_STATUS", 19, 16));
				FIELD("SCRUB", yesno_t, BITS("%MC0_STATUS", 40));
				FIELD("UECC", yesno_t, BITS("%MC0_STATUS", 45));
				FIELD("CECC", yesno_t, BITS("%MC0_STATUS", 46));
				FIELD("SYND", hex8_t, BITS("%MC0_STATUS", 54, 47));
				FIELD("PCC", yesno_t, BITS("%MC0_STATUS", 57));
				FIELD("ADDRV", yesno_t, BITS("%MC0_STATUS", 58));
				FIELD("MISCV", yesno_t, BITS("%MC0_STATUS", 59));
				FIELD("EN", yesno_t, BITS("%MC0_STATUS", 60));
				FIELD("UC", yesno_t, BITS("%MC0_STATUS", 61));
				FIELD("OVER", yesno_t, BITS("%MC0_STATUS", 62));
				FIELD("VAL", yesno_t, BITS("%MC0_STATUS", 63));
				if (EVAL("ADDRV") == 1) {
					// MC0_ADDR bits valid if ADDRV set
					// TODO: what datatype for all of these?
					if (FIELD_TEST("ERR_CODE", 0x0800)) {
						// Bus error
						// System Data / line-fill
						FIELD("Address", addr32_t,
								BITS("%MC0_ADDR", 39, 6));
					} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
						// Memory error
						if (EVAL("ERR_CODE_CacheLevel") == "level_2") {
							// L2 Cache Data / line-fill
							FIELD("Address", addr32_t,
									BITS("%MC0_ADDR", 39, 6));
						} else if (FIELD_EQ("ERR_CODE_MemoryTransactionType",
								    "generic_error")) {
							// Data Array / scrub
							FIELD("Address", hex_t,
									BITS("%MC0_ADDR", 11, 3));
						} else if ((FIELD_EQ("ERR_CODE_MemoryTransactionType",
								     "data_read") ||
							    FIELD_EQ("ERR_CODE_MemoryTransactionType",
								     "data_write"))) {
							// Data Array / load/store
							// or, Data Tag Array / load/store
							FIELD("Address", hex_t,
									BITS("%MC0_ADDR", 39, 3));
						} else if ((FIELD_EQ("ERR_CODE_MemoryTransactionType",
								     "evict") ||
							    FIELD_EQ("ERR_CODE_MemoryTransactionType",
								     "snoop"))) {
							// Data Array / snoop/victim
							// or, Snoop Tag Array / snoop/victim
							FIELD("Address", hex_t,
									BITS("%MC0_ADDR", 11, 6));
							
						}
					} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
						// TLB error
						// L1 Data TLB / load/store
						// or, L2 Data TLB / load/store
						FIELD("Address", hex_t, BITS("%MC0_ADDR", 47, 12));
					}
				}
			}); // data_cache

			SCOPE("instr_cache", {
				REG64("%MC1_CTL", 0X404);
				REG64("%MC1_STATUS", 0X405);
				REG64("%MC1_ADDR", 0X406);
				// REG64("%MC1_MISC", 0X407); // unsupported on AMD

				FIELD("ECCI", yesno_t, BITS("%MC1_CTL", 0));
				FIELD("ECCM", yesno_t, BITS("%MC1_CTL", 1));
				FIELD("IDP", yesno_t, BITS("%MC1_CTL", 2));
				FIELD("IMTP", yesno_t, BITS("%MC1_CTL", 3));
				FIELD("ISTP", yesno_t, BITS("%MC1_CTL", 4));
				FIELD("L1TP", yesno_t, BITS("%MC1_CTL", 5));
				FIELD("L2TP", yesno_t, BITS("%MC1_CTL", 6));
				FIELD("RDDE", yesno_t, BITS("%MC1_CTL", 9));
				FIELD("ERR_CODE", hex16_t, BITS("%MC1_STATUS", 15, 0));
				if (FIELD_TEST("ERR_CODE", 0x0800)) {
					// Bus error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC1_STATUS", 1, 0));
					FIELD("ERR_CODE_MemoryOrIO",
							ENUM([0, "memory_access"],
								  [2, "io_access"],
								  [3, "generic"]),
							BITS("%MC1_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC1_STATUS", 7, 4));
					FIELD("ERR_CODE_Timeout", yesno_t,
							BITS("%MC1_STATUS", 8));
					FIELD("ERR_CODE_ParticipationProcessor",
							ENUM([0, "local_node_originated"],
								  [1, "local_node_responded"],
								  [2, "local_node_third_party"],
								  [3, "generic"]),
							BITS("%MC1_STATUS", 10, 9));
				} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
					// Memory error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC1_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC1_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC1_STATUS", 7, 4));
				} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
					// TLB error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC1_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC1_STATUS", 3, 2));
				}
				FIELD("EXT_ERR_CODE", ENUM([0, "phys_array"],
								[1, "virt_array"]),
						BITS("%MC1_STATUS", 19, 16));
				FIELD("SCRUB", yesno_t, BITS("%MC1_STATUS", 40));
				FIELD("UECC", yesno_t, BITS("%MC1_STATUS", 45));
				FIELD("CECC", yesno_t, BITS("%MC1_STATUS", 46));
				FIELD("SYND", hex8_t, BITS("%MC1_STATUS", 54, 47));
				FIELD("PCC", yesno_t, BITS("%MC1_STATUS", 57));
				FIELD("ADDRV", yesno_t, BITS("%MC1_STATUS", 58));
				FIELD("MISCV", yesno_t, BITS("%MC1_STATUS", 59));
				FIELD("EN", yesno_t, BITS("%MC1_STATUS", 60));
				FIELD("UC", yesno_t, BITS("%MC1_STATUS", 61));
				FIELD("OVER", yesno_t, BITS("%MC1_STATUS", 62));
				FIELD("VAL", yesno_t, BITS("%MC1_STATUS", 63));
				if (EVAL("ADDRV") == 1) {
					// MC1_ADDR bits valid if ADDRV set
					// TODO: what datatype for all of these?
					if (FIELD_TEST("ERR_CODE", 0x0800)) {
						// Bus error
						// System Data / line-fill
						FIELD("Address", addr32_t,
								BITS("%MC1_ADDR", 39, 6));
					} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
						// Memory error
						if (EVAL("ERR_CODE_CacheLevel") == "level_2") {
							// L2 Cache Data / line-fill
							FIELD("Address", addr32_t,
									BITS("%MC1_ADDR", 39, 6));
						} else if (EVAL("UC") == 1) {
							// Snoop Tag Array / snoop/victim
							FIELD("Address", hex_t,
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
						FIELD("Address", hex_t, BITS("%MC1_ADDR", 47, 12));
					}
				}

			}); // instr_cache

			SCOPE("bus", {
				REG64("%MC2_CTL", 0X408);
				REG64("%MC2_STATUS", 0X409);
				REG64("%MC2_ADDR", 0X40a);
				// REG64("%MC2_MISC", 0X40b); // unsupported

				FIELD("S_RDE_HP", yesno_t, BITS("%MC2_CTL", 0));
				FIELD("S_RED_TLB", yesno_t, BITS("%MC2_CTL", 1));
				FIELD("S_RDE_ALL", yesno_t, BITS("%MC2_CTL", 2));
				FIELD("S_ECC1_TLB", yesno_t, BITS("%MC2_CTL", 3));
				FIELD("S_ECC1_HP", yesno_t, BITS("%MC2_CTL", 4));
				FIELD("S_ECCM_TLB", yesno_t, BITS("%MC2_CTL", 5));
				FIELD("S_ECCM_HP", yesno_t, BITS("%MC2_CTL", 6));
				FIELD("L2T_PAR_ICDC", yesno_t, BITS("%MC2_CTL", 7));
				FIELD("L2T_PAR_TLB", yesno_t, BITS("%MC2_CTL", 8));
				FIELD("L2T_PAR_SNP", yesno_t, BITS("%MC2_CTL", 9));
				FIELD("L2T_PAR_CPB", yesno_t, BITS("%MC2_CTL", 10));
				FIELD("L2T_PAR_SCR", yesno_t, BITS("%MC2_CTL", 11));
				FIELD("L2D_ECC1_TLB", yesno_t, BITS("%MC2_CTL", 12));
				FIELD("L2D_ECC1_SNP", yesno_t, BITS("%MC2_CTL", 13));
				FIELD("L2D_ECC1_CPB", yesno_t, BITS("%MC2_CTL", 14));
				FIELD("L2D_ECCM_TLB", yesno_t, BITS("%MC2_CTL", 15));
				FIELD("L2D_ECCM_SNP", yesno_t, BITS("%MC2_CTL", 16));
				FIELD("L2D_ECCM_CPB", yesno_t, BITS("%MC2_CTL", 17));
				FIELD("L2T_ECC1_SCR", yesno_t, BITS("%MC2_CTL", 18));
				FIELD("L2T_ECCM_SCR", yesno_t, BITS("%MC2_CTL", 19));
				FIELD("ERR_CODE", hex16_t, BITS("%MC2_STATUS", 15, 0));
				if (FIELD_TEST("ERR_CODE", 0x0800)) {
					// Bus error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC2_STATUS", 1, 0));
					FIELD("ERR_CODE_MemoryOrIO",
							ENUM([0, "memory_access"],
								  [2, "io_access"],
								  [3, "generic"]),
							BITS("%MC2_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC2_STATUS", 7, 4));
					FIELD("ERR_CODE_Timeout", yesno_t,
							BITS("%MC2_STATUS", 8));
					FIELD("ERR_CODE_ParticipationProcessor",
							ENUM([0, "local_node_originated"],
								  [1, "local_node_responded"],
								  [2, "local_node_third_party"],
								  [3, "generic"]),
							BITS("%MC2_STATUS", 10, 9));
				} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
					// Memory error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC2_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC2_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC2_STATUS", 7, 4));
				} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
					// TLB error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC2_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC2_STATUS", 3, 2));
				}
				FIELD("EXT_ERR_CODE", ENUM([0, "bus_or_cache_data_array_error"],
								[2, "cache_tag_array_error"]),
						BITS("%MC2_STATUS", 19, 16));
				FIELD("SCRUB", yesno_t, BITS("%MC2_STATUS", 40));
				FIELD("UECC", yesno_t, BITS("%MC2_STATUS", 45));
				FIELD("CECC", yesno_t, BITS("%MC2_STATUS", 46));
				FIELD("SYND", hex8_t, BITS("%MC2_STATUS", 54, 47));
				FIELD("PCC", yesno_t, BITS("%MC2_STATUS", 57));
				FIELD("ADDRV", yesno_t, BITS("%MC2_STATUS", 58));
				FIELD("MISCV", yesno_t, BITS("%MC2_STATUS", 59));
				FIELD("EN", yesno_t, BITS("%MC2_STATUS", 60));
				FIELD("UC", yesno_t, BITS("%MC2_STATUS", 61));
				FIELD("OVER", yesno_t, BITS("%MC2_STATUS", 62));
				FIELD("VAL", yesno_t, BITS("%MC2_STATUS", 63));
				if (EVAL("ADDRV") == 1) {
					// MC2_ADDR bits valid if ADDRV set
					// TODO: what datatype for all of these?
					if (FIELD_TEST("ERR_CODE", 0x0800 | 0x0100) &&
					    EVAL("ERR_CODE_CacheLevel") == "level_2") {
						// L2 Cache error
						if (FIELD_EQ("EXT_ERR_CODE", 
							     "bus_or_cache_data_array_error")) {
							FIELD("Address", hex_t,
									BITS("%MC2_ADDR", 39, 6));
						} else if (FIELD_EQ("EXT_ERR_CODE",
								    "cache_tag_array_error")) {
							// TODO: how do I determine L2 cache size?
						}
					}
					// TODO: how do I detect a system address out of range error?
				}
			}); // bus

			SCOPE("ls", {
				REG64("%MC3_CTL", 0X40c);
				REG64("%MC3_STATUS", 0X40d);
				REG64("%MC3_ADDR", 0X40e);
				// REG64("%MC3_MISC", 0X40f); // unsupported on AMD

				FIELD("S_RDE_L", yesno_t, BITS("%MC3_CTL", 0));
				FIELD("S_RDE_S", yesno_t, BITS("%MC3_CTL", 0));
				FIELD("ERR_CODE", hex16_t, BITS("%MC3_STATUS", 15, 0));
				if (FIELD_TEST("ERR_CODE", 0x0800)) {
					// Bus error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC3_STATUS", 1, 0));
					FIELD("ERR_CODE_MemoryOrIO",
							ENUM([0, "memory_access"],
								  [2, "io_access"],
								  [3, "generic"]),
							BITS("%MC3_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC3_STATUS", 7, 4));
					FIELD("ERR_CODE_Timeout", yesno_t,
							BITS("%MC3_STATUS", 8));
					FIELD("ERR_CODE_ParticipationProcessor",
							ENUM([0, "local_node_originated"],
								  [1, "local_node_responded"],
								  [2, "local_node_third_party"],
								  [3, "generic"]),
							BITS("%MC3_STATUS", 10, 9));
				} else if (FIELD_TEST("ERR_CODE", 0x0100)) {
					// Memory error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC3_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC3_STATUS", 3, 2));
					FIELD("ERR_CODE_MemoryTransactionType",
							ENUM([0, "generic_error"],
								  [1, "generic_read"],
								  [2, "generic_write"],
								  [3, "data_read"],
								  [4, "data_write"],
								  [5, "instruction_fetch"],
								  [6, "prefetch"],
								  [7, "evict"],
								  [8, "snoop"]),
							BITS("%MC3_STATUS", 7, 4));
				} else if (FIELD_TEST("ERR_CODE", 0x0010)) {
					// TLB error
					FIELD("ERR_CODE_CacheLevel",
							ENUM([0, "level_0"],
								  [1, "level_1"],
								  [2, "level_2"],
								  [3, "generic"]),
							BITS("%MC3_STATUS", 1, 0));
					FIELD("ERR_CODE_TransactionType",
							ENUM([0, "instruction"],
								  [1, "data"],
								  [2, "generic"]),
							BITS("%MC3_STATUS", 3, 2));
				}
				FIELD("SCRUB", yesno_t, BITS("%MC3_STATUS", 40));
				FIELD("UECC", yesno_t, BITS("%MC3_STATUS", 45));
				FIELD("CECC", yesno_t, BITS("%MC3_STATUS", 46));
				FIELD("SYND", hex8_t, BITS("%MC3_STATUS", 54, 47));
				FIELD("PCC", yesno_t, BITS("%MC3_STATUS", 57));
				FIELD("ADDRV", yesno_t, BITS("%MC3_STATUS", 58));
				FIELD("MISCV", yesno_t, BITS("%MC3_STATUS", 59));
				FIELD("EN", yesno_t, BITS("%MC3_STATUS", 60));
				FIELD("UC", yesno_t, BITS("%MC3_STATUS", 61));
				FIELD("OVER", yesno_t, BITS("%MC3_STATUS", 62));
				FIELD("VAL", yesno_t, BITS("%MC3_STATUS", 63));
				if (EVAL("ADDRV") == 1) {
					// MC3_ADDR bits valid if ADDRV set
					// TODO: how do I detect a system address out of range error?
				}
			}); // ls

			SCOPE("nb", {
			}); // nb
		}); // mc
	}); // msr
}


k8_ht_config(int seg, int bus, int dev, int fun)
{
	PCI_SCOPE("ht_config", seg, bus, dev, fun, {

		//
		// HT routing table registers
		//

		SCOPE("ht_route", {
			// each HT route is a 4 bit bitmask
			type ht_route_t = BITMASK(
					 [0, "self"],
					[1, "link0"],
					[2, "link1"],
					[3, "link2"]);

			for (int addr = 0x40; addr < 0x60; addr += 4) {
				SCOPE("node[]", {

				REG32("%ht_route", addr);
				FIELD("RQRoute", ht_route_t, BITS("%ht_route", 3, 0));
				FIELD("RPRoute", ht_route_t, BITS("%ht_route", 11, 8));
				FIELD("BCRoute", ht_route_t, BITS("%ht_route", 19, 16));

				});
			}
		});

		//
		// Node ID register
		//
		REG32("%node_id", 0x60);
		FIELD("NodeId", int_t, BITS("%node_id", 2, 0));
		FIELD("NodeCnt", XFORM(int_t, LAMBDA(_1+1), LAMBDA(_1-1)),
				BITS("%node_id", 6, 4));
		FIELD("SbNode", int_t, BITS("%node_id", 10, 8));
		FIELD("LkNode", int_t, BITS("%node_id", 14, 12));
		FIELD("CpuCnt", XFORM(int_t, LAMBDA(_1+1), LAMBDA(_1-1)),
				BITS("%node_id", 19, 16));

		//
		// Unit ID register
		//
		REG32("%unit_id", 0x64);
		FIELD("C0Unit", int_t, BITS("%unit_id", 1, 0));
		FIELD("C1Unit", int_t, BITS("%unit_id", 3, 2));
		FIELD("MctUnit", int_t, BITS("%unit_id", 5, 4));
		FIELD("HbUnit", int_t, BITS("%unit_id", 7, 6));
		FIELD("SbLink", int_t, BITS("%unit_id", 10, 8));

		//
		// Link transaction control register
		//
		REG32("%link_txn_ctrl", 0x68);
		FIELD("DisRdBP", yesno_t, BITS("%link_txn_ctrl", 0));
		FIELD("DisRdDwP", yesno_t, BITS("%link_txn_ctrl", 1));
		FIELD("DisWrBP", yesno_t, BITS("%link_txn_ctrl", 2));
		FIELD("DisWrDwP", yesno_t, BITS("%link_txn_ctrl", 3));
		FIELD("DisMTS", yesno_t, BITS("%link_txn_ctrl", 4));
		FIELD("Cpu1En", yesno_t, BITS("%link_txn_ctrl", 5));
		FIELD("CPUReqPassPW", yesno_t, BITS("%link_txn_ctrl", 6));
		FIELD("CPURdRspPassPW", yesno_t, BITS("%link_txn_ctrl", 7));
		FIELD("DisPMemC", yesno_t, BITS("%link_txn_ctrl", 8));
		FIELD("DisRmtPMemC", yesno_t, BITS("%link_txn_ctrl", 9));
		FIELD("DisFillP", yesno_t, BITS("%link_txn_ctrl", 10));
		FIELD("RespPassPW", yesno_t, BITS("%link_txn_ctrl", 11));
		FIELD("BufRelPri", ENUM(
					[0, "64"],
					[1, "16"],
					[2, "8"],
					[3, "2"]),
				BITS("%link_txn_ctrl", 14, 13));
		FIELD("LimitCldtCfg", yesno_t, BITS("%link_txn_ctrl", 15));
		FIELD("LintEn", yesno_t, BITS("%link_txn_ctrl", 16));
		FIELD("ApicExtBrdCst", ENUM(
					[0, "0x0f"],
					[1, "0xff"]),
				BITS("%link_txn_ctrl", 17));
		FIELD("ApicExtId", yesno_t, BITS("%link_txn_ctrl", 18));
		FIELD("ApicExtSpur", yesno_t, BITS("%link_txn_ctrl", 19));
		FIELD("SeqIdSrcNodeEn", yesno_t, BITS("%link_txn_ctrl", 20));
		FIELD("DsNpReqLmt", ENUM(
					[0, "no limit"],
					[1, "1"],
					[2, "4"],
					[3, "8"]),
				BITS("%link_txn_ctrl", 22, 21));
		if (EVAL("$k8/k8_rev") == "rev_f") {
			FIELD("InstallStateS", yesno_t, BITS("%link_txn_ctrl", 23));
		}
		FIELD("DispRefModeEn", yesno_t, BITS("%link_txn_ctrl", 24));
		FIELD("CHtExtAddrEn", yesno_t, BITS("%link_txn_ctrl", 25));
		FIELD("EnPReqHiPriTblWlk", yesno_t, BITS("%link_txn_ctrl", 31));

		//
		// Link initialization control register
		//
		REG32("%link_init_ctrl", 0x68);
		FIELD("RouteTblDis", yesno_t, BITS("%link_init_ctrl", 0));
		FIELD("ReqDis", yesno_t, BITS("%link_init_ctrl", 1));
		FIELD("DefLnk", ENUM(
					[0, "link0"],
					[1, "link1"],
					[2, "link2"],
					[3, "link3"],
					[4, "self"]),
				BITS("%link_init_ctrl", 3, 2));
		FIELD("ColdRstDet", yesno_t, BITS("%link_init_ctrl", 4));
		if (EVAL("$k8/k8_rev") == "rev_f") {
			FIELD("BiosRstDet", hex_t,
					BITS("%link_init_ctrl", 5) +
					BITS("%link_init_ctrl", 10, 9));
		} else  {
			FIELD("BiosRstDet", hex_t, BITS("%link_init_ctrl", 5));
		}
		FIELD("InitDet", yesno_t, BITS("%link_init_ctrl", 6));
		FIELD("DefSubLnk", int_t, BITS("%link_init_ctrl", 8));
		FIELD("Tr0Double", yesno_t, BITS("%link_init_ctrl", 20));

		// The HyperTransport capability blocks are handled by the generic
		// PCI code.

		//
		// k8-specific HT registers
		//
		for (int addr = 0x90; addr < 0x100; addr += 0x20) {
			SCOPE("ldt[]", {
				//
				// HT buffer count
				//
				REG32("%ldt_buf_count", addr);
				FIELD("Req", int_t, BITS("%ldt_buf_count", 3, 0));
				FIELD("PReq", int_t, BITS("%ldt_buf_count", 7, 4));
				FIELD("Rsp", int_t, BITS("%ldt_buf_count", 11, 8));
				FIELD("Probe", int_t, BITS("%ldt_buf_count", 15, 12));
				FIELD("ReqD", int_t, BITS("%ldt_buf_count", 18, 16));
				FIELD("PReqD", int_t, BITS("%ldt_buf_count", 22, 20));
				FIELD("RspD", int_t, BITS("%ldt_buf_count", 26, 24));

				//
				// HT bus nums
				//
				REG32("%ldt_bus_nums", addr+4);
				FIELD("PriBusNum", int_t, BITS("%ldt_bus_nums", 7, 0));
				FIELD("SecBusNum", int_t, BITS("%ldt_bus_nums", 15, 8));
				FIELD("SubBusNum", int_t, BITS("%ldt_bus_nums", 23, 16));

				//
				// HT type
				//
				REG32("%ldt_type", addr+8);
				FIELD("LinkCon", yesno_t, BITS("%ldt_type", 0));
				FIELD("InitComplete", yesno_t, BITS("%ldt_type", 1));
				FIELD("NC", BOOL("noncoherent", "coherent"),
						BITS("%ldt_type", 2));
				FIELD("UniP_cLDT", BOOL("uniproc_coherent_nb", "normal"),
						BITS("%ldt_type", 3));
				FIELD("LinkConPend", yesno_t, BITS("%ldt_type", 4));

			});
		}
	}); // ht_config
}


k8_address_map(int seg, int bus, int dev, int fun)
{
	PCI_SCOPE("address_map", seg, bus, dev, fun+1, {

		//
		// DRAM base/limit registers
		//
		for (int addr = 0x40; addr < 0x80; addr += 0x08) {
			SCOPE("dram[]", {
				REG32("%dram_base", addr);
				REG32("%dram_limit", addr + 0x04);

				FIELD("RE", enabledisable_t, BITS("%dram_base", 0));
				FIELD("WE", enabledisable_t, BITS("%dram_base", 1));
				if (EVAL("RE") || EVAL("WE")) {
					FIELD("DRAMBase", addr64_t,
					    BITS("%dram_base", 31, 16) + BITS("%0", 23, 0));
					FIELD("DRAMLimit", addr64_t,
					    BITS("%dram_limit", 31, 16) + BITS("%1", 23, 0));
					FIELD("DstNode", int_t, BITS("%dram_limit", 2, 0));
					FIELD("IntlvEn", ENUM([0, "none"],
							[1, "a12"],
							[3, "a12_a13"],
							[7, "a12_a13_a14"]),
						BITS("%dram_base", 10, 8));
					FIELD("IntlvSel", BITMASK(
							[0, "a12"],
							[1, "a13"],
							[2, "a14"]),
						BITS("%dram_limit", 10, 8));
				}
			});
		}

		//
		// Memory-mapped I/O base/limit registers
		//
		for (int addr = 0x80; addr < 0xc0; addr += 0x08) {
			SCOPE("mmio[]", {
				REG32("%mmio_base", addr);
				REG32("%mmio_limit", addr + 0x04);

				FIELD("RE", enabledisable_t, BITS("%mmio_base", 0));
				FIELD("WE", enabledisable_t, BITS("%mmio_base", 1));
				if (EVAL("RE") || EVAL("WE")) {
					FIELD("MMIOBase", addr64_t,
					    BITS("%mmio_base", 31, 8) + BITS("%0", 15, 0));
					FIELD("MMIOLimit", addr64_t,
					    BITS("%mmio_limit", 31, 8) + BITS("%1", 15, 0));
					FIELD("DstNode", int_t, BITS("%mmio_limit", 2, 0));
					FIELD("DstLink", int_t, BITS("%mmio_limit", 5, 4));
					FIELD("NP", yesno_t, BITS("%mmio_limit", 7));
					FIELD("CpuDis", yesno_t, BITS("%mmio_base", 2));
					FIELD("Lock", yesno_t, BITS("%mmio_base", 3));
				}
			});
		}

		//
		// PCI I/O base/limit registers
		//
		for (int addr = 0xc0; addr < 0xe0; addr += 0x08) {
			SCOPE("pciio[]", {
				REG32("%pciio_base", addr);
				REG32("%pciio_limit", addr + 0x04);

				FIELD("RE", enabledisable_t, BITS("%pciio_base", 0));
				FIELD("WE", enabledisable_t, BITS("%pciio_base", 1));
				if (EVAL("RE") || EVAL("WE")) {
					FIELD("PCIIOBase", addr64_t,
					    BITS("%pciio_base", 24, 12) + BITS("%0", 11, 0));
					FIELD("PCIIOLimit", addr64_t,
					    BITS("%pciio_limit", 24, 12) + BITS("%1", 11, 0));
					FIELD("DstNode", int_t, BITS("%pciio_limit", 2, 0));
					FIELD("DstLink", int_t, BITS("%pciio_limit", 5, 4));
					FIELD("VE", enabledisable_t, BITS("%pciio_base", 4));
					FIELD("IE", enabledisable_t, BITS("%pciio_base", 5));
				}
			});
		}

		//
		// Configuration base/limit registers
		//
		for (int addr = 0xe0; addr < 0xf0; addr += 0x04) {
			SCOPE("config[]", {
				REG32("%config", addr);
				FIELD("RE", enabledisable_t, BITS("%config", 0));
				FIELD("WE", enabledisable_t, BITS("%config", 1));
				if (EVAL("RE") || EVAL("WE")) {
					FIELD("DstNode", int_t, BITS("%config", 6, 4));
					FIELD("DstLink", int_t, BITS("%config", 9, 8));
					FIELD("BusNumBase", int_t, BITS("%config", 23, 16));
					FIELD("BusNumLimit", int_t, BITS("%config", 31, 24));
					FIELD("DevCmpEn", enabledisable_t,
							BITS("%config", 2));
				}
			});
		}

		//
		// DRAM hole address register
		//
		REG32("%DRAMHole", 0xf0);
		SCOPE("dram_hole", {
			FIELD("DramHoleValid", truefalse_t, BITS("../%DRAMHole", 0));
			if (EVAL("DramHoleValid")) {
				FIELD("DramHoleOffset", hex32_t,
					BITS("../%DRAMHole", 15, 8) + BITS("%0", 23, 0));
				FIELD("DramHoleBase", hex32_t,
					BITS("../%DRAMHole", 31, 24) + BITS("%0", 23, 0));
			}
		});
	}); // address_map
}

// Helper functions for use in defining the DRAM additional data registers.
dram_additional_data_read(int index)
{
	WRITE("../%dram_additional_data_offset", index);
	while (READ(BITS("../%dram_additional_data_offset", 31)) == 0)
		{} // do nothing
	return READ("../%dram_additional_data_port");
}
dram_additional_data_write(int index, int value)
{
	const int DCT_ACCESS_WRITE = (1<<30);
	WRITE("../%dram_additional_data_port", value);
	WRITE("../%dram_additional_data_offset", index | DCT_ACCESS_WRITE);
	while (READ(BITS("../%dram_additional_data_offset", 31)) == 0)
		{} // do nothing
}

dram_additional_data(string name, int address)
{
	SCOPE(name, {
		//
		// Output driver compensation control
		//
		REG32("%output_driver_compensation_ctrl",
		      BIND(dram_additional_data_read,  address + 0x00),
		      BIND(dram_additional_data_write, address + 0x00));
		FIELD("CkeDreStren", ENUM(
					[0, "1.0x"],
					[1, "1.25x"],
					[2, "1.5x"],
					[3, "2.0x"]),
				BITS("%output_driver_compensation_ctrl", 1, 0));
		FIELD("CsOdtDrvStren", ENUM(
					[0, "1.0x"],
					[1, "1.25x"],
					[2, "1.5x"],
					[3, "2.0x"]),
				BITS("%output_driver_compensation_ctrl", 5, 4));
		FIELD("AddrCmdDrvStren", ENUM(
					[0, "1.0x"],
					[1, "1.25x"],
					[2, "1.5x"],
					[3, "2.0x"]),
				BITS("%output_driver_compensation_ctrl", 9, 8));
		FIELD("ClkDrvStren", ENUM(
					[0, "0.75x"],
					[1, "1.0x"],
					[2, "1.25x"],
					[3, "1.5x"]),
				BITS("%output_driver_compensation_ctrl", 13, 12));
		FIELD("DataDrvStren", ENUM(
					[0, "0.75x"],
					[1, "1.0x"],
					[2, "1.25x"],
					[3, "1.5x"]),
				BITS("%output_driver_compensation_ctrl", 17, 16));
		FIELD("DqsDrvStren", ENUM(
					[0, "0.75x"],
					[1, "1.0x"],
					[2, "1.25x"],
					[3, "1.5x"]),
				BITS("%output_driver_compensation_ctrl", 21, 20));
		FIELD("ProcOdt", ENUM(
					[0, "300 ohms"],
					[1, "150 ohms"],
					[2, "75 ohms"]),
				BITS("%output_driver_compensation_ctrl", 29, 28));

		//
		// Write data timing control
		//
		REG32("%write_data_timing_lo_ctrl",
		      BIND(dram_additional_data_read,  address + 0x01),
		      BIND(dram_additional_data_write, address + 0x01));
		REG32("%write_data_timing_hi_ctrl",
		      BIND(dram_additional_data_read,  address + 0x02),
		      BIND(dram_additional_data_write, address + 0x02));

		FIELD("WrDatTimeByte0", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_lo_ctrl", 5, 0));
		FIELD("WrDatTimeByte1", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_lo_ctrl", 13, 8));
		FIELD("WrDatTimeByte2", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_lo_ctrl", 21, 16));
		FIELD("WrDatTimeByte3", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_lo_ctrl", 29, 24));
		FIELD("WrDatTimeByte4", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_hi_ctrl", 5, 0));
		FIELD("WrDatTimeByte5", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_hi_ctrl", 13, 8));
		FIELD("WrDatTimeByte6", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_hi_ctrl", 21, 16));
		FIELD("WrDatTimeByte7", INT("/96 MEMCLK"),
		      BITS("%write_data_timing_hi_ctrl", 29, 24));

		REG32("%write_data_ecc_timing_ctrl",
		      BIND(dram_additional_data_read,  address + 0x03),
		      BIND(dram_additional_data_write, address + 0x03));
		FIELD("WrChkTime", INT("/96 MEMCLK"),
		      BITS("%write_data_ecc_timing_ctrl", 5, 0));

		//
		// Address timing control
		//
		REG32("%address_timing_ctrl",
		      BIND(dram_additional_data_read,  address + 0x04),
		      BIND(dram_additional_data_write, address + 0x04));
		FIELD("CkeFineDelay", INT("/64 MEMCLK"),
		      BITS("%address_timing_ctrl", 4, 0));
		FIELD("CkeSetup", BOOL("1 MEMCLK", "1/2 MEMCLK"),
		      BITS("%address_timing_ctrl", 5));
		FIELD("CsOdtFineDelay", INT("/64 MEMCLK"),
		      BITS("%address_timing_ctrl", 12, 8));
		FIELD("CsOdtSetup", BOOL("1 MEMCLK", "1/2 MEMCLK"),
		      BITS("%address_timing_ctrl", 13));
		FIELD("AddrCmdFineDelay", INT("/64 MEMCLK"),
		      BITS("%address_timing_ctrl", 20, 16));
		FIELD("AddrCmdSetup", BOOL("1 MEMCLK", "1/2 MEMCLK"),
		      BITS("%address_timing_ctrl", 21));
		FIELD("AtcDllMaxPhases", BOOL("64 phases", "32 phases"),
		      BITS("%address_timing_ctrl", 28));

		//
		// Read DQS timing control
		//
		REG32("%read_dqs_timing_lo_ctrl",
		      BIND(dram_additional_data_read,  address + 0x05),
		      BIND(dram_additional_data_write, address + 0x05));
		REG32("%read_dqs_timing_hi_ctrl",
		      BIND(dram_additional_data_read,  address + 0x06),
		      BIND(dram_additional_data_write, address + 0x06));

		FIELD("RdDqsTimeByte0", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_lo_ctrl", 5, 0));
		FIELD("RdDqsTimeByte1", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_lo_ctrl", 13, 8));
		FIELD("RdDqsTimeByte2", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_lo_ctrl", 21, 16));
		FIELD("RdDqsTimeByte3", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_lo_ctrl", 29, 24));
		FIELD("RdDqsTimeByte4", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_hi_ctrl", 5, 0));
		FIELD("RdDqsTimeByte5", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_hi_ctrl", 13, 8));
		FIELD("RdDqsTimeByte6", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_hi_ctrl", 21, 16));
		FIELD("RdDqsTimeByte7", INT("/96 MEMCLK"),
		      BITS("%read_dqs_timing_hi_ctrl", 29, 24));

		REG32("%read_dqs_ecc_timing_ctrl",
		      BIND(dram_additional_data_read,  address + 0x07),
		      BIND(dram_additional_data_write, address + 0x07));
		FIELD("RdDqsTimeCheck", INT("/96 MEMCLK"),
		      BITS("%write_data_ecc_timing_ctrl", 5, 0));

		//
		// DQS receiver enable timing control
		//
		REG32("%dqs_receiver_enable_timing_ctrl[]",
		      BIND(dram_additional_data_read,  address + 0x10),
		      BIND(dram_additional_data_write, address + 0x10));
		FIELD("DqsRcvEnDelay[]", XFORM(INT("ps"),
					LAMBDA(_1 * 50), LAMBDA(_1)),
				BITS("%dqs_receiver_enable_timing_ctrl[-1]", 7, 0));
		REG32("%dqs_receiver_enable_timing_ctrl[]",
		      BIND(dram_additional_data_read,  address + 0x13),
		      BIND(dram_additional_data_write, address + 0x13));
		FIELD("DqsRcvEnDelay[]", XFORM(INT("ps"),
					LAMBDA(_1 * 50), LAMBDA(_1)),
				BITS("%dqs_receiver_enable_timing_ctrl[-1]", 7, 0));
		REG32("%dqs_receiver_enable_timing_ctrl[]",
		      BIND(dram_additional_data_read,  address + 0x16),
		      BIND(dram_additional_data_write, address + 0x16));
		FIELD("DqsRcvEnDelay[]", XFORM(INT("ps"),
					LAMBDA(_1 * 50), LAMBDA(_1)),
				BITS("%dqs_receiver_enable_timing_ctrl[-1]", 7, 0));
		REG32("%dqs_receiver_enable_timing_ctrl[]",
		      BIND(dram_additional_data_read,  address + 0x19),
		      BIND(dram_additional_data_write, address + 0x19));
		FIELD("DqsRcvEnDelay[]", XFORM(INT("ps"),
					LAMBDA(_1 * 50), LAMBDA(_1)),
				BITS("%dqs_receiver_enable_timing_ctrl[-1]", 7, 0));
	});
}

k8_dram_controller(int seg, int bus, int dev, int fun)
{
	PCI_SCOPE("dram_ctrl", seg, bus, dev, fun+2, {
		//
		// DRAM CS Base registers
		//
		for (int addr = 0x40; addr < 0x60; addr += 0x04) {
			SCOPE("cs_base[]", {
				REG32("%cs_base", addr);

				if (EVAL("$k8/k8_rev") == "rev_e") {
					FIELD("CSBE", enabledisable_t,
							BITS("%cs_base", 0));
					FIELD("BaseAddr", addr64_t,
							BITS("%cs_base", 31, 21) +
							BITS("%0", 24, 20) +
							BITS("%cs_base", 15, 9) +
							BITS("%0", 12, 0));
				} else if (EVAL("$k8/k8_rev") == "rev_f") {
					FIELD("CSEnable", enabledisable_t,
							BITS("%cs_base", 0));
					FIELD("Spare", yesno_t,
							BITS("%cs_base", 1));
					FIELD("TestFail", yesno_t,
							BITS("%cs_base", 2));
					FIELD("BaseAddr", addr64_t,
							BITS("%cs_base", 28, 19) +
							BITS("%0", 26, 22) +
							BITS("%cs_base", 13, 5) +
							BITS("%0", 12, 0));
				}
			});
		}

		//
		// DRAM CS Mask registers
		//
		if (EVAL("$k8/k8_rev") == "rev_e") {
			for (int addr = 0x60; addr < 0x80; addr += 0x04) {
				SCOPE("cs_mask[]", {
					REG32("%cs_mask", addr);
					FIELD("AddrMask", hex64_t,
							BITS("%cs_mask", 29, 21) +
							BITS("%0", 24, 20) +
							BITS("%cs_mask", 15, 9) +
							BITS("%0", 12, 0)); 
				});
			}
		} else if (EVAL("$k8/k8_rev") == "rev_f") {
			for (int addr = 0x60; addr <= 0x6C; addr += 0x04) {
				SCOPE("cs_mask[]", {
					REG32("%cs_mask", addr);
					FIELD("AddrMask", hex64_t,
							BITS("%cs_mask", 28, 19) +
							BITS("%0", 26, 22) +
							BITS("%cs_mask", 13, 5) +
							BITS("%0", 12, 0));
				});
			}
		}

		if (EVAL("$k8/k8_rev") == "rev_f") {
			//
			// DRAM Control register
			//
			REG32("%dram_control", 0x78);
			SCOPE("dram_control", {
				FIELD("RdPtrInit", hex_t, BITS("../%dram_control", 3, 0));
				FIELD("RdPadRcvFifoDly", XFORM(
						FIXED(1, "memory clocks(s)" ),
							LAMBDA(_1+1), LAMBDA(_1-1)),
						BITS("../%dram_control", 6, 4));
				FIELD("AltVidC3MemClkTriEn", enabledisable_t,
						BITS("../%dram_control", 16));
				FIELD("DllTempAdjTime", ENUM([0, "ms_5"],
								  [1, "ms_1"]),
						BITS("../%dram_control", 17));
				FIELD("DqsRcvEnTrain", ENUM([0, "normal_mode"],
								 [1, "training_mode"]),
						BITS("../%dram_control", 18));
			});

			//
			// DRAM Initialization register
			//

			REG32("%dram_init", 0x7C);
			SCOPE("dram_init", {
				type send_t = BOOL("sending", "not_sending");

				FIELD("MrsAddress", hex16_t, 
						BITS("../%dram_init", 15, 0));
				FIELD("MrsBank", hex_t,
						BITS("../%dram_init", 18, 16));
				FIELD("SendPchgAll", send_t,
						BITS("../%dram_init", 24));
				FIELD("SendAutoRefresh", send_t,
						BITS("../%dram_init", 25));
				FIELD("SendMrsCmd", send_t,
						BITS("../%dram_init", 26));
				FIELD("DeassertMemRstX", yesno_t,
						BITS("../%dram_init", 27));
				FIELD("AssertCke", yesno_t,
						BITS("../%dram_init", 28));
				FIELD("EnDramInit", enabledisable_t,
						BITS("../%dram_init", 31));
			});
		}

		//
		// DRAM Timing Low/High registers
		//

		type busclocks_t = INT("bus clock(s)");

		REG32("%dram_timing_low", 0x88);
		REG32("%dram_timing_high", 0x8C);
		SCOPE("dram_timing", {
			// dram_timing_low
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("Tcl", ENUM([1, "cl_2"],
						       [2, "cl_3"],
						       [5, "cl_2.5"]),
						BITS("../%dram_timing_low", 2, 0));
				FIELD("Trc", XFORM(busclocks_t,
							LAMBDA(_1 + 7), LAMBDA(_1 - 7)),
						BITS("../%dram_timing_low", 7, 4));
				FIELD("Trfc", XFORM(busclocks_t,
							LAMBDA(_1 + 9), LAMBDA(_1 - 9)),
						BITS("../%dram_timing_low", 11, 8));
				FIELD("Trcd", busclocks_t,
						BITS("../%dram_timing_low", 14, 12));
				FIELD("Trrd", busclocks_t,
						BITS("../%dram_timing_low", 18, 16));
				FIELD("Tras", busclocks_t,
						BITS("../%dram_timing_low", 23, 20));
				FIELD("Trp", busclocks_t,
						BITS("../%dram_timing_low", 26, 24));
				FIELD("Twr", XFORM(busclocks_t,
							LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
						BITS("../%dram_timing_low", 28));
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("Tcl", XFORM(int_t,
							LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
						BITS("../%dram_timing_low", 2, 0));
				FIELD("Trcd", XFORM(busclocks_t,
							LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
						BITS("../%dram_timing_low", 5, 4));
				FIELD("Trp", XFORM(busclocks_t,
							LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
						BITS("../%dram_timing_low", 9, 8));
				// FIXME: depends on BurstLength32
				FIELD("Trtp", BOOL(
					"3_clocks_32_byte_burst_or_5_clocks_64_byte_burst",
					"2_clocks_32_byte_burst_or_4_clocks_64_byte_burst"),
						BITS("../%dram_timing_low", 11));
				FIELD("Tras", XFORM(busclocks_t,
							LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
						BITS("../%dram_timing_low", 15, 12));
				FIELD("Trc", XFORM(busclocks_t,
							LAMBDA(_1 + 11), LAMBDA(_1 - 11)),
						BITS("../%dram_timing_low", 19, 16));
				FIELD("Twr", XFORM(busclocks_t,
							LAMBDA(_1 + 3), LAMBDA(_1 - 3)),
						BITS("../%dram_timing_low", 21, 20));
				FIELD("Trrd", XFORM(busclocks_t,
							LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
						BITS("../%dram_timing_low", 23, 22));
				// FIXME: this could be better if we knew the package type
				FIELD("MemClkDis", BITMASK(
					[0, "am2_ma1_clk1"],
					[1, "am2_ma0_clk1_s1g1_ma0_clk1"],
					[2, "f1207_ma3_clk"],
					[3, "f1207_ma2_clk"],
					[4, "f1207_ma1_clk_am2_ma1_clk0"],
					[5, "f1207_ma0_clk_am2_ma0_clk0_s1g1_ma0_clk0"],
					[6, "am2_ma1_clk2"],
					[7, "am2_ma0_clk2_s1g1_ma0_clk2"]),
						BITS("../%dram_timing_low", 31, 24));
			}

			// dram_timing_high
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("Twtr", XFORM(busclocks_t,
							LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
						BITS("../%dram_timing_high", 0));
				FIELD("Trwt", XFORM(busclocks_t,
							LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
						BITS("../%dram_timing_high", 6, 4));
				FIELD("Tref", ENUM([0, "100MHz_15.6us"],
							[1, "133MHz_15.6us"],
							[2, "166MHz_15.6us"],
							[3, "200MHz_15.6us"],
							[8, "100MHz_7.8us"],
							[9, "133MHz_7.8us"],
							[10, "166MHz_7.8us"],
							[11, "200MHz_7.8us"],
							[16, "100MHz_3.9us"],
							[17, "133MHz_3.9us"],
							[18, "166MHz_3.9us"],
							[19, "200MHz_3.9us"]),
						BITS("../%dram_timing_high", 12, 8));
				FIELD("Twcl", XFORM(busclocks_t,
							LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
						BITS("../%dram_timing_high", 22, 20));
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("TrwtTO", XFORM(busclocks_t,
							LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
						BITS("../%dram_timing_high", 6, 4));
				FIELD("Twtr", busclocks_t,
						BITS("../%dram_timing_high", 9, 8));
				FIELD("Twrrd", busclocks_t,
						BITS("../%dram_timing_high", 11, 10));
				FIELD("Twrwr", XFORM(busclocks_t,
							LAMBDA(_1 + 1), LAMBDA(_1 - 1)),
						BITS("../%dram_timing_high", 13, 12));
				FIELD("Trdrd", XFORM(busclocks_t,
							LAMBDA(_1 + 2), LAMBDA(_1 - 2)),
						BITS("../%dram_timing_high", 15, 14));
				FIELD("Tref", ENUM([0, "undefined"],
							[2, "refresh_us_7_8"],
							[3, "refresh_us_3_9"]),
						BITS("../%dram_timing_high", 17, 16));
				FIELD("DisAutoRefresh", BOOL("disabled", "enabled"),
						BITS("../%dram_timing_high", 18));
				for (int i = 0; i < 4; i++) {
					FIELD("Trfc[]", ENUM([0, "ns_75"],
								 [1, "ns_105"],
								 [2, "ns_127"],
								 [3, "ns_195"],
								 [4, "ns_327"]),
						BITS("../%dram_timing_high", 3*i+22, 3*i+20));
				}
				type trfc_t = ENUM( [0, "ns_75"], [1, "ns_105"], [2, "ns_127"],
					       [3, "ns_195"], [4, "ns_327"]);
				FIELD("Trfc[]", trfc_t,
					BITS("../%dram_timing_high", 22, 20));
				FIELD("Trfc[]", trfc_t,
					BITS("../%dram_timing_high", 25, 23));
				FIELD("Trfc[]", trfc_t,
					BITS("../%dram_timing_high", 28, 26));
				FIELD("Trfc[]", trfc_t,
					BITS("../%dram_timing_high", 31, 29));
			}

		});

		//
		// DRAM Configuration Low/High registers
		//

		REG32("%dram_config_low", 0x90);
		REG32("%dram_config_high", 0x94);
		SCOPE("dram_config", {
			type ns_t = INT("ns");

			// dram_config_low
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("DLL_Dis", BOOL("disabled", "enabled"),
						BITS("../%dram_config_low", 0));
				FIELD("D_DRV", BOOL("weak_drive", "normal_drive"),
						BITS("../%dram_config_low", 1));
				FIELD("QFC_EN", enabledisable_t,
						BITS("../%dram_config_low", 2));
				FIELD("DisDqsHys", yesno_t, BITS("../%dram_config_low", 3));
				FIELD("Burst2Opt", yesno_t, BITS("../%dram_config_low", 5));
				FIELD("Mod64BitMux", yesno_t, BITS("../%dram_config_low", 6));
				// rev D and earlier
				// FIELD("ScratchBit", hex_t, BITS("../%dram_config_low", 7));
				FIELD("PwrDwnTriEn", enabledisable_t,
						BITS("../%dram_config_low", 7));
				FIELD("DramInit", yesno_t, BITS("../%dram_config_low", 8));
				FIELD("DualDimmEn", enabledisable_t,
						BITS("../%dram_config_low", 9));
				FIELD("DramEn", enabledisable_t,
						BITS("../%dram_config_low", 10));
				FIELD("MemClrStatus", BOOL("complete", "incomplete"),
						BITS("../%dram_config_low", 11));
				FIELD("ESR", yesno_t, BITS("../%dram_config_low", 12));
				FIELD("SR_S", BOOL("active", "normal"),
						BITS("../%dram_config_low", 13));
				FIELD("RdWrQByp", ENUM([0, "2"],
							    [1, "4"],
							    [2, "8"],
							    [3, "16"]),
						BITS("../%dram_config_low", 15, 14));
				FIELD("Width128", BOOL("bits_128", "bits_64"),
						BITS("../%dram_config_low", 16));
				FIELD("DimmEcEn", enabledisable_t,
						BITS("../%dram_config_low", 17));
				FIELD("UnBuffDimm", BOOL("unbuffered", "buffered"),
						BITS("../%dram_config_low", 18));
				FIELD("x4DIMMS[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 20));
				FIELD("x4DIMMS[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 21));
				FIELD("x4DIMMS[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 22));
				FIELD("x4DIMMS[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 23));
				FIELD("DisInRcvrs", BOOL("disabled", "enabled"),
						BITS("../%dram_config_low", 24));
				FIELD("BypMax", int_t, BITS("../%dram_config_low", 27, 25));
				FIELD("En2T", yesno_t, BITS("../%dram_config_low", 28));
				FIELD("UpperCSMap", yesno_t,
						BITS("../%dram_config_low", 29));
				// PowerDownCtl is different in rev. D
				FIELD("PwrDownCtl", ENUM([0, "power_down_disabled"],
							      [1, "alternating_DIMM_clock"],
							      [2, "same_DIMM_clock"],
							      [3, "independent_DIMM_clock"]),
						BITS("../%dram_config_low", 31, 30));
				if (EVAL("Burst2Opt") == 1
				 && EVAL("UnBuffDimm") == 1
				 && EVAL("En2T") == 0) {
					FIELD("Bytes32En", ENUM([0, "8beat_64byte"],
								     [1, "4beat_64byte"],
								     [2, "4beat_32byte"],
								     [3, "2beat_32byte"]),
							BITS("../%dram_config_low", 19) +
							BITS("../%dram_config_low", 16));
				} else {
					FIELD("Bytes32En", ENUM([0, "8beat_64byte"],
								     [1, "4beat_64byte"],
								     [2, "4beat_32byte"],
					// FIXME: what to do about duplicate enum keys?
								     [3, "_4beat_64byte"]),
							BITS("../%dram_config_low", 19) +
							BITS("../%dram_config_low", 16));
				}
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("InitDram", yesno_t, BITS("../%dram_config_low", 0));
				FIELD("ExitSelfRef", yesno_t, BITS("../%dram_config_low", 1));
				FIELD("DramTerm", ENUM([0, "disabled"],
							    [1, "ohms_75"],
							    [2, "ohms_150"],
							    [3, "ohms_50"]),
						BITS("../%dram_config_low", 5, 4));
				FIELD("DramDrvWeak", BOOL("weak_mode", "normal_mode"),
						BITS("../%dram_config_low", 7));
				FIELD("ParEn", enabledisable_t,
						BITS("../%dram_config_low", 8));
				FIELD("SelfRefRateEn", enabledisable_t,
						BITS("../%dram_config_low", 9));
				FIELD("BurstLength32", BOOL("bytes_32", "bytes_64"),
						BITS("../%dram_config_low", 10));
				FIELD("Width128", BOOL("bits_128", "bits_64"),
						BITS("../%dram_config_low", 11));
				FIELD("x4Dimm[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 12));
				FIELD("x4Dimm[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 13));
				FIELD("x4Dimm[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 14));
				FIELD("x4Dimm[]", BOOL("x4_dimm", "not_x4"),
						BITS("../%dram_config_low", 15));
				FIELD("UnBuffDimm", BOOL("unbuffered", "buffered"),
						BITS("../%dram_config_low", 16));
				FIELD("DimmEccEn", enabledisable_t,
						BITS("../%dram_config_low", 19));
			}

			// dram_config_high
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("AsyncLat", ns_t, BITS("../%dram_config_high", 3, 0));
				FIELD("RdPreamble", XFORM(FIXED(1, "ns" ),
							LAMBDA(_1+4), LAMBDA(_1-4)),
						BITS("../%dram_config_high", 11, 8));
				FIELD("MemDQDrvStren", ENUM(
							[0, "reduction_none"],
							 [1, "reduction_15_percent"],
							 [2, "reduction_30_percent"],
							 [3, "reduction_50_percent"]),
						BITS("../%dram_config_high", 14, 13));
				FIELD("DisableJitter", yesno_t,
						BITS("../%dram_config_high", 15));
				FIELD("ILD_lmt", ENUM([0, "cycles_0"],
							   [1, "cycles_4"],
							   [2, "cycles_8"],
							   [3, "cycles_16"],
							   [4, "cycles_32"],
							   [5, "cycles_64"],
							   [6, "cycles_128"],
							   [7, "cycles_256"]),
						BITS("../%dram_config_high", 18, 16));
				FIELD("DCC_EN", enabledisable_t,
						BITS("../%dram_config_high", 19));
				FIELD("MemClk", ENUM([0, "MHz_100"],
							  [2, "MHz_133"],
							  [5, "MHz_166"],
							  [7, "MHz_200"]),
						BITS("../%dram_config_high", 22, 20));
				FIELD("MCR", enabledisable_t,
						BITS("../%dram_config_high", 25));
				FIELD("MC_EN[]", enabledisable_t,
						BITS("../%dram_config_high", 26));
				FIELD("MC_EN[]", enabledisable_t,
						BITS("../%dram_config_high", 27));
				FIELD("MC_EN[]", enabledisable_t,
						BITS("../%dram_config_high", 28));
				FIELD("MC_EN[]", enabledisable_t,
						BITS("../%dram_config_high", 29));
				FIELD("OddDivisorCorrect", enabledisable_t,
						BITS("../%dram_config_high", 31));
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("MemClkFreq", ENUM([0, "MHz_200"],
							      [1, "MHz_266"],
							      [2, "MHz_333"],
							      [3, "MHz_400"]),
						BITS("../%dram_config_high", 2, 0));
				FIELD("MemClkFreqVal", yesno_t, 
						BITS("../%dram_config_high", 3));
				FIELD("MaxAsyncLat", ns_t,
						BITS("../%dram_config_high", 7, 4));
				FIELD("RDqsEn", BOOL("read_DQS_pins", "data_mask_pins"),
						BITS("../%dram_config_high", 12));
				FIELD("DisDramInterface", BOOL("disabled", "enabled"),
						BITS("../%dram_config_high", 14));
				FIELD("PowerDownEn", enabledisable_t,
						BITS("../%dram_config_high", 15));
				FIELD("PowerDownMode", BOOL("chip_select_cke_control",
								 "channel_cke_control"),
						BITS("../%dram_config_high", 16));
				FIELD("FourRankSODimm", truefalse_t,
						BITS("../%dram_config_high", 17));
				FIELD("FourRankRDimm", truefalse_t,
						BITS("../%dram_config_high", 18));
				FIELD("SlowAccessMode", enabledisable_t,
						BITS("../%dram_config_high", 20));
				FIELD("BankSwizzleMode", enabledisable_t,
						BITS("../%dram_config_high", 22));
				FIELD("DcqBypassMax", INT("bypass(es)"),
						BITS("../%dram_config_high", 27, 24));
				FIELD("FourActWindow", ENUM(
							[0, "no_tFAW_window_restriction"],
							[1, "MEMCLK_cycles_8"],
							[2, "MEMCLK_cycles_9"],
							[3, "MEMCLK_cycles_10"], 
							[4, "MEMCLK_cycles_11"],
							[5, "MEMCLK_cycles_12"],
							[6, "MEMCLK_cycles_13"],
							[7, "MEMCLK_cycles_14"],
							[8, "MEMCLK_cycles_15"],
							[9, "MEMCLK_cycles_16"],
							[10, "MEMCLK_cycles_17"],
							[11, "MEMCLK_cycles_18"],
							[12, "MEMCLK_cycles_19"],
							[13, "MEMCLK_cycles_20"]),
						BITS("../%dram_config_high", 31, 28));
			}
		}); // dram_config

		//
		// DRAM Bank Address Mapping register
		//
		REG32("%bank_address_mapping", 0x80);
		SCOPE("bank_address_mapping", {
			if (EVAL("$k8/k8_rev") == "rev_e") {
				type cs_int = ENUM(
					[0, "MB_32_12x8"],
					[1, "MB_64_12x9"],
					[2, "MB_128_13x9"],
					[3, "MB_128_12x10"],
					[4, "MB_256_13x10"],
					[5, "MB_512_14x10"],
					[6, "MB_256_12x11"],
					[7, "MB_512_13x11"],
					[8, "MB_1024_14x11"],
					[9, "MB_1024_13x12"],
					[10, "MB_2048_14x12"]);
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				if (EVAL("../dram_config/Width128") == "bits_64") {
					type cs_int = ENUM(
						[0, "MB_128_13x9x2"],
						[1, "MB_256_13x10x2"],
						[2, "MB_512_14x10x2"],
						[3, "MB_512_13x11x2"],
						[4, "MB_512_13x10x3"],
						[5, "MB_1024_14x10x3"],
						[6, "MB_1024_14x11x2"],
						[7, "MB_2048_15x10x3"],
						[8, "MB_2048_14x11x3"],
						[9, "MB_4096_15x11x3"],
						[10, "MB_4096_16x10x3"],
						[11, "MB_8192_16x11x3"]);
				} else { // Width128 == bits_128
					type cs_int = ENUM(
						[0, "MB_256_13x9x2"],
						[1, "MB_512_13x10x2"],
						[2, "MB_1024_14x10x2"],
						[3, "MB_1024_13x11x2"],
						[4, "MB_1024_13x10x3"],
						[5, "MB_2048_14x10x3"],
						[6, "MB_2048_14x11x2"],
						[7, "MB_4096_15x10x3"],
						[8, "MB_4096_14x11x3"],
						[9, "MB_8192_15x11x3"],
						[10, "MB_8192_16x10x3"],
						[11, "MB_16384_16x11x3"]);
				}
			}

			// these could be "disabled" or hidden if cs[x].CSEnable == false
			FIELD("CS1_0", "cs_int", BITS("../%bank_address_mapping", 3, 0));
			FIELD("CS3_2", "cs_int", BITS("../%bank_address_mapping", 7, 4));
			FIELD("CS5_4", "cs_int", BITS("../%bank_address_mapping", 11, 8));
			FIELD("CS7_6", "cs_int", BITS("../%bank_address_mapping", 15, 12));
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("BankSwizzleMode", enabledisable_t,
						BITS("../%bank_address_mapping", 30));
			}

		});

		if (EVAL("$k8/k8_rev") == "rev_e") {
			//
			// DRAM Delay Line register
			//

			REG32("%dram_delay_line", 0x98);
			SCOPE("dram_delay_line", {
				FIELD("Adj", int_t, BITS("../%dram_delay_line", 23, 16));
				FIELD("AdjSlower", yesno_t, BITS("../%dram_delay_line", 24));
				FIELD("AdjFaster", yesno_t, BITS("../%dram_delay_line", 25));
				FIELD("AltVidC3MemClkTriEn", enabledisable_t,
						BITS("../%dram_delay_line", 27));
				FIELD("CompPwrSaveEn", enabledisable_t,
						BITS("../%dram_delay_line", 28));
				FIELD("DllSpeed", ENUM([0, "speed_low"],
							    [1, "speed_high"]),
						BITS("../%dram_delay_line", 29));
				FIELD("DllSpeedOverride", yesno_t,
						BITS("../%dram_delay_line", 30));
			});

			//
			// Scratch Register
			//
			FIELD("ScratchData", hex32_t, REG32(0x9C));
		} else if (EVAL("$k8/k8_rev") == "rev_f") {
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
			SCOPE("dram_misc", {
				FIELD("DisableJitter", yesno_t,
						BITS("../%dram_controller_misc", 1));
				FIELD("RdWrQByp", ENUM([0, "2"], [1, "4"],
							    [2, "8"], [3, "16"]),
						BITS("../%dram_controller_misc", 3, 2));
				FIELD("Mod64BitMux", enabledisable_t,
						BITS("../%dram_controller_misc", 4));
				FIELD("DCC_EN", enabledisable_t,
						BITS("../%dram_controller_misc", 5));
				FIELD("ILD_lmt", ENUM([0, "cycles_0"],
							   [1, "cycles_4"],
							   [2, "cycles_8"],
							   [3, "cycles_16"],
							   [4, "cycles_32"],
							   [5, "cycles_64"],
							   [6, "cycles_128"],
							   [7, "cycles_256"]),
						BITS("../%dram_controller_misc", 8, 6));
				FIELD("DramEnabled", enabledisable_t,
						BITS("../%dram_controller_misc", 9));
				FIELD("PwrSavingsEn", enabledisable_t,
						BITS("../%dram_controller_misc", 10));
				// FIXME: this could be better if we knew the package type
				FIELD("MemClkDis", BITMASK(
					[0, "am2_mb1_clk1"],
					[1, "am2_mb0_clk1_s1g1_mb0_clk1"],
					[2, "f1207_mb3_clk"],
					[3, "f1207_mb2_clk"],
					[4, "f1207_mb1_clk_am2_mb1_clk0"],
					[5, "f1207_mb0_clk_am2_mb0_clk0_s1g1_mb0_clk0"],
					[6, "am2_mb1_clk2"],
					[7, "am2_mb0_clk2_s1g1_mb0_clk2"]),
						BITS("../%dram_controller_misc", 31, 24));
			}); // dram_misc
		}
	}); // dram_ctrl
}

k8_misc_control(int seg, int bus, int dev, int fun)
{
	PCI_SCOPE("misc_ctrl", seg, bus, dev, fun+3, {
		SCOPE("mca_northbridge", {
			//
			// MCA Northbridge Control/Configuration registers
			//
			REG32("%control", 0x40);
			REG32("%config", 0x44);
			SCOPE("control_config", {
				// control
				FIELD("CorrEccEn", enabledisable_t, BITS("../%control", 0));
				FIELD("UnCorrEccEn", enabledisable_t, BITS("../%control", 1));
				FIELD("CrcErr0En", enabledisable_t, BITS("../%control", 2));
				FIELD("CrcErr1En", enabledisable_t, BITS("../%control", 3));
				FIELD("CrcErr2En", enabledisable_t, BITS("../%control", 4));
				FIELD("SyncPkt0En", enabledisable_t, BITS("../%control", 5));
				FIELD("SyncPkt1En", enabledisable_t, BITS("../%control", 6));
				FIELD("SyncPkt2En", enabledisable_t, BITS("../%control", 7));
				FIELD("MstrAbrtEn", enabledisable_t, BITS("../%control", 8));
				FIELD("TgtAbrtEn", enabledisable_t, BITS("../%control", 9));
				FIELD("GartTblWkEn", enabledisable_t, BITS("../%control", 10));
				FIELD("AtomicRMWEn", enabledisable_t, BITS("../%control", 11));
				FIELD("WchDogTmrEn", enabledisable_t, BITS("../%control", 12));
				if (EVAL("$k8/k8_rev") == "rev_f") {
					FIELD("DramParEn", enabledisable_t,
							BITS("../%control", 18));
				}

				// config
				FIELD("CpuEccErrEn", enabledisable_t, BITS("../%config", 0));
				FIELD("CpuRdDatErrEn", enabledisable_t, BITS("../%config", 1));
				FIELD("SyncOnUcEccEn", enabledisable_t, BITS("../%config", 2));
				FIELD("SyncPktGenDis", disableenable_t, BITS("../%config", 3));
				FIELD("SyncPktPropDis", disableenable_t, BITS("../%config", 4));
				FIELD("IoMstAbortDis", disableenable_t, BITS("../%config", 5));
				FIELD("CpuErrDis", disableenable_t, BITS("../%config", 6));
				FIELD("IoErrDis", disableenable_t, BITS("../%config", 7));
				FIELD("WdogTmrDis", disableenable_t, BITS("../%config", 8));
				FIELD("WdogTmrCntSel", ENUM([0, "4095"],
								 [1, "2047"],
								 [2, "1023"],
								 [3, "511"],
								 [4, "255"],
								 [5, "127"],
								 [6, "63"],
								 [7, "31"]),
						BITS("../%config", 11, 9));
				FIELD("WdogTmrBaseSel", ENUM([0, "ms_1"],
								  [1, "us_1"],
								  [2, "ns_5"]),
						BITS("../%config", 13, 12));
				FIELD("LdtLinkSel", ENUM([0, "ht_link_0"],
							      [1, "ht_link_1"],
							      [2, "ht_link_2"]),
						BITS("../%config", 15, 14));
				FIELD("GenCrcErrByte0", BOOL("generating", "not_generating"),
						BITS("../%config", 16));
				FIELD("GenCrcErrByte1", BOOL("generating", "not_generating"),
						BITS("../%config", 17));
				FIELD("SyncOnWdogEn", enabledisable_t, BITS("../%config", 20));
				FIELD("SyncOnAnyErrEn", enabledisable_t, BITS("../%config", 21));
				FIELD("EccEn", enabledisable_t, BITS("../%config", 22));
				FIELD("ChipKillEccEn", enabledisable_t, BITS("../%config", 23));
				FIELD("IoRdDatErrEn", enabledisable_t, BITS("../%config", 24));
				FIELD("DisPciCfgCpuErrRsp", yesno_t, BITS("../%config", 25));
				if (EVAL("$k8/k8_rev") == "rev_e") {
					FIELD("NbMcaToMstExcEn", enabledisable_t,
							BITS("../%config", 27));
				} else if (EVAL("$k8/k8_rev") == "rev_f") {
					FIELD("NbMcaToMstCpuEn", enabledisable_t,
							BITS("../%config", 27));
				}
				if (EVAL("$k8/k8_rev") == "rev_f") {
					FIELD("DisTgtAbtCpuErrRsp", yesno_t,
							BITS("../%config", 28));
					FIELD("DisMstAbtCpuErrRsp", yesno_t,
							BITS("../%config", 29));
					FIELD("SyncOnDramAdrParErrEn", enabledisable_t,
							BITS("../%config", 30));
				}
			}); // control_config

			//
			// MCA Northbridge Status registers
			//
			REG32("%status_low", 0x48);
			REG32("%status_high", 0x4C);
			SCOPE("status", {
				// Error code types
				type cachelevel_t = ENUM(
						[0, "level_0"],
						[1, "level_1"],
						[2, "level_2"],
						[3, "generic"]);
				type memorio_t = ENUM(
						[0, "memory_access"],
						[2, "io_access"],
						[3, "generic"]);
				type mtt_t = ENUM(
						[0, "generic_error"],
						[1, "generic_read"],
						[2, "generic_write"],
						[3, "data_read"],
						[4, "data_write"],
						[5, "instruction_fetch"],
						[6, "prefetch"],
						[7, "evict"],
						[8, "snoop"]);
				type participationproc_t = ENUM(
						[0, "local_node_originated"],
						[1, "local_node_responded"],
						[2, "local_node_observed"],
						[3, "generic"]);
				type transtype_t = ENUM(
						[0, "instruction"],
						[1, "data"],
						[2, "generic"]);

				FIELD("ErrorCode", hex16_t, BITS("../%status_low", 15, 0));
				if (EVAL("ErrorCode") & 0x0800) {
					// Bus error
					FIELD("CacheLevel", cachelevel_t,
							BITS("../%status_low", 1, 0));
					FIELD("MemoryOrIO", memorio_t,
							BITS("../%status_low", 3, 2));
					FIELD("MemoryTransactionType", mtt_t,
							BITS("../%status_low", 7, 4));
					FIELD("Timeout", yesno_t,
							BITS("../%status_low", 8));
					FIELD("ParticipationProcessor", participationproc_t,
							BITS("../%status_low", 10, 9));
					FIELD("ErrorCodeExt", ENUM([0, "ecc_error"],
									[1, "crc_error"],
									[2, "sync_error"],
									[3, "mst_abort"],
									[4, "tgt_abort"],
									[6, "rmw_error"],
									[7, "wdog_error"],
									[8, "chipkill_ecc_error"],
									[9, "dev_error"],
									[13, "dram_parity_error"]),
							BITS("../%status_low", 19, 16));
					if (EVAL("ErrorCodeExt") == "chipkill_ecc_error") {
						FIELD("Syndrome", hex8_t,
								BITS("../%status_low", 31, 24));
					}
				} else if (EVAL("ErrorCode") & 0x0100) {
					// Memory error
					FIELD("CacheLevel", cachelevel_t,
							BITS("../%status_low", 1, 0));
					FIELD("TransactionType", transtype_t,
							BITS("../%status_low", 3, 2));
					FIELD("MemoryTransactionType", mtt_t,
							BITS("../%status_low", 7, 4));
				} else if (EVAL("ErrorCode") & 0x0010) {
					// TLB error
					FIELD("CacheLevel", cachelevel_t,
							BITS("../%status_low", 1, 0));
					FIELD("TransactionType", transtype_t,
							BITS("../%status_low", 3, 2));
					FIELD("ErrorCodeExt", ENUM([5, "gart_error"]),
							BITS("../%status_low", 19, 16));
				}

				FIELD("ErrCPU[]", yesno_t, BITS("../%status_high", 0));
				FIELD("ErrCPU[]", yesno_t, BITS("../%status_high", 1));
				FIELD("LDTLink[]", BOOL("associated", "not_associated"),
						BITS("../%status_high", 4));
				FIELD("LDTLink[]", BOOL("associated", "not_associated"),
						BITS("../%status_high", 5));
				FIELD("LDTLink[]", BOOL("associated", "not_associated"),
						BITS("../%status_high", 6));
				FIELD("ErrScrub", yesno_t, BITS("../%status_high", 8));
				if (EVAL("$k8/k8_rev") == "rev_f") {
					FIELD("DramChannel", ENUM([0, "A"], [1, "B"]),
							BITS("../%status_high", 9));
				}
				FIELD("UnCorrECC", yesno_t, BITS("../%status_high", 13));
				FIELD("CorrECC", yesno_t, BITS("../%status_high", 14));
				FIELD("ECC_Synd", hex8_t, BITS("../%status_high", 22, 15));
				FIELD("PCC", yesno_t, BITS("../%status_high", 25));
				FIELD("ErrAddrVal", BOOL("valid", "invalid"),
						BITS("../%status_high", 26));
				FIELD("ErrMiscVal", yesno_t, BITS("../%status_high", 27));
				FIELD("ErrEn", enabledisable_t, BITS("../%status_high", 28));
				FIELD("ErrUnCorr", yesno_t, BITS("../%status_high", 29));
				FIELD("ErrOver", yesno_t, BITS("../%status_high", 30));
				FIELD("ErrValid", yesno_t, BITS("../%status_high", 31));
			});

			//
			// MCA Northbridge Address registers
			//
			REG32("%address_low", 0x50);
			REG32("%address_high", 0x54);

			if (DEFINED("ErrorCodeExt")
			 && EVAL("ErrorCodeExt") == "wdog_error") {
				SCOPE("wdog_error", {
					FIELD("LdtCmd", hex_t, BITS("../%address_low", 8, 3));
					FIELD("OpType", ENUM([0, "normal_op"],
								  [1, "bus_lock"],
								  [2, "local_apic_access"],
								  [3, "interrupt_request"],
								  [4, "system_mgmt_request"],
								  [5, "interrupt_broadcast"],
								  [6, "stop_grant"],
								  [7, "smi_ack"]),
							BITS("../%address_low", 11, 9));
					FIELD("NextAction", 
						ENUM([0, "complete"],
							  [2, "send_request"],
							  [3, "send_second_request"],
							  [4, "send_final_response_back"],
							  [5, "send_initial_response_back"],
							  [6, "send_final_response_to_mem"],
							  [7, "send_initial_response_to_mem"]),
						BITS("../%address_low", 14, 12));
					FIELD("SrcPtr", ENUM([0, "host_bridge_local_node"],
								  [1, "cpu_local_node"],
								  [2, "mem_controller_local_node"],
								  [5, "ht_link_0"],
								  [6, "ht_link_1"],
								  [7, "ht_link_2"]),
							BITS("../%address_low", 17, 15));
					FIELD("SrcUnit", int_t, BITS("../%address_low", 19, 18));
					FIELD("SrcNode", int_t, BITS("../%address_low", 22, 20));
					FIELD("DstUnit", int_t, BITS("../%address_low", 24, 23));
					FIELD("DstNode", int_t, BITS("../%address_low", 27, 25));
					FIELD("WaitPW", yesno_t, BITS("../%address_low", 29));
					if (EVAL("$k8/k8_rev") == "rev_e") {
						FIELD("WaitCode", bitfield_t,
								BITS("../%address_high", 2, 0) +
								BITS("../%address_low", 31, 30));
					} else if (EVAL("$k8/k8_rev") == "rev_f") {
						FIELD("WaitCode", ENUM(
							[0, "no_wait_cond"],
							KV("xbar_wait_for_probe_response_"
							   "from_cpu_0", 4),
							KV("xbar_wait_for_probe_response_"
							   "from_cpu_1", 5),
							[8, "xbar_wait_because_of_gart_or_dev_miss"],
							KV("xbar_wait_for_gart_table_walk_"
							   "state_machine_to_free", 9),
							[10, "xbar_or_cpu_wait_for_bus_to_unlock"],
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
					FIELD("RspCnt", INT("responses pending"),
							BITS("../%address_high", 6, 3));
					FIELD("GartTblWkInProg", yesno_t,
							BITS("../%address_high", 7));
				});
			} else {
				FIELD("ErrAddr", addr64_t, 
						BITS("%address_high", 7, 0) +
						BITS("%address_low", 31, 3) +
						BITS("%0", 2, 0));
			}
		}); // mca_northbridge

		//
		// Scrub Control register
		//
		SCOPE("scrubbers", {
			REG32("%scrub_control", 0x58);
			type scrub_rate_t = ENUM( [0, "off"],
					     [1, "ns_40"],
					     [2, "ns_80"],
					     [3, "ns_160"],
					     [4, "ns_320"],
					     [5, "ns_640"],
					     [6, "us_1_28"],
					     [7, "us_2_56"],
					     [8, "us_5_12"],
					     [9, "us_10_2"],
					     [10, "us_20_5"],
					     [11, "us_41_0"],
					     [12, "us_81_9"],
					     [13, "us_163_8"],
					     [14, "us_327_7"],
					     [15, "us_655_4"],
					     [16, "ms_1_31"],
					     [17, "ms_2_62"],
					     [18, "ms_5_24"],
					     [19, "ms_10_49"],
					     [20, "ms_20_97"],
					     [21, "ms_42_00"],
					     [22, "ms_84_00"]);

			FIELD("DramScrub", scrub_rate_t, BITS("%scrub_control", 4, 0));
			FIELD("L2Scrub", scrub_rate_t, BITS("%scrub_control", 12, 8));
			FIELD("DcacheScrub", scrub_rate_t, BITS("%scrub_control", 20, 16));

			//
			// DRAM Scrub Address registers
			//
			REG32("%dram_scrub_address_low", 0x5C);
			REG32("%dram_scrub_address_high", 0x60);

			FIELD("ScrubReDirEn", yesno_t, BITS("%dram_scrub_address_low", 0));
			FIELD("ScrubAddr", addr64_t,
					BITS("%dram_scrub_address_high", 7, 0) +
					BITS("%dram_scrub_address_low", 31, 6) +
					BITS("%0", 5, 0));
		}); // scrubbers

		if (EVAL("$k8/k8_rev") == "rev_f") {
			//
			// HTC register
			//
			REG32("%htc", 0x64);
			SCOPE("htc", {
				FIELD("HtcEn", enabledisable_t, BITS("../%htc", 0));
				FIELD("HtcSbcEn", enabledisable_t, BITS("../%htc", 1));
				FIELD("HtcAct", yesno_t, BITS("../%htc", 4));
				FIELD("HtcActSts", yesno_t, BITS("../%htc", 5));
			});

			//
			// Thermal Control register
			//
			REG32("%ThermalControl", 0x68);
			SCOPE("thermal_control", {
				FIELD("StcSbcTmpHiEn", enabledisable_t,
						BITS("../%ThermalControl", 0));
				FIELD("StcSbcTmpLoEn", enabledisable_t, 
						BITS("../%ThermalControl", 1));
				FIELD("StcApcTmpHiEn", enabledisable_t,
						BITS("../%ThermalControl", 2));
				FIELD("StcApcTmpLoEn", enabledisable_t,
						BITS("../%ThermalControl", 3));
				FIELD("StcTmpHiSts", yesno_t,
						BITS("../%ThermalControl", 6));
				FIELD("StcTmpLoSts", yesno_t,
						BITS("../%ThermalControl", 7));
				FIELD("StcTmpLmt", XFORM(INT("C"),
							LAMBDA(52 + (_1 * 2)),
							LAMBDA((_1 - 52) / 2)),
						BITS("../%ThermalControl", 20, 16));
				FIELD("StcHystLmt", XFORM(INT("C"),
							LAMBDA(2 + (_1 * 2)),
							LAMBDA((_1 - 2) / 2)),
						BITS("../%ThermalControl", 27, 24));
			});
		}

		SCOPE("buffer_counts", {
			//
			// SRI-to-XBAR Buffer Counts register
			//
			REG32("%sri_to_xbar", 0x70);
			SCOPE("sri_to_xbar", {
				FIELD("UReq", int_t, BITS("../%sri_to_xbar", 1, 0));
				FIELD("UPReq", int_t, BITS("../%sri_to_xbar", 5, 4));
				FIELD("URsp", int_t, BITS("../%sri_to_xbar", 9, 8));
				FIELD("ReqD", int_t, BITS("../%sri_to_xbar", 17, 16));
				FIELD("DispRefReq", int_t, BITS("../%sri_to_xbar", 21, 20));
				FIELD("URspD", int_t, BITS("../%sri_to_xbar", 25, 24));
				FIELD("DReq", int_t, BITS("../%sri_to_xbar", 29, 28));
				FIELD("DPReq", int_t, BITS("../%sri_to_xbar", 31, 30));
			});

			//
			// XBAR-to-SRI Buffer Counts register
			//
			REG32("%xbar_to_sri", 0x74);
			SCOPE("xbar_to_sri", {
				FIELD("UReq", int_t, BITS("../%xbar_to_sri", 2, 0));
				FIELD("UPReq", int_t, BITS("../%xbar_to_sri", 6, 4));
				FIELD("Prb", int_t, BITS("../%xbar_to_sri", 15, 12));
				FIELD("DispRefReq", int_t, BITS("../%xbar_to_sri", 22, 20));
				FIELD("DReq", int_t, BITS("../%xbar_to_sri", 29, 28));
				FIELD("DPReq", int_t, BITS("../%xbar_to_sri", 31, 30));
			});

			//
			// MCT-to-XBAR Buffer Counts register
			//
			REG32("%mct_to_xbar", 0x78);
			SCOPE("mct_to_xbar", {
				FIELD("Rsp", int_t, BITS("../%mct_to_xbar", 11, 8));
				FIELD("Prb", int_t, BITS("../%mct_to_xbar", 14, 12));
				FIELD("RspD", int_t, BITS("../%mct_to_xbar", 27, 24));
			});

			//
			// Free List Buffer Counts register
			//
			REG32("%free_list", 0x7C);
			SCOPE("free_list", {
				FIELD("FreeCmd", int_t, BITS("../%free_list", 3, 0));
				FIELD("FReq", int_t, BITS("../%free_list", 5, 4));
				FIELD("FRsp", int_t, BITS("../%free_list", 10, 8));
				FIELD("FRspD", int_t, BITS("../%free_list", 13, 12));
			});
		}); // buffer_counts

		//
		// Power Management Control registers
		//
		REG32("%power_mgmt_control_low", 0x80);
		REG32("%power_mgmt_control_high", 0x84);

		type clksel_t = ENUM( [0, "divide_by_8"],
				 [1, "divide_by_16"],
				 [2, "divide_by_32"],
				 [3, "divide_by_64"],
				 [4, "divide_by_128"],
				 [5, "divide_by_256"],
				 [6, "divide_by_512"]);

		// Low
		for (int i = 0; i < 32; i += 8) {
			SCOPE("PMM[]", {
				FIELD("CPULowPrEn", enabledisable_t,
						BITS("../%power_mgmt_control_low", 0+i));
				FIELD("NBLowPwrEn", enabledisable_t,
						BITS("../%power_mgmt_control_low", 1+i));
				FIELD("FidVidEn", enabledisable_t,
						BITS("../%power_mgmt_control_low", 2+i));
				FIELD("AltVidEn", enabledisable_t,
						BITS("../%power_mgmt_control_low", 3+i));
				FIELD("ClkSel", clksel_t,
						BITS("../%power_mgmt_control_low", 6+i, 4+i));
			});
		}

		// High
		for (int i = 0; i < 32; i += 8) {
			SCOPE("PMM[]", {
				FIELD("CPULowPrEn", enabledisable_t,
						BITS("../%power_mgmt_control_high", 0+i));
				FIELD("NBLowPwrEn", enabledisable_t,
						BITS("../%power_mgmt_control_high", 1+i));
				FIELD("FidVidEn", enabledisable_t,
						BITS("../%power_mgmt_control_high", 2+i));
				FIELD("AltVidEn", enabledisable_t,
						BITS("../%power_mgmt_control_high", 3+i));
				FIELD("ClkSel", clksel_t,
						BITS("../%power_mgmt_control_high", 6+i, 4+i));
			});
		}

		SCOPE("gart", {
			//
			// GART Aperture Control register
			//
			REG32("%aperture_control", 0x90);
			SCOPE("aperture_control", {
				FIELD("GartEn", enabledisable_t, BITS("../%aperture_control", 0));
				FIELD("GartSize", ENUM([0, "MB_32"],
							    [1, "MB_64"],
							    [2, "MB_128"],
							    [3, "MB_256"],
							    [4, "MB_512"],
							    [5, "MB_1024"],
							    [6, "MB_2048"]),
						BITS("../%aperture_control", 3, 1));
				FIELD("DisGartCpu", yesno_t, BITS("../%aperture_control", 4));
				FIELD("DisGartIo", yesno_t, BITS("../%aperture_control", 5));
				FIELD("DisGartTblWlkPrb", yesno_t, BITS("../%aperture_control", 6));
			});

			//
			// GART Aperture Base register
			//
			REG32("%aperture_base", 0x94);
			FIELD("GartAperBaseAddr", addr64_t,
					BITS("%aperture_base", 14, 0) +
					BITS("%0", 24, 0));

			//
			// GART Table Base register
			//
			REG32("%table_base", 0x98);
			FIELD("GartTblBaseAddr", addr64_t,
					BITS("%table_base", 31, 4) +
					BITS("%0", 11, 0));

			//
			// GART Cache Control register
			//
			REG32("%cache_control", 0x9C);
			SCOPE("cache_control", {
				FIELD("InvGart", BOOL("invalidating", "not_invalidating"),
						BITS("../%cache_control", 0));
				FIELD("GartPteErr", yesno_t, BITS("../%cache_control", 1));
			});
		}); // gart

		if (EVAL("$k8/k8_rev") == "rev_f") {
			//
			// Power Control Miscellaneous register
			//
			REG32("%power_control_misc", 0xA0);
			SCOPE("power_control_misc", {
				FIELD("PsiVid", hex_t, BITS("../%power_control_misc", 5, 0));
				FIELD("PsiVidEn", enabledisable_t,
						BITS("../%power_control_misc", 7));
			});

			//
			// On-Line Spare Control register
			//
			REG32("%on_line_spare_control", 0xB0);
			SCOPE("on_line_spare_control", {
				FIELD("SwapEn", yesno_t,
						BITS("../%on_line_spare_control", 0));
				FIELD("SwapDone", yesno_t,
						BITS("../%on_line_spare_control", 1));
				FIELD("BadDramCs", int_t,
						BITS("../%on_line_spare_control", 6, 4));
				FIELD("SwapDoneInt", ENUM([0, "none"],
							       [2, "smi"]),
						BITS("../%on_line_spare_control", 13, 12));
				FIELD("EccErrInt", ENUM([0, "none"],
							     [2, "smi"]),
						BITS("../%on_line_spare_control", 15, 14));
				FIELD("EccErrCntDramCs", int_t,
						BITS("../%on_line_spare_control", 18, 16));
				FIELD("EccErrCntDramChan", int_t, 
						BITS("../%on_line_spare_control", 20));
				FIELD("EccErrCntWrEn", enabledisable_t,
						BITS("../%on_line_spare_control", 23));
				FIELD("EccErrCnt", int_t,
						BITS("../%on_line_spare_control", 27, 24));
			});
		}

		//
		// Clock Power/Timing registers
		//
		REG32("%clock_power_timing_low", 0xD4);
		REG32("%clock_power_timing_high", 0xD8);
		SCOPE("clock_power_timing", {
			FIELD("GPE", ENUM([1, "sys_clocks_200"],
					       [2, "sys_clocks_400"],
					       [3, "sys_clocks_600"],
					       [4, "sys_clocks_800"],
					       [5, "sys_clocks_1600"],
					       [6, "sys_clocks_3000"],
					       [7, "sys_clocks_20000"]),
					BITS("../%clock_power_timing_low", 2, 0));
			FIELD("ClkRampHyst", ENUM([0, "ns_0"],
						       [1, "ns_125"],
						       [2, "ns_250"],
						       [3, "ns_375"],
						       [4, "ns_500"],
						       [5, "ns_750"],
						       [6, "ns_1000"],
						       [7, "ns_2000"],
						       [8, "us_4"],
						       [9, "us_8"],
						       [10, "us_16"],
						       [11, "us_32"]),
					BITS("../%clock_power_timing_low", 11, 8));
			FIELD("ReConDel", INT("us"),
					BITS("../%clock_power_timing_low", 15, 12));
			FIELD("LClkPLLLock", INT("system bus clocks to wait"),
					BITS("../%clock_power_timing_low", 31, 16) +
					BITS("%0", 3, 0));
			FIELD("VSTime", INT("system clock cycles"),
					BITS("../%clock_power_timing_high", 19, 0));
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("AltVid", hex_t,
						BITS("../%clock_power_timing_high", 24, 20));
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("AltVid", hex_t,
						BITS("../%clock_power_timing_high", 25, 20));
			}
			if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("AltVidTriEn", enabledisable_t,
						BITS("../%clock_power_timing_high", 26));
				FIELD("GfxMode", BOOL("uma", "discrete"),
						BITS("../%clock_power_timing_high", 27));
			}
			FIELD("RampVIDOff", ENUM([0, "mV_0"],
						      [1, "mV_25"],
						      [2, "mV_50"],
						      [3, "mV_75"],
						      [4, "mV_100"],
						      [5, "mV_125"],
						      [6, "mV_150"],
						      [7, "mV_175"]),
					BITS("../%clock_power_timing_high", 30, 28));
		});

		//
		// HyperTransport Read Pointer Optimization register
		//
		REG32("%ht_read_ptr_optimization", 0xDC);
		SCOPE("ht_read_pointer_optimiation", {
			type ht_clocks = INT("HT clock(s)");

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
		});

		//
		// Thermtrip Status register
		//
		REG32("%thermtrip_status", 0xE4);
		SCOPE("thermtrip_status", {
			FIELD("Thermtp", yesno_t, BITS("../%thermtrip_status", 1));
			if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("ThermSenseCoreSel", BOOL("cpu0", "cpu1"),
						BITS("../%thermtrip_status", 2));
			}
			FIELD("ThermtpSense0", yesno_t, BITS("../%thermtrip_status", 3));
			FIELD("ThermtpSense1", yesno_t, BITS("../%thermtrip_status", 4));
			FIELD("ThermtpEn", enabledisable_t, BITS("../%thermtrip_status", 5));
			if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("ThermSenseSel", int_t,
						BITS("../%thermtrip_status", 6));
			}
			if (EVAL("$k8/k8_rev") == "rev_e") {
				FIELD("DiodeOffsetSignBit", BOOL("negative", "positive"),
						BITS("../%thermtrip_status", 24));
				if (EVAL("DiodeOffsetSignBit") == "negative") {
					FIELD("DiodeOffset", XFORM(INT("C"),
								LAMBDA(-1 * _1),
								LAMBDA(-1 * _1)),
							BITS("../%thermtrip_status", 13, 8));
				} else { // positive
					FIELD("DiodeOffset", INT("C"),
							BITS("../%thermtrip_status", 13, 8));
				}
				FIELD("TCaseMax", XFORM(INT("degrees C"),
							LAMBDA(_1 + 51), LAMBDA(_1 - 51)),
						BITS("../%thermtrip_status", 28, 25));
			} else if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("DiodeOffset", XFORM(INT("degrees C"),
								LAMBDA(11 - _1),
								LAMBDA((-1 * _1) + 11)),
						BITS("../%thermtrip_status", 13, 8));
				FIELD("CurTmp", XFORM(INT("degrees C"),
							    LAMBDA(_1 - 49),
							    LAMBDA(_1 + 49)),
						BITS("../%thermtrip_status", 23, 14));
				FIELD("TjOffset", int_t,
						BITS("../%thermtrip_status", 28, 24));
			}
			FIELD("SwThermtp", onoff_t, BITS("../%thermtrip_status", 31));
		});

		//
		// Northbridge Capabilities register
		//
		REG32("%northbridge_capabilities", 0xE8);
		SCOPE("northbridge_capabilities", {
			// FIXME: what to do about field names starting with a number?
			FIELD("_128BitCap", yesno_t, BITS("../%northbridge_capabilities", 0));
			FIELD("MPCap", yesno_t, BITS("../%northbridge_capabilities", 1));
			FIELD("BigMPCap", yesno_t, BITS("../%northbridge_capabilities", 2));
			FIELD("EccCap", yesno_t, BITS("../%northbridge_capabilities", 3));
			FIELD("ChipKillEccCap", yesno_t,
					BITS("../%northbridge_capabilities", 4));
			FIELD("DramFreq", ENUM([0, "no_limit"],
						    [1, "MHz_166"],
						    [2, "MHz_133"],
						    [3, "MHz_100"]),
					BITS("../%northbridge_capabilities", 6, 5));
			FIELD("MemCntCap", yesno_t, BITS("../%northbridge_capabilities", 8));
			if (EVAL("$k8/k8_rev") == "rev_f") {
				FIELD("HtcCap", yesno_t,
						BITS("../%northbridge_capabilities", 10));
			}
			FIELD("CmpCap", ENUM([0, "single_core"],
						  [1, "dual_Core"]),
					BITS("../%northbridge_capabilities", 13, 12));
		});

		if (EVAL("$k8/k8_rev") == "rev_f") {
			//
			// CPUID Family Model register
			//

			// Don't bother separating out the fields of this register
			// because we already handle it in CPUID.
			FIELD("cpuid_family_model", hex32_t, REG32(0xFC));
		}

	}); //misc_ctrl
}

/* FIXME: re-enable this
// Register our 'keystone' PCI device.
DISCOVER("pci", 0x1022, 0x1100, {
	int seg = args[0];
	int bus = args[1];
	int dev = args[2];
	int fun = args[3];

	SCOPE("k8[]", {
		BOOKMARK("k8");

		//FIXME: this would break if PCI discovery runs before CPU.  This
		//   is not true today, and once we have a more dynamic tree this
		//   will not be an issue
		// for simplicity
		ALIAS("k8_rev", "/cpu[0]/k8_rev");

		// function 0
		k8_ht_config(seg, bus, dev, fun);

		// function 1
		k8_address_map(seg, bus, dev, fun);

		// function 2
		k8_dram_controller(seg, bus, dev, fun);

		// function 3
		k8_misc_control(seg, bus, dev, fun);
	}); // k8[]
});

// Ignore other sub-devices in this same device, we'll handle them above.
DISCOVER("pci", 0x1022, 0x1101);
DISCOVER("pci", 0x1022, 0x1102);
DISCOVER("pci", 0x1022, 0x1103);
*/

// Called from discovery code below.
k8_cpu()
{
	int cpu = args[0];

	string scope_name = sprintf("cpu.%d", cpu);
	SCOPE(scope_name, {
		BOOKMARK("cpu");

		// define CPUID regs
		k8_cpuid(cpu);

		// add a field for k8 revision to each core
		type k8_rev_t = ENUM(
			[0xe, "rev_e"],
			[0xf, "rev_f"],
			[0x0, "unknown"]);
		if (EVAL("cpuid/family") == 0xf) {
			if (EVAL("cpuid/model") >= 0x40) {
				FIELD("k8_rev", k8_rev_t, 0xf);
			} else if (EVAL("cpuid/model") >= 0x20) {
				FIELD("k8_rev", k8_rev_t, 0xe);
			} else {
				FIELD("k8_rev", k8_rev_t, 0);
			}
		} else {
			FIELD("k8_rev", k8_rev_t, 0);
		}

		// define MSR regs
		k8_msr(cpu);
	}); // cpu
	ALIAS("cpu[]", scope_name);
}
/* FIXME: re-enable this
DISCOVER("cpu",                       // k8 rev E
         0x444d416369746e6568747541,  // vendor = AMD
         0x0f, 0x0f,                  // family range
         0x20, 0x39,                  // model range
         0x00, 0xFF, k8_cpu);         // stepping range
DISCOVER("cpu",                       // k8 rev F
         0x444d416369746e6568747541,  // vendor = AMD
         0x0f, 0x0f,                  // family range
         0x40, 0xFF,                  // model range
         0x00, 0xFF, k8_cpu);        // stepping range
//FIXME: register other devices this owns
*/
