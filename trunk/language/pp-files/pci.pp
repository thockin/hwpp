import "pp";

module pci;

// Register a discovery catch-all for the "pci" driver.
discover("pci", [], ${
	int seg = args[0];
	int bus = args[1];
	int dev = args[2];
	int fun = args[3];

	string name = sprintf("pci.%d.%d.%d.%d", seg, bus, dev, fun);
	PCI_SCOPE(name, seg, bus, dev, fun, ${});
});

//TODO: unify cap, sts, en, dis, msk, svr names everywhere.  Look at aer

// NOTE: all symbols are file-local unless marked 'public'

///////////////// Function literals and pointers /////////////////////
//
// func BAR = ${ string name = $1; int address = $2; ...  }
//       equiv to
// func<string name, int address> BAR = ${ ... }
//       equiv to
// func BAR(string name, int address) { ... }
//
// Notes:
//
// Args are passed as a list<var>, called "args".  All lists are really
// associative arrays, so args[0] works as well as args["abc"].  If the
// function signature has typed names, the names are filled from the list
// values of the same name and type checked at run time (var types can accept
// anything).
//
// If the caller specifies named args, the values are assigned the specified
// keys in the args list.  If the caller does not specify names, the args are
// name, in order, to correspond with the called function's signature
// (provided that types match).  Unnamed args beyond the called function's
// signature list are given random names.  Mixing named and unnamed args in a
// single function call is probably a mistake, and a warning should be issued.
//
// Accessing a list key that does not exist results in 'null' (a key word).
// The args map can be iterated as 'for (k,v) in args' or similar, and are
// iterated in insert order.
//
// If you define a function taking named params, you should name all params.
// If you need flexible params, name none or take a list as the last
// argument.
//
// Consider:
//     FOO(int x, string y) { ... }
// Calls:
//     FOO(x: 93, y: "seventy-six", z: false);
//          -> x = args["x"] = args[0] = 93
//          -> y = args["y"] = args[1] = "seventy-six"
//          -> args["z"] = args[2] = false
//          -> iterated as 93, "seventy-six", false
//     FOO(y: "seventy-six", z: false, x: 93);
//          -> x = args["x"] = args[2] = 93
//          -> y = args["y"] = args[0] = "seventy-six"
//          -> args["z"] = args[1] = false
//          -> iterated as "seventy-six", false, 93
//     FOO(93, "seventy-six", false);
//          -> x = args["x"] = args[0] = 93
//          -> y = args["y"] = args[1] = "seventy-six"
//          -> args[2] = false
//          -> iterated as 93, "seventy-six", false
//     FOO("seventy-six", false, 93);
//          -> x = args["x"] = args[0] = "seventy-six" => type error
//          -> y = args["y"] = args[1] = false => type error;
//          -> args[2] = 93
//          -> iterated as "seventy-six", false, 93
//
// Consider:
//     BAR() { ... }
// Calls:
//     BAR(x: 93, y: "seventy-six", z: false);
//          -> args["x"] = args[0] = 93
//          -> args["y"] = args[1] = "seventy-six";
//          -> args["z"] = args[2] = false
//          -> iterated as 93, "seventy-six", false
//     FOO(y: "seventy-six", z: false, x: 93);
//          -> args["x"] = args[2] = 93
//          -> args["y"] = args[0] = "seventy-six";
//          -> args["z"] = args[1] = false
//          -> iterated as "seventy-six", false, 93
//     FOO(93, "seventy-six", false);
//          -> args[0] = 93
//          -> args[1] = "seventy-six";
//          -> args[2] = false
//          -> iterated as 93, "seventy-six", false
//     FOO("seventy-six", false, 93);
//          -> args[0] = "seventy-six"
//          -> args[1] = false
//          -> args[2] = 93
//          -> iterated as "seventy-six", false, 93

//FIXME: how do I check that a var's type is what I expect?  Maybe implicit
//       conversions do this?
//    var x;
//    int y = x; // <- error
//    int z = int(x) // <- conversion, but may fail?

public fldfmt pci_vendor_t = ENUM(
	[0, "unknown"],
	[0x8086, "intel"],
	[0x1022, "amd"],
	[0x10de, "nvidia"],
	[0x14e4, "broadcom"],
	[0x1095, "silicon_image"],
	[0x11ab, "marvell"],
	[0x1ae0, "google"],
	[0x10b5, "plx"]);
	//FIXME: default?

public fldfmt pci_class_t = ENUM(
	//TODO: incomplete list
	[0x00, "pre_classcode"],
	[0x01, "mass_storage"],
	[0x02, "network"],
	[0x03, "display"],
	[0x04, "multimedia"],
	[0x05, "memory"],
	[0x06, "bridge"],
	[0x07, "simple_comm"],
	[0x08, "base_system"],
	[0x09, "input"],
	[0x0a, "docking"],
	[0x0b, "processor"],
	[0x0c, "serial"],
	[0x0d, "wireless"],
	[0x0e, "intelligent_io"],
	[0x0f, "satellite"],
	[0x10, "crypto"],
	[0x11, "dsp"],
	[0xff, "other"]);
	//FIXME: default?

//FIXME: use pci_subclass_t[$class] notation?  What about unknown classes?
public fldfmt pci_subclass_pre_classcode_t = ENUM(
		[0x00, "non_vga_dev"],
		[0x01, "vga_dev"]);
public fldfmt pci_subclass_mass_storage_t = ENUM(
		[0x00, "scsi_ctrlr"],
		[0x01, "ide_ctrlr"],
		[0x02, "floppy_ctrlr"],
		[0x03, "ipi_bus_ctrlr"],
		[0x04, "raid_ctrlr"],
		[0x05, "ata_ctrlr"],
		[0x06, "sata_ctrlr"],
		[0x06, "sas_ctrlr"],
		[0x80, "other"]);
public fldfmt pci_subclass_network_t = ENUM(
		[0x00, "ethernet_ctrlr"],
		[0x01, "token_ring_ctrlr"],
		[0x02, "fddi_ctrlr"],
		[0x03, "atm_ctrlr"],
		[0x04, "isdn_ctrlr"],
		[0x80, "other"]);
public fldfmt pci_subclass_display_t = ENUM(
		[0x00, "vga_ctrlr"],
		[0x01, "xga_ctrlr"],
		[0x02, "three_d_ctrlr"],
		[0x80, "other"]);
public fldfmt pci_subclass_multimedia_t = ENUM(
		[0x00, "video_dev"],
		[0x01, "audio_dev"],
		[0x02, "computer_telephony_dev"],
		[0x80, "other"]);
public fldfmt pci_subclass_memory_t = ENUM(
		[0x00, "ram"],
		[0x01, "flash"],
		[0x80, "other"]);
public fldfmt pci_subclass_bridge_t = ENUM(
		[0x00, "host_bridge"],
		[0x01, "isa_bridge"],
		[0x02, "eisa_bridge"],
		[0x03, "mca_bridge"],
		[0x04, "pci_bridge"],
		[0x05, "pcmcia_bridge"],
		[0x06, "nubus_bridge"],
		[0x07, "cardbus_bridge"],
		[0x08, "raceway_bridge"],
		[0x80, "other"]);
//FIXME: classes 0x07-0x11

public fldfmt pci_bar_t = ENUM(
		[0, "bar0"],
		[1, "bar1"],
		[2, "bar2"],
		[3, "bar3"],
		[4, "bar4"],
		[5, "bar5"]);

public fldfmt pci_capability_t = ENUM(
		[0, "unknown"],
		[0x01, "power_mgmt"],
		[0x02, "agp"],
		[0x03, "vpd"],
		[0x04, "slot_id"],
		[0x05, "msi"],
		[0x06, "hot_swap"],
		[0x07, "pcix"],
		[0x08, "ht"],
		[0x09, "vendor"],
		[0x0a, "usb2_dbg_port"],
		[0x0b, "resource_ctrl"],
		[0x0c, "hot_plug"],
		[0x0d, "ssid"],
		[0x0e, "agp8x"],
		[0x0f, "secure"],
		[0x10, "pcie"],
		[0x11, "msix"],
		[0x12, "sata"],
		[0X13, "af"]);

public fldfmt pcie_capability_t = ENUM(
		[0, "unknown"],
		[0x01, "aer"],
		[0x02, "vchannel"],
		[0x03, "serial_num"],
		[0x04, "power_budget"],
		[0x05, "root_link_decl"],
		[0x06, "root_internal_link_ctrl"],
		[0x07, "root_event_collector_endpoint_assoc"],
		[0x08, "multi_func_vchannel"],
		[0x09, "vc2"],
		[0x0a, "rcrb_header"],
		[0x0b, "vendor_specific"],
		[0x0d, "acs"]);

// All standard BARs look like this.
public
BAR(string name, int address)
{
	//FIXME: how do we parse this { ... } as a function ptr, but if { ... }
	//       or even a simple nested scope { int i; bar(); } as inline
	//       - make them all function calls, if it is an rval, make it
	//         function, else make it an inline, called immediately?
	//       - make it contextual
	//       - change it to {{ ... }} or ={ ... } or something
	//       - make it contextual AND use {{ ... }}
	SCOPE(name, ${
		REG32("%lower", address);
		//NOTE: args to enum are key:value, key can be int or string!
		//      Just pass all args as a hash.
		//TODO: something more clever with BITS
		FIELD("type", ENUM([0, "mem"], [1, "io"]), BITS("%lower", 0));

		//TODO: what if the arg to EVAL is a reg?
		if (EVAL("type") == "mem") {
			FIELD("width",
			      ENUM( [0, "bits32"],
			            [1, "bits20"],
			            [2, "bits64"]),
			      BITS("%lower", 2, 1));
			FIELD("prefetch", "yesno_t", BITS("%lower", 3));
		}

		if (EVAL("type") == "io") {
			FIELD("address", "addr16_t",
					BITS("%lower", 15, 2) +
					BITS("%0", 1, 0));
		//TODO: replace "else if" with "elseif"?
		} else if (EVAL("width") == "bits32"
		        || EVAL("width") == "bits20") {
			FIELD("address", "addr32_t",
					BITS("%lower", 31, 4) +
					BITS("%0", 3, 0));
		} else {
			REG32("%upper", address+4);
			FIELD("address", "addr64_t",
					BITS("%upper", 31, 0) +
					BITS("%lower", 31, 4) +
					BITS("%0", 3, 0));
		}
	});
}

ht_link_control(int address)
{
	REG16("%control", address);
	SCOPE("control", ${
		FIELD("SrcIdEn", "yesno_t", BITS("../%control", 0));
		FIELD("CFIEn", "yesno_t", BITS("../%control", 1));
		FIELD("CST", "yesno_t", BITS("../%control", 2));
		FIELD("CFEr", "yesno_t", BITS("../%control", 3));
		FIELD("LinkFail", "yesno_t", BITS("../%control", 4));
		FIELD("InitCmp", "yesno_t", BITS("../%control", 5));
		FIELD("EOC", "yesno_t", BITS("../%control", 6));
		FIELD("TXOff", "yesno_t", BITS("../%control", 7));
		FIELD("CRCErr", "hex4_t", BITS("../%control", 11, 8));
		FIELD("IsocEn", "yesno_t", BITS("../%control", 12));
		FIELD("LSEn", "yesno_t", BITS("../%control", 13));
		FIELD("ExtCTL", "yesno_t", BITS("../%control", 14));
		FIELD("b64En", "yesno_t", BITS("../%control", 15));
	});
}

ht_link_config(int address)
{
	fldfmt ht_link_width_t = ENUM( [0, "bits8"],
	                             [1, "bits16"],
	                             [3, "bits32"],
	                             [4, "bits2"],
	                             [5, "bits4"],
	                             [7, "nc"]);

	REG16("%config", address);
	SCOPE("config", ${
		FIELD("MaxLinkWidthIn", "ht_link_width_t",
		      BITS("../%config", 2, 0));
		FIELD("DwFcIn", "yesno_t",
		      BITS("../%config", 3));
		FIELD("MaxLinkWidthOut", "ht_link_width_t",
		      BITS("../%config", 6, 4));
		FIELD("DwFcOut", "yesno_t",
		      BITS("../%config", 7));

		FIELD("LinkWidthIn", "ht_link_width_t",
		      BITS("../%config", 10, 8));
		FIELD("DwFcInEn", "yesno_t",
		      BITS("../%config", 11));
		FIELD("LinkWidthOut", "ht_link_width_t",
		      BITS("../%config", 14, 12));
		FIELD("DwFcOutEn", "yesno_t",
		      BITS("../%config", 15));
	});
}

ht_link_freq_err(int address)
{
	fldfmt ht_link_freq_t = ENUM(
			[0, "mhz200"],
			[1, "mhz300"],
			[2, "mhz400"],
			[3, "mhz500"],
			[4, "mhz600"],
			[5, "mhz800"],
			[6, "mhz1000"],
			[7, "mhz1200"],
			[8, "mhz1400"],
			[9, "mhz1600"],
			[1, "mhz1800"],
			[11, "mhz2000"],
			[12, "mhz2200"],
			[13, "mhz2400"],
			[14, "mhz2600"],
			[15, "mhz100"]);

	REG8("%freq_err", address);
	FIELD("Freq", "ht_link_freq_t", BITS("%freq_err", 3, 0));
	FIELD("ProtoErr", "yesno_t", BITS("%freq_err", 4));
	FIELD("OverErr", "yesno_t", BITS("%freq_err", 5));
	FIELD("EOCErr", "yesno_t", BITS("%freq_err", 6));
	FIELD("CTLTimeout", "yesno_t", BITS("%freq_err", 7));
}

ht_link_freq_cap(int address)
{
	fldfmt ht_link_freq_cap_t = BITMASK(
			[0, "mhz200"],
			[1, "mhz300"],
			[2, "mhz400"],
			[3, "mhz500"],
			[4, "mhz600"],
			[5, "mhz800"],
			[6, "mhz1000"],
			[7, "mhz1200"],
			[8, "mhz1400"],
			[9, "mhz1600"],
			[10, "mhz1800"],
			[11, "mhz2000"],
			[12, "mhz2200"],
			[13, "mhz2400"],
			[14, "mhz2600"]);

	REG16("%freq_cap", address);
	FIELD("FreqCap", "ht_link_freq_cap_t", BITS("%freq_cap", 14, 0));
}

ht_error_handling(int address)
{
	REG16("%error", address);
	SCOPE("error", ${
		//TODO: var error = GET_REGISTER("../error") -> tmp local alias?
		FIELD("proto_flood", "yesno_t", BITS("../%error", 0));
		FIELD("over_flood", "yesno_t", BITS("../%error", 1));
		FIELD("proto_fatal", "yesno_t", BITS("../%error", 2));
		FIELD("over_fatal", "yesno_t", BITS("../%error", 3));
		FIELD("eoc_fatal", "yesno_t", BITS("../%error", 4));
		FIELD("resp_fatal", "yesno_t", BITS("../%error", 5));
		FIELD("crc_fatal", "yesno_t", BITS("../%error", 6));
		FIELD("serr_fatal", "yesno_t", BITS("../%error", 7));
		FIELD("chain_fail", "yesno_t", BITS("../%error", 8));
		FIELD("resp_err", "yesno_t", BITS("../%error", 9));
		FIELD("proto_nonfatal", "yesno_t", BITS("../%error", 10));
		FIELD("over_nonfatal", "yesno_t", BITS("../%error", 11));
		FIELD("eoc_nonfatal", "yesno_t", BITS("../%error", 12));
		FIELD("resp_nonfatal", "yesno_t", BITS("../%error", 13));
		FIELD("crc_nonfatal", "yesno_t", BITS("../%error", 14));
		FIELD("serr_nonfatal", "yesno_t", BITS("../%error", 15));
	});
}

ht_slave_link(string name, int address)
{
	SCOPE(name, ${
		ht_link_control(address);
		ht_link_config(address+2);
		ht_link_freq_err(address+0x09);
		ht_link_freq_cap(address+0x0a);
	});
}

// HyperTransport primary/slave
ht_slave_capability(int address)
{
	REG16("%command", address + 2);
	FIELD("base_unit_id", "int_t", BITS("%command", 4, 0));
	FIELD("unit_count", "int_t", BITS("%command", 9, 5));
	FIELD("master", "yesno_t", BITS("%command", 10));
	FIELD("default_direction", "yesno_t", BITS("%command", 11));
	FIELD("drop_on_uninit", "yesno_t", BITS("%command", 12));

	ht_slave_link("link0", address + 0x04);
	ht_slave_link("link1", address + 0x08);

	//FIXME: clean up HT field names all over - inconsistent BumpyCaps
	REG8("%rev", address + 0x0c);
	FIELD("MajorRev", "int_t", BITS("%rev", 7, 5));
	FIELD("MinorRev", "int_t", BITS("%rev", 4, 0));

	REG8("%feature", address + 0x10);
	SCOPE("feature", ${
		FIELD("iso_flow_ctrl", "yesno_t", BITS("../%feature", 0));
		FIELD("ldtstop", "yesno_t", BITS("../%feature", 1));
		FIELD("crc_test_mode", "yesno_t", BITS("../%feature", 2));
		FIELD("extended_ctl_time", "yesno_t", BITS("../%feature", 3));
		FIELD("bits64_addressing", "yesno_t", BITS("../%feature", 4));
		FIELD("unit_id_reorder_dis", "yesno_t", BITS("../%feature", 5));
		FIELD("source_id", "yesno_t", BITS("../%feature", 6));
	});

	FIELD("scratchpad", "hex16_t", REG16(address + 0x14));

	ht_error_handling(address + 0x16);

	FIELD("mem_base_hi", "addr16_t", REG8(address + 0x18));
	FIELD("mem_limit_hi", "addr16_t", REG8(address + 0x19));
	FIELD("bus", "int_t", REG8(address + 0x1a));
}

ht_host_link(string name, int address)
{
	SCOPE(name, ${
		ht_link_control(address);
		ht_link_config(address+2);
		ht_link_freq_err(address+0x05);
		ht_link_freq_cap(address+0x06);
	});
}

ht_host_capability(int address)
{
	REG16("%command", address + 2);
	FIELD("warm_reset", "yesno_t", BITS("%command", 0));
	FIELD("dbl_ended", "yesno_t", BITS("%command", 1));
	FIELD("device", "int_t", BITS("%command", 6, 2));
	FIELD("chain_side", "yesno_t", BITS("%command", 7));
	FIELD("host_hide", "yesno_t", BITS("%command", 8));
	FIELD("act_slave", "yesno_t", BITS("%command", 10));
	FIELD("in_eoc_err", "yesno_t", BITS("%command", 11));
	FIELD("drop_on_uninit", "yesno_t", BITS("%command", 12));

	ht_host_link("link", address + 0x04);

	REG8("%rev", address + 0x08);
	FIELD("major_rev", "int_t", BITS("%rev", 7, 5));
	FIELD("minor_rev", "int_t", BITS("%rev", 4, 0));

	//FIXME: some of these only apply to ht3, if you want to be strict
	REG16("%feature", address + 0x0c);
	SCOPE("feature", ${
		FIELD("iso_flow_ctrl", "yesno_t", BITS("../%feature", 0));
		FIELD("ldtstop", "yesno_t", BITS("../%feature", 1));
		FIELD("crc_test_mode", "yesno_t", BITS("../%feature", 2));
		FIELD("extended_ctl_time", "yesno_t", BITS("../%feature", 3));
		FIELD("bits64_addressing", "yesno_t", BITS("../%feature", 4));
		FIELD("unit_id_reorder_dis", "yesno_t", BITS("../%feature", 5));
		FIELD("source_id", "yesno_t", BITS("../%feature", 6));
		FIELD("ext_regs", "yesno_t", BITS("../%feature", 8));
		FIELD("upstream_cfg", "yesno_t", BITS("../%feature", 9));
	});

	//NOTE: comparison operators are specific to a datatype
	if (EVAL("feature/ext_regs") == TRUE) {
		FIELD("scratchpad", "hex16_t", REG16(address + 0x10));
		ht_error_handling(address + 0x12);
		FIELD("mem_base_hi", "addr16_t", REG8(address + 0x14));
		FIELD("mem_limit_hi", "addr16_t", REG8(address + 0x15));
	}
}

ht_revision_capability(int address)
{
	REG8("%rev", address+2);
	FIELD("major_rev", "int_t", BITS("%rev", 7, 5));
	FIELD("minor_rev", "int_t", BITS("%rev", 4, 0));
}

ht_extended_config_capability(int address)
{
	REG32("%address", address+4);
	FIELD("type", ENUM([0, "type0"], [1, "type1"]), BITS("%address", 28));
	FIELD("bus", "int_t", BITS("%address", 27, 20));
	FIELD("device", "int_t", BITS("%address", 19, 15));
	FIELD("function", "int_t", BITS("%address", 14, 12));
	FIELD("register", "hex_t",
			BITS("%address", 11, 2) +
			BITS("%0", 1, 0));
	FIELD("data", "hex32_t", REG32(address+8));
}

ht_address_mapping_capability(int address)
{
	REG16("%command", address + 0x02);
	FIELD("num_dma", "int_t", BITS("%command", 3, 0));
	FIELD("io_size", "int_t", BITS("%command", 8, 4));
	FIELD("map_type", ENUM([0, "bits40"], [1, "bits64"]),
			BITS("%command", 10, 9));

	//TODO: change EVAL to GET_FIELD ?
	if (EVAL("map_type") == "bits40") {
		REG32("%sec_non_prefetch", address + 0x04);
		SCOPE("sec_non_prefetch", ${
			FIELD("enable", "yesno_t",
			      BITS("../%sec_non_prefetch", 31));
			FIELD("isoch", "yesno_t",
			      BITS("../%sec_non_prefetch", 30));
			FIELD("noncoherent", "yesno_t",
			      BITS("../%sec_non_prefetch", 29));
			FIELD("compat", "yesno_t",
			      BITS("../%sec_non_prefetch", 28));
			FIELD("base", "addr64_t",
			      BITS("../%sec_non_prefetch", 19, 0) +
			      BITS("%0", 19, 0));
		});

		REG32("%sec_prefetch", address + 0x08);
		SCOPE("sec_prefetch", ${
			FIELD("enable", "yesno_t",
			      BITS("../%sec_prefetch", 31));
			FIELD("isochronous", "yesno_t",
			      BITS("../%sec_prefetch", 30));
			FIELD("noncoherent", "yesno_t",
			      BITS("../%sec_prefetch", 29));
			FIELD("compat", "yesno_t",
			      BITS("../%sec_prefetch", 28));
			FIELD("base", "addr64_t",
			      BITS("../%sec_prefetch", 19, 0) +
			      BITS("%0", 19, 0));
		});

		int value = READ("num_dma");
		for (int i = 0; i < value; i++) {
			SCOPE("dma[]", ${
				REG32("%lower", address + 0x0c + (8*i));
				REG32("%upper", address + 0x10 + (8*i));

				FIELD("enable", "yesno_t",
				      BITS("%lower", 31));
				FIELD("isochronous", "yesno_t",
				      BITS("%lower", 30));
				FIELD("noncoherent", "yesno_t",
				      BITS("%lower", 29));

				FIELD("pri_base", "addr64_t",
				      BITS("%lower", 15, 0) +
				      BITS("%0", 23, 0));
				FIELD("sec_base", "addr64_t",
				      BITS("%lower", 31, 16) +
				      BITS("%0", 23, 0));
				FIELD("sec_limit", "addr64_t",
				      BITS("%lower", 15, 0) +
				      BITS("%1", 23, 0));
			});
		}
	} else if (EVAL("map_type") == "bits64") {
		//FIXME: procfields index/data pair access within PCI
	}
}

ht_msi_mapping_capability(int address)
{
	REG8("%flags", address+2);
	FIELD("en", "yesno_t", BITS("%flags", 0));
	FIELD("fixed", "yesno_t", BITS("%flags", 1));
	if (EVAL("fixed") == FALSE) {
		REG32("%lower", address+4);
		REG32("%upper", address+8);
		FIELD("address", "addr64_t",
		      BITS("%upper", 31, 0) +
		      BITS("%lower", 31, 20) +
		      BITS("%0", 19, 0));
	}
}

power_mgmt_capability(int address)
{
	REG16("%pmc", address+2);
	FIELD("version", ENUM(
				[1, "pcipm_v1_0"],
				[2, "pcipm_v1_1"],
				[3, "pcipm_v1_2"]),
			BITS("%pmc", 2, 0));
	FIELD("clock", "yesno_t", BITS("%pmc", 3));
	FIELD("dsi", "yesno_t", BITS("%pmc", 5));
	FIELD("aux_current", ENUM(
				[0, "self"],
				[1, "mA_55"],
				[2, "mA_100"],
				[3, "mA_160"],
				[4, "mA_220"],
				[5, "mA_270"],
				[6, "mA_320"],
				[7, "mA_375"]),
			BITS("%pmc", 8, 6));
	FIELD("d1_support", "yesno_t", BITS("%pmc", 9));
	FIELD("d2_support", "yesno_t", BITS("%pmc", 10));
	FIELD("pme_support", BITMASK(
				[0, "d0"],
				[1, "d1"],
				[2, "d2"],
				[3, "d3hot"],
				[4, "d3cold"]),
			BITS("%pmc", 15, 11));

	REG16("%pmcsr", address+4);
	FIELD("power_state", ENUM(
				[0, "d0"],
				[1, "d1"],
				[2, "d2"],
				[3, "d3hot"]),
			BITS("%pmcsr", 1, 0));
	FIELD("no_soft_reset", "yesno_t", BITS("%pmcsr", 3));
	FIELD("pme_en", "yesno_t", BITS("%pmcsr", 8));
	FIELD("pme_status", "yesno_t", BITS("%pmcsr", 15));

	REG8("%pmcsr_bse", address+6);
	FIELD("bpcc_en", "yesno_t", BITS("%pmcsr_bse", 7));
	if (EVAL("bpcc_en") == TRUE) {
		FIELD("b2_b3", "yesno_t", BITS("%pmcsr_bse", 6));
	}

	//FIXME: procfield: this is really an index/data pair set of regs
	FIELD("data_select", "hex4_t", BITS("%pmcsr", 12, 9));
	FIELD("data_scale", "hex4_t", BITS("%pmcsr", 14, 13));
	REG8("%data", address+7);
	FIELD("data", "hex8_t", BITS("%data", 7, 0));
}

slot_id_capability(int address)
{
	REG8("%slot", address+2);
	FIELD("nslots", "int_t", BITS("%slot", 4, 0));
	FIELD("first", "yesno_t", BITS("%slot", 5));
	FIELD("chassis", "int_t", REG8(address+3));
}

msi_capability(int address)
{
	// message control
	REG16("%msg_ctrl", address + 2);
	FIELD("msi_enable", "yesno_t", BITS("%msg_ctrl", 0));
	FIELD("multi_msg_cap", ENUM(
				[0, "vec1"],
				[1, "vec2"],
				[2, "vec4"],
				[3, "vec8"],
				[4, "vec16"],
				[5, "vec32"]),
			BITS("%msg_ctrl", 3, 1));
	FIELD("multi_msg_en", ENUM(
				[0, "vec1"],
				[1, "vec2"],
				[2, "vec4"],
				[3, "vec8"],
				[4, "vec16"],
				[5, "vec32"]),
			BITS("%msg_ctrl", 6, 4));
	FIELD("cap64", "yesno_t", BITS("%msg_ctrl", 7));
	FIELD("mask_per_vec", "yesno_t", BITS("%msg_ctrl", 8));

	// message address
	REG32("%msg_addr", address + 4);

	// is this 64 bit capable?
	if (EVAL("cap64") == FALSE) {
		// no, just use the low address and define the data
		FIELD("msg_addr", "addr32_t",
				BITS("%msg_addr", 31, 2) +
				BITS("%0", 1, 0));
		FIELD("msg_data", "hex16_t", REG16(address + 8));

		if (EVAL("mask_per_vec") == TRUE) {
			// mask and pending bits
			REG32("%mask", address + 12);
			REG32("%pending", address + 16);
		}
	} else {
		// add the high address and define the data
		REG32("%msg_addr_hi",  address + 8);
		FIELD("msg_addr", "addr64_t",
				BITS("%msg_addr_hi", 31, 0) +
				BITS("%msg_addr", 31, 2) +
				BITS("%0", 1, 0));
		FIELD("msg_data", "hex16_t", REG16(address + 12));

		if (EVAL("mask_per_vec") == TRUE) {
			// mask and pending bits
			REG32("%mask", address + 16);
			REG32("%pending", address + 20);
		}
	}

	if (EVAL("mask_per_vec") == TRUE) {
		int vecs = 1 << READ("multi_msg_cap");
		for (int i = 0; i < vecs; i++) {
			FIELD("mask[]", "yesno_t", BITS("%mask", i));
			FIELD("pend[]", "yesno_t", BITS("%pending", i));
		}
	}
}

msix_capability(int address)
{
	REG16("%msg_ctrl", address + 2);
	FIELD("msix_enable", "yesno_t", BITS("%msg_ctrl", 15));
	//TODO: XFORM<int_t>() ?
	//TODO: {$1+1} or fn(int x){x+1} or ...
	FIELD(name: "table_size",
	      //FIXME: 'type' is a keyword, so it can't be an arg name.  yuck
	      fmt:  XFORM(fmt:   int_t,
	                  read:  ${return argv[0]+1;},
	                  write: ${return argv[0]-1;}),
	      bits: BITS("%msg_ctrl", 9, 0));
	FIELD("func_mask", "yesno_t", BITS("%msg_ctrl", 14));

	// the table is memory mapped through a BAR
	REG32("%table_ptr", address + 4);
	FIELD("table_bir", "pci_bar_t", BITS("%table_ptr", 2, 0));
	FIELD("table_offset", "hex_t",
			BITS("%table_ptr", 31, 3) +
			BITS("%0", 2, 0));

	//FIXME: I should not have to do this - the lambdas should handle it
	int table_size = READ("table_size") + 1;

	//FIXME: a better way to do this?
	string bar;
	int base;
	int size;

	//FIXME: broken!!! EVAL("table_bir")?
	//FIXME: need language support.  sprintf?
	//bar = "$pci/" + GET_FIELD("table_bir")->evaluate() + "/address";
	base = READ(bar) + READ("table_offset");
	size = table_size * 16;
	SCOPE("table", BIND("mem", base, size), ${
		for (int i = 0; i < table_size; i++) {
			SCOPE("entry[]", ${
				REG32("%msg_addr", i*16 + 0);
				REG32("%msg_upper_addr", i*16 + 4);
				FIELD("address", "addr64_t",
						BITS("%msg_upper_addr", 31, 0) +
						BITS("%msg_addr", 31, 2) +
						BITS("%0", 1, 0));
				FIELD("msg_data", "hex_t", REG32(i*16 + 8));
				REG32("%vector_ctrl", i*16 + 12);
				FIELD("mask", "yesno_t",
						BITS("%vector_ctrl", 0));
			});
		}
	});

	// the pending bit array is memory mapped through a BAR
	REG32("%pba_ptr", address + 8);
	FIELD("pba_bir", "pci_bar_t", BITS("%pba_ptr", 2, 0));
	FIELD("pba_offset", "hex_t",
			BITS("%pba_ptr", 31, 3) +
			BITS("%0", 2, 0));

	//FIXME: a better way to do this?
	//FIXME: need language support.  sprintf?
	//bar = "$pci/" + GET_FIELD("pba_bir")->evaluate() + "/address";
	base = READ(bar) + READ("table_offset");
	size = (table_size+63) / 64;
	SCOPE("pba", BIND("mem", ARGS(base, size * 8)), ${
		int tmp_size = table_size;
		// loop for each PBA QWORD
		for (int i = 0; i < size; i++) {
			REG64("%pending[]", i);
			for (int j = 0; j < 64; j++) {
				if (j >= tmp_size) {
					break;
				}
				//TODO: sprintf
				FIELD("pending" + to_string(i*64 + j),
				      "yesno_t", BITS("%pending[-1]", j));
			}
			tmp_size -= 64;
		}
	});
}

ht_capability(int address)
{
	REG8("%subcap", address + 3);
	// Check the upper two bits - if they are zero, then we have
	// a slave/primary interface block or a host/secondary interface
	// block.  If they are non-zero, we have some other HT capability.
	// This matters because they use different encodings of the
	// capability type (3 bits vs 5 bits).
	FIELD("is_interface", BOOL("no", "yes"), BITS("%subcap", 7, 6));
	if (EVAL("is_interface") == "yes") {
		// subtype is a 3 bit field
		FIELD("subtype", ENUM(
					[0, "slave_primary"],
					[1, "host_secondary"]),
				BITS("%subcap", 7, 5));

		if (EVAL("subtype") == "slave_primary") {
			ht_slave_capability(address);
		} else if (EVAL("subtype") == "host_secondary") {
			ht_host_capability(address);
		}
	} else {
		// subtype is a 5 bit field
		FIELD("subtype", ENUM(
					[8, "switch"],
					[16, "intr_discovery"],
					[17, "revision"],
					[18, "unit_id_clump"],
					[19, "extended_config"],
					[20, "address_mapping"],
					[21, "msi_mapping"],
					[22, "direct_route"],
					[23, "vc_set"],
					[24, "retry_mode"],
					[25, "x86_encode"],
					[26, "gen3"],
					[27, "function_extend"],
					[28, "power_mgmt"]),
				BITS("%subcap", 7, 3));

		if (EVAL("subtype") == "switch") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "intr_discovery") {
			//FIXME: not implemented yet
			//procfields: index/data accesses within PCI space
		} else if (EVAL("subtype") == "revision") {
			ht_revision_capability(address);
		} else if (EVAL("subtype") == "unit_id_clump") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "extended_config") {
			ht_extended_config_capability(address);
		} else if (EVAL("subtype") == "address_mapping") {
			ht_address_mapping_capability(address);
		} else if (EVAL("subtype") == "msi_mapping") {
			ht_msi_mapping_capability(address);
		} else if (EVAL("subtype") == "direct_route") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "vc_set") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "retry_mode") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "x86_encode") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "gen3") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "function_extend") {
			//FIXME: not implemented yet
		} else if (EVAL("subtype") == "power_mgmt") {
			//FIXME: not implemented yet
			//procfields: index/data pairs in PCI space
		}
	}
}

ssid_capability(int address)
{
	FIELD("ssvid", "pci_vendor_t", REG16(address + 0x04));
	FIELD("ssid", "hex16_t", REG16(address + 0x06));
}

pcie_capability(int address)
{
	// all PCI-E devices implement this block
	REG16("%pcie_caps", address + 0x02);
	FIELD("version", "int_t", BITS("%pcie_caps", 3, 0));
	FIELD("type", ENUM(
				[0, "endpoint"],
				[1, "legacy_endpoint"],
				[4, "root_port"],
				[5, "upstream_switch_port"],
				[6, "downstream_switch_port"],
				[7, "pcie_pci_bridge"],
				[8, "pci_pcie_bridge"],
				[9, "root_integrated_endpoint"],
				[10, "root_event_collector"]),
			BITS("%pcie_caps", 7, 4));
	if (EVAL("type") == "root_port"
	 || EVAL("type") == "downstream_switch_port") {
		FIELD("slot_impl", "yesno_t", BITS("%pcie_caps", 8));
	}
	FIELD("intr_msg_num", "int_t", BITS("%pcie_caps", 13, 9));
	FIELD("tcs", "yesno_t", BITS("%pcie_caps", 14));

	// common datatypes
	fldfmt pcie_l0s_latency_t = ENUM(
			[0, "ns_64"],
			[1, "ns_128"],
			[2, "ns_256"],
			[3, "ns_512"],
			[4, "us_1"],
			[5, "us_2"],
			[6, "us_4"],
			[7, "us_4_plus"]);
	fldfmt pcie_l1_latency_t = ENUM(
			[0, "us_1"],
			[1, "us_2"],
			[2, "us_4"],
			[3, "us_8"],
			[4, "us_16"],
			[5, "us_32"],
			[6, "us_64"],
			[7, "us_64_plus"]);
	fldfmt pcie_width_t = ENUM(
			[0, "x0"],
			[1, "x1"],
			[2, "x2"],
			[4, "x4"],
			[8, "x8"],
			[12, "x12"],
			[16, "x16"],
			[32, "x32"]);
	fldfmt pcie_link_speed_t = ENUM(
			[0, "GTs_25_only"],
			[1, "GTs_25"],
			[2, "GTs_50"]);


	// all PCI-E devices implement this block
	SCOPE("device", ${
	REG32("%dev_caps", address + 0x04);
	REG16("%dev_control", address + 0x08);
	REG16("%dev_status", address + 0x0a);

	fldfmt pcie_payload_size_t = ENUM(
			[0, "bytes128"],
			[1, "bytes256"],
			[2, "bytes512"],
			[3, "bytes1024"],
			[4, "bytes2048"],
			[5, "bytes4096"]);
	FIELD("max_payload_cap", "pcie_payload_size_t",
			BITS("%dev_caps", 2, 0));
	FIELD("max_payload", "pcie_payload_size_t",
			BITS("%dev_control", 7, 5));
	FIELD("max_read", "pcie_payload_size_t",
			BITS("%dev_control", 14, 12));

	FIELD("phantom_funcs_cap", ENUM(
				[0, "phantom_0"],
				[1, "phantom_4"],
				[2, "phantom_6"],
				[3, "phantom_8"]),
			BITS("%dev_caps", 4, 3));
	FIELD("phantom_funcs", "yesno_t", BITS("%dev_control", 9));

	FIELD("ext_tag_field_cap", ENUM(
				[0, "b5"],
				[1, "b8"]),
			BITS("%dev_caps", 5));
	FIELD("ext_tag_field", "yesno_t", BITS("%dev_control", 8));

	if (EVAL("../type") == "endpoint"
	 || EVAL("../type") == "legacy_endpoint"
	 || EVAL("../type") == "root_integrated_endpoint") {
		FIELD("max_l0s_lat", "pcie_l0s_latency_t",
				BITS("%dev_caps", 8, 6));
		FIELD("max_l1_lat", "pcie_l1_latency_t",
				BITS("%dev_caps", 11, 9));
		FIELD("func_reset_cap", "yesno_t", BITS("%dev_caps", 28));
		FIELD("func_reset", "yesno_t", BITS("%dev_control", 15));
	}

	if (EVAL("../type") == "upstream_switch_port") {
		FIELD("power_limit_value", "int_t", BITS("%dev_caps", 25, 18));
		FIELD("power_limit_scale", ENUM(
					[0, "x1_0"],
					[1, "x0_1"],
					[2, "x0_01"],
					[3, "x0_001"]),
				BITS("%dev_caps", 27, 26));
	}
	FIELD("role_based_err_cap", "yesno_t", BITS("%dev_caps", 15));
	FIELD("corr_err_report", "yesno_t", BITS("%dev_control", 0));
	FIELD("corr_err_det", "yesno_t", BITS("%dev_status", 0));
	FIELD("nonfatal_err_report", "yesno_t", BITS("%dev_control", 1));
	FIELD("nonfatal_err_det", "yesno_t", BITS("%dev_status", 1));
	FIELD("fatal_err_report", "yesno_t", BITS("%dev_control", 2));
	FIELD("fatal_err_det", "yesno_t", BITS("%dev_status", 2));
	FIELD("unsup_req_report", "yesno_t", BITS("%dev_control", 3));
	FIELD("unsup_req_det", "yesno_t", BITS("%dev_status", 3));
	FIELD("relax", "yesno_t", BITS("%dev_control", 4));
	FIELD("aux_pm", "yesno_t", BITS("%dev_control", 10));
	FIELD("aux_pm_det", "yesno_t", BITS("%dev_status", 4));
	FIELD("en_no_snoop", "yesno_t", BITS("%dev_control", 11));
	//FIXME: need langauge support for this
	if (!DEFINED("func_reset") && EVAL("$pci/class") == "bridge") {
		FIELD("bridge_retry_en", "yesno_t", BITS("%dev_control", 15));
	}
	FIELD("txn_pend", "yesno_t", BITS("%dev_status", 5));

	if (FIELD_GE("../version", 2)) {
		REG32("%dev_caps2", address + 0x24);
		REG16("%dev_control2", address + 0x28);
		REG16("%dev_status2", address + 0x2a);

		fldfmt pcie_completion_timeout_t = ENUM(
				[0, "us_50_ms_50"],
				[1, "us_50_ms_10"],
				[2, "ms_10_ms_250"],
				[3, "us_50_ms_250"],
				[6, "ms_10_s_4"],
				[7, "us_50_s_4"],
				[14, "ms_10_s_64"],
				[15, "us_50_s_64"]);

		FIELD("completion_timeout_ranges",
				"pcie_completion_timeout_t",
				BITS("%dev_caps2", 3, 0));
		FIELD("completion_timeout",
				"pcie_completion_timeout_t",
				BITS("%dev_control2", 3, 0));

		FIELD("completion_timeout_disable_en", "yesno_t",
				BITS("%dev_caps2", 4));
		FIELD("completion_timeout_disable", "yesno_t",
				BITS("%dev_control2", 4));
	}

	}); // device

	//
	// not all devices implement the remaining blocks
	//
	if (EVAL("type") == "endpoint"
	 || EVAL("type") == "legacy_endpoint"
	 || EVAL("type") == "root_port"
	 || EVAL("type") == "upstream_switch_port"
	 || EVAL("type") == "downstream_switch_port"
	 || EVAL("type") == "pcie_pci_bridge"
	 || EVAL("type") == "pci_pcie_bridge") {
		SCOPE("link", ${
		REG32("%link_caps", address + 0x0c);
		REG16("%link_control", address + 0x10);
		REG16("%link_status", address + 0x12);

		FIELD("port_number", "int_t", BITS("%link_caps", 31, 24));

		FIELD("supported_speeds", ENUM(
					[1, "GTs_25"],
					[2, "GTs_50_25"]),
				BITS("%link_caps", 3, 0));
		FIELD("speed", "pcie_link_speed_t",
				BITS("%link_status", 3, 0));
		FIELD("max_width", "pcie_width_t", BITS("%link_caps", 9, 4));
		FIELD("width", "pcie_width_t", BITS("%link_status", 9, 4));

		FIELD("aspm_support", BITMASK(
					[0, "l0s"],
					[1, "l1"]),
				BITS("%link_caps", 11, 10));
		FIELD("aspm_ctl", BITMASK(
					[0, "l0s"],
					[1, "l1"]),
				BITS("%link_control", 1, 0));

		FIELD("l0s_exit_latency", "pcie_l0s_latency_t",
				BITS("%link_caps", 14, 12));
		FIELD("l1_exit_latency", "pcie_l1_latency_t",
				BITS("%link_caps", 17, 15));

		FIELD("clock_pm_cap", "yesno_t", BITS("%link_caps", 18));
		FIELD("clock_pm_en", "yesno_t", BITS("%link_control", 8));

		FIELD("surprise_down_err_report_cap", "yesno_t",
				BITS("%link_caps", 19));
		FIELD("data_link_active_report_cap", "yesno_t",
				BITS("%link_caps", 20));
		if (EVAL("data_link_active_report_cap") == TRUE) {
			FIELD("data_link_active", "yesno_t",
					BITS("%link_status", 13));
		}
		FIELD("link_bw_notify_cap", "yesno_t",
				BITS("%link_caps", 21));
		if (EVAL("link_bw_notify_cap") == TRUE) {
			FIELD("link_bw_status", "yesno_t",
					BITS("%link_status", 14));
			FIELD("link_auto_bw_status", "yesno_t",
					BITS("%link_status", 15));
		}

		FIELD("rcb", BOOL("b64", "b128"),
				BITS("%link_control", 3));
		if (EVAL("../type") == "root_port"
		 || EVAL("../type") == "downstream_switch_port"
		 || EVAL("../type") == "pci_pcie_bridge") {
			FIELD("link_disable", "yesno_t",
					BITS("%link_control", 4));
			FIELD("link_retrain", "yesno_t",
					BITS("%link_control", 5));
			FIELD("link_bw_mgmt_intr_en", "yesno_t",
					BITS("%link_control", 10));
			FIELD("link_auto_bw_mgmt_intr_en", "yesno_t",
					BITS("%link_control", 11));
		}
		FIELD("common_clock", "yesno_t", BITS("%link_control", 6));
		FIELD("extended_sync", "yesno_t", BITS("%link_control", 7));
		FIELD("hw_auto_width_dis", "yesno_t",
				BITS("%link_control", 9));

		if (EVAL("../type") == "root_port"
		 || EVAL("../type") == "downstream_switch_port"
		 || EVAL("../type") == "pci_pcie_bridge") {
			FIELD("link_training", "yesno_t",
					BITS("%link_status", 11));
		}
		FIELD("slot_clock", "yesno_t", BITS("%link_status", 12));

		if (FIELD_GE("../version", 2)) {
			REG32("%link_caps2", address + 0x2c);
			REG16("%link_control2", address + 0x30);
			REG16("%link_status2", address + 0x32);

			FIELD("target_link_speed", "pcie_link_speed_t",
					BITS("%link_control2", 3, 0));
			FIELD("enter_compliance", "yesno_t",
					BITS("%link_control2", 4));
			FIELD("hw_auto_speed_dis", "yesno_t",
					BITS("%link_control2", 5));
			FIELD("selecatble_deemphasis",
					BOOL("dB_neg_6_0", "dB_neg_3_5"),
					BITS("%link_control2", 6));
			FIELD("transit_margin", "hex4_t",
					BITS("%link_control2", 9, 7));
			FIELD("enter_mod_compliance", "yesno_t",
					BITS("%link_control2", 10));
			FIELD("compliance_sos", "yesno_t",
					BITS("%link_control2", 11));
			FIELD("compliance_deemphasis",
					BOOL("dB_neg_6_0", "dB_neg_3_5"),
					BITS("%link_control2", 12));

			FIELD("current_deemphasis",
					BOOL("dB_neg_6_0", "dB_neg_3_5"),
					BITS("%link_status2", 0));
		}

		}); // link
	}

	if (DEFINED("slot_impl") && EVAL("slot_impl") == TRUE) {
		SCOPE("slot", ${
		REG32("%slot_caps", address + 0x14);
		REG16("%slot_control", address + 0x18);
		REG16("%slot_status", address + 0x1a);

		FIELD("slot_number", "int_t", BITS("%slot_caps", 31, 19));

		FIELD("attn_button", "yesno_t", BITS("%slot_caps", 0));
		if (EVAL("attn_button") == TRUE) {
			FIELD("attn_button_en", "yesno_t",
					BITS("%slot_control", 0));
			FIELD("attn_button_press", "yesno_t",
					BITS("%slot_status", 0));
		}

		FIELD("power_controller", "yesno_t", BITS("%slot_caps", 1));
		if (EVAL("power_controller") == TRUE) {
			FIELD("power_controller_ctrl",
					BOOL("on", "off"),
					BITS("%slot_control", 10));
			FIELD("power_fault_en", "yesno_t",
					BITS("%slot_control", 1));
			FIELD("power_fault_det", "yesno_t",
					BITS("%slot_status", 1));
		}

		FIELD("mrl_sensor", "yesno_t", BITS("%slot_caps", 2));
		if (EVAL("mrl_sensor") == TRUE) {
			FIELD("mrl_sensor_en", "yesno_t",
					BITS("%slot_control", 2));
			FIELD("mrl_sensor_change", "yesno_t",
					BITS("%slot_status", 2));
			FIELD("mrl_sensor_state",
					BOOL("closed", "open"),
					BITS("%slot_status", 5));
		}

		FIELD("attn_indicator", "yesno_t", BITS("%slot_caps", 3));
		if (EVAL("attn_indicator") == TRUE) {
			FIELD("attn_indicator_ctrl", ENUM(
						[1, "on"],
						[2, "blink"],
						[3, "off"]),
					BITS("%slot_control", 7, 6));
		}

		FIELD("power_indicator", "yesno_t", BITS("%slot_caps", 4));
		if (EVAL("power_indicator") == TRUE) {
			FIELD("power_indicator_ctrl", ENUM(
						[1, "on"],
						[2, "blink"],
						[3, "off"]),
					BITS("%slot_control", 9, 8));
		}

		FIELD("hotplug_capable", "yesno_t", BITS("%slot_caps", 6));
		if (EVAL("hotplug_capable") == TRUE) {
			FIELD("hotplug_surprise", "yesno_t",
					BITS("%slot_caps", 5));
			FIELD("presence_detect_en", "yesno_t",
					BITS("%slot_control", 3));
			FIELD("presence_detect_change", "yesno_t",
					BITS("%slot_status", 3));
			FIELD("presence_detect_state",
					BOOL("empty", "present"),
					BITS("%slot_status", 6));
			FIELD("hot_plug_intr_en", "yesno_t",
					BITS("%slot_control", 5));
			FIELD("no_cmd_complete", "yesno_t",
					BITS("%slot_caps", 18));
			if (EVAL("no_cmd_complete") == TRUE == 0) {
				FIELD("cmd_complete_intr_en",
						"yesno_t", BITS("%slot_control", 4));
				FIELD("cmd_completed", "yesno_t",
						BITS("%slot_status", 4));

			}
		}

		FIELD("electromech_lock", "yesno_t", BITS("%slot_caps", 17));
		if (EVAL("electromech_lock") == TRUE) {
			FIELD("electromech_lock_ctrl", "int_t",
					BITS("%slot_control", 11));
			FIELD("electromech_lock_status",
					BOOL("disengaged", "engaged"),
					BITS("%slot_status", 7));
		}

		//FIXME: better as a procfield?
		FIELD("slot_power_limit", "int_t", BITS("%slot_caps", 14, 7));
		FIELD("slot_power_scale", "int_t", BITS("%slot_caps", 16, 15));

		if (EVAL("../link/data_link_active_report_cap") == TRUE) {
			FIELD("data_link_state_change_en", "yesno_t",
					BITS("%slot_control", 12));
			FIELD("data_link_state_change", "yesno_t",
					BITS("%slot_status", 8));
		}

		if (FIELD_GE("../version", 2)) {
			REG32("%link_caps2", address + 0x34);
			REG16("%link_control2", address + 0x38);
			REG16("%link_status2", address + 0x3a);
		}
		}); // slot
	}

	if (EVAL("type") == "root_event_collector"
	 || EVAL("type") == "root_port") {
		SCOPE("root", ${
		REG16("%root_control", address + 0x1c);
		REG16("%root_caps", address + 0x1e);
		REG32("%root_status", address + 0x20);

		FIELD("serr_on_cerr", "yesno_t", BITS("%root_control", 0));
		FIELD("serr_on_nferr", "yesno_t", BITS("%root_control", 1));
		FIELD("serr_on_ferr", "yesno_t", BITS("%root_control", 2));
		FIELD("pme_intr_en", "yesno_t", BITS("%root_control", 3));

		FIELD("crs_visible_cap", "yesno_t", BITS("%root_caps", 0));
		FIELD("crs_visible_en", "yesno_t", BITS("%root_control", 4));

		FIELD("pme_status", "yesno_t", BITS("%root_status", 16));
		FIELD("pme_pending", "yesno_t", BITS("%root_status", 17));
		FIELD("pme_requester", "hex16_t", BITS("%root_status", 15, 0));

		}); // root
	}
}

aer_ecapability(int address)
{
	BOOL("fatal_t", "fatal", "nonfatal");

	// uncorrectable errors
	SCOPE("uncorrectable", ${

	REG32("%error_status", address + 0x4);
	REG32("%error_mask", address + 0x8);
	REG32("%error_severity", address + 0xc);

	SCOPE("data_link_proto_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 4));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 4));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 4));
	});

	SCOPE("surprise_down_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 5));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 5));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 5));
	});

	SCOPE("poisoned_tlp", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 12));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 12));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 12));
	});

	SCOPE("flow_ctrl_proto_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 13));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 13));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 13));
	});

	SCOPE("completion_timeout", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 14));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 14));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 14));
	});

	SCOPE("completer_abort", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 15));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 15));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 15));
	});

	SCOPE("unexpected_completion", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 16));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 16));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 16));
	});

	SCOPE("receiver_overflow", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 17));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 17));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 17));
	});

	SCOPE("malformed_tlp", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 18));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 18));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 18));
	});

	SCOPE("ecrc_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 19));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 19));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 19));
	});

	SCOPE("unsupported_request_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 20));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 20));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 20));
	});

	SCOPE("acs_violation", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 21));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 21));
	FIELD("severity", "fatal_t", BITS("../%error_severity", 21));
	});

	}); // uncorrectable

	// correctable errors
	SCOPE("correctable", ${

	REG32("%error_status", address + 0x10);
	REG32("%error_mask", address + 0x14);

	SCOPE("receiver_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 0));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 0));
	});

	SCOPE("bad_tlp", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 6));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 6));
	});

	SCOPE("bad_dllp", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 7));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 7));
	});

	SCOPE("replay_num_rollover", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 8));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 8));
	});

	SCOPE("replay_timer_timeout", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 12));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 12));
	});

	SCOPE("advisory_nf_err", ${
	FIELD("status", "yesno_t", BITS("../%error_status", 21));
	FIELD("masked", "yesno_t", BITS("../%error_mask", 13));
	});

	}); // correctable

	// capabilities and control
	REG32("%caps_control", address + 0x18);
	FIELD("first_err_ptr", "hex8_t",
			BITS("%caps_control", 4, 0));
	FIELD("ecrc_generation_cap", "yesno_t",
			BITS("%caps_control", 5));
	FIELD("ecrc_generation_en", "yesno_t",
			BITS("%caps_control", 6));
	FIELD("ecrc_check_cap", "yesno_t",
			BITS("%caps_control", 7));
	FIELD("ecrc_check_en", "yesno_t",
			BITS("%caps_control", 8));

	// header log
	REG32("%header_log_0", address + 0x1c);
	REG32("%header_log_1", address + 0x20);
	REG32("%header_log_2", address + 0x24);
	REG32("%header_log_3", address + 0x28);
	FIELD("header_log", "hex128_t",
			BITS("%header_log_3", 31, 0) +
			BITS("%header_log_2", 31, 0) +
			BITS("%header_log_1", 31, 0) +
			BITS("%header_log_0", 31, 0));

	// if this device is a root port or root complex event collector
	if (EVAL("$pci/capability.pcie[0]/type") == "root_event_collector"
	 || EVAL("$pci/capability.pcie[0]/type") == "root_port") {
		// root error command and status
		REG32("%root_error_command", address + 0x2c);
		REG32("%root_error_status", address + 0x30);

		FIELD("correctable_err_en", "yesno_t",
				BITS("%root_error_command", 0));
		FIELD("correctable_err_rcvd", "yesno_t",
				BITS("%root_error_status", 0));
		FIELD("correctable_err_multi", "yesno_t",
				BITS("%root_error_status", 1));

		FIELD("nonfatal_err_en", "yesno_t",
				BITS("%root_error_command", 1));
		FIELD("nonfatal_rcvd", "yesno_t",
				BITS("%root_error_status", 5));

		FIELD("fatal_err_en", "yesno_t",
				BITS("%root_error_command", 2));
		FIELD("fatal_rcvd", "yesno_t",
				BITS("%root_error_status", 6));

		FIELD("uncorrectable_err_rcvd", "yesno_t",
				BITS("%root_error_status", 2));
		FIELD("uncorrectable_err_multi", "yesno_t",
				BITS("%root_error_status", 3));
		FIELD("first_uncorrectable", "fatal_t",
				BITS("%root_error_status", 4));

		FIELD("intr_msg_num", "hex8_t",
				BITS("%root_error_status", 31, 27));

		REG32("%error_src_id", address + 0x34);
		FIELD("correctable_src_id", "hex16_t",
				BITS("%error_src_id", 15, 0));
		FIELD("uncorrectable_src_id", "hex16_t",
				BITS("%error_src_id", 31, 16));
	}
}

// Handle the PCI capabilities linked-list.
explore_capabilities()
{
	// Walk the PCI capabilities linked-list.
	if (EVAL("status/caps") == TRUE) {
		FIELD("capptr", "hex8_t", REG8(0x34));

		int ptr = READ("capptr");
		while (ptr != 0 && ptr != 0xff) {
			SCOPE("capability[]", ${

			FIELD("offset", "hex8_t", ptr);
			FIELD("id", "pci_capability_t", REG8(ptr));
			FIELD("next", "hex8_t", REG8(ptr+1));

			if (EVAL("id") == "power_mgmt") {
				// PCI spec
				power_mgmt_capability(ptr);
			} else if (EVAL("id") == "agp") {
				// PCI spec
				//FIXME: not implemented yet
			} else if (EVAL("id") == "agp8x") {
				// PCI spec
				//FIXME: not implemented yet
			} else if (EVAL("id") == "vpd") {
				// PCI spec
				//FIXME: not implemented yet
			} else if (EVAL("id") == "slot_id") {
				// PCI-PCI bridge spec
				slot_id_capability(ptr);
			} else if (EVAL("id") == "msi") {
				// PCI spec
				msi_capability(ptr);
			} else if (EVAL("id") == "msix") {
				// PCI spec
				msix_capability(ptr);
			} else if (EVAL("id") == "resource_ctrl") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "hot_swap") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "hot_plug") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "ht") {
				// HT spec
				ht_capability(ptr);
			} else if (EVAL("id") == "usb2_dbg_port") {
				// EHCI spec
				//FIXME: not implemented yet
			} else if (EVAL("id") == "ssid") {
				// PCI-PCI bridge spec
				ssid_capability(ptr);
			} else if (EVAL("id") == "pcix") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "pcie") {
				// PCI-E spec
				pcie_capability(ptr);
			} else if (EVAL("id") == "vendor") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "secure") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "sata") {
				//FIXME: not implemented yet, what spec??
			} else if (EVAL("id") == "af") {
				//FIXME: not implemented yet, ECN
			} else if (EVAL("id") == 0) {
				// This should not be needed, except that
				// some hardware vendors make the
				// capability pointer link to a
				// capability block with id=0 and next=0.
			} else {
				WARN("unknown PCI capability 0x%02x @ 0x%02x",
						READ("id"), ptr);
			}

			ptr = READ("next");
			});
			//FIXME: sprintf?
			//ALIAS("capability."
			      //+GET_FIELD("capability[-1]/id")->evaluate()+"[]",
			      //"capability[-1]");
			//FIXME: need evaluate to return a valid path_element, or be sanitized
		}
	}

	// Only PCI-X and PCI-Express devices should have extended
	// capabilities.  Those devices self identify through the
	// existence of a traditional PCI capability block.  If we have a
	// non-zero capability header, walk the extended capabilities
	// linked-list.
	// FIXME: need RCRB support
	if ((DEFINED("$pci/capability.pcie") || DEFINED("$pci/capability.pcix"))
	 && DEFINED("$pci/%PCI.100") && (READ("$pci/%PCI.100") != 0)) {
		int ptr = 0x100;
		while (ptr != 0 && ptr != 0xfff) {
			SCOPE("ecapability[]", ${

			FIELD("offset", "hex16_t", ptr);
			FIELD("id", "pcie_capability_t", REG16(ptr));
			REG16("%ver_next", ptr+2);
			FIELD("version", "hex4_t", BITS("%ver_next", 3, 0));
			FIELD("next", "hex12_t", BITS("%ver_next", 15, 4));

			if (EVAL("id") == "aer") {
				// PCI-E spec
				aer_ecapability(ptr);
			} else if (EVAL("id") == "vchannel") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "serial_num") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "power_budget") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "root_link_decl") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "root_internal_link_ctrl") {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id",
				   "root_event_collector_endpoint_assoc")) {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "multi_func_vchannel") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "vc2") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "rcrb_header") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "vendor_specific") {
				//FIXME: not implemented yet
			} else if (EVAL("id") == "acs") {
				//FIXME: not implemented yet
			} else if (FIELD_EQ("id", 0)) {
				// This should not be needed, except that
				// some hardware vendors make the
				// capability pointer link to a
				// capability block with id=0 and next=0.
			} else {
				WARN("unknown PCIE capability 0x%04x @ 0x%03x",
						READ("id"), ptr);
			}

			ptr = READ("next");
			});
			//FIXME: sprintf?
			//ALIAS("ecapability."
			      //+GET_FIELD("ecapability[-1]/id")->evaluate()+"[]",
			      //"ecapability[-1]");
			//FIXME: need evaluate to return a valid path_element, or be sanitized
		}
	}
	//FIXME: scan EHCI extended capabilities
}

create_pci_bridge()
{
	FIELD("pri_bus", "int_t", REG8(0x18));
	FIELD("sec_bus", "int_t", REG8(0x19));
	FIELD("sub_bus", "int_t", REG8(0x1a));
	FIELD("sec_latency", INT("clocks"), REG8(0x1b));

	// Secondary status
	REG16("%sec_status", 0x1e);
	SCOPE("sec_status", ${
		FIELD("cap66", "yesno_t", BITS("../%sec_status", 5));
		FIELD("fbb", "yesno_t", BITS("../%sec_status", 7));
		FIELD("mdperr", "yesno_t", BITS("../%sec_status", 8));
		FIELD("devsel", ENUM(
					[0, "FAST"],
					[1, "MEDIUM"],
					[2, "SLOW"]),
			 BITS("../%sec_status", 10, 9));
		FIELD("sigtabort", "yesno_t", BITS("../%sec_status", 11));
		FIELD("rcvtabort", "yesno_t", BITS("../%sec_status", 12));
		FIELD("rcvmabort", "yesno_t", BITS("../%sec_status", 13));
		FIELD("rcvserr", "yesno_t", BITS("../%sec_status", 14));
		FIELD("perr", "yesno_t", BITS("../%sec_status", 15));
	});

	// IO decode window
	SCOPE("io_window", ${
		REG8("%base_lo", 0x1c);
		REG16("%base_hi", 0x30);
		REG8("%limit_lo", 0x1d);
		REG16("%limit_hi", 0x32);

		FIELD("width", ENUM(
					[0, "bits16"],
					[1, "bits32"]),
				BITS("%base_lo", 3, 0));

		if (EVAL("width") == "bits16") {
			FIELD("base", "addr16_t",
					BITS("%base_lo", 7, 4) +
					BITS("%0", 11, 0));
			FIELD("limit", "addr16_t",
					BITS("%limit_lo", 7, 4) +
					BITS("%1", 11, 0));
		} else if (EVAL("width") == "bits32") {
			FIELD("base", "addr32_t",
					BITS("%base_hi", 15, 0) +
					BITS("%base_lo", 7, 4) +
					BITS("%0", 11, 0));
			FIELD("limit", "addr32_t",
					BITS("%limit_hi", 15, 0) +
					BITS("%limit_lo", 7, 4) +
					BITS("%1", 11, 0));
		}
	});

	// MMIO decode window
	SCOPE("mem_window", ${
		REG16("%base", 0x20);
		REG16("%limit", 0x22);

		FIELD("base", "addr32_t",
				BITS("%base", 15, 4) +
				BITS("%0", 19, 0));
		FIELD("limit", "addr32_t",
				BITS("%limit", 15, 4) +
				BITS("%1", 19, 0));
	});

	// Prefetched memory decode window
	SCOPE("prefetch_window", ${
		REG16("%base_lo", 0x24);
		REG32("%base_hi", 0x28);
		REG16("%limit_lo", 0x26);
		REG32("%limit_hi", 0x2c);

		FIELD("width", ENUM(
					[0, "bits32"],
					[1, "bits64"]),
				BITS("%base_lo", 3, 0));

		if (EVAL("width") == "bits32") {
			FIELD("base", "addr32_t",
					BITS("%base_lo", 15, 4) +
					BITS("%0", 19, 0));
			FIELD("limit", "addr32_t",
					BITS("%limit_lo", 15, 4) +
					BITS("%1", 19, 0));
		} else if (EVAL("width") == "bits64") {
			FIELD("base", "addr64_t",
					BITS("%base_hi", 31, 0) +
					BITS("%base_lo", 15, 4) +
					BITS("%0", 19, 0));
			FIELD("limit", "addr64_t",
					BITS("%limit_hi", 31, 0) +
					BITS("%limit_lo", 15, 4) +
					BITS("%1", 19, 0));
		}
	});

	// Capabilities
	explore_capabilities();

	// Expansion ROM
	REG32("%rombase", 0x38);
	SCOPE("rombase", ${
		FIELD("enabled", "yesno_t", BITS("../%rombase", 0));
		FIELD("address", "addr32_t",
				BITS("../%rombase", 31, 11) +
				BITS("%0", 10, 0));
	});

	// Bridge control
	REG16("%bridge_ctrl", 0x3e);
	SCOPE("bridge_ctrl", ${
		BOOL("discard_timer_t", "short", "long");
		FIELD("perr", "yesno_t", BITS("../%bridge_ctrl", 0));
		FIELD("serr", "yesno_t", BITS("../%bridge_ctrl", 1));
		FIELD("isa", "yesno_t", BITS("../%bridge_ctrl", 2));
		FIELD("vga", "yesno_t", BITS("../%bridge_ctrl", 3));
		FIELD("vga16", "yesno_t", BITS("../%bridge_ctrl", 4));
		FIELD("mst_abort", "yesno_t", BITS("../%bridge_ctrl", 5));
		FIELD("sec_reset", "yesno_t", BITS("../%bridge_ctrl", 6));
		FIELD("fbb", "yesno_t", BITS("../%bridge_ctrl", 7));
		FIELD("pri_discard", "discard_timer_t",
		    BITS("../%bridge_ctrl", 8));
		FIELD("sec_discard", "discard_timer_t",
		    BITS("../%bridge_ctrl", 9));
		FIELD("discard_status", "yesno_t", BITS("../%bridge_ctrl", 10));
		FIELD("discard_serr", "yesno_t", BITS("../%bridge_ctrl", 11));
	});
}

create_device()
{
	// if BAR1 is not defined, BAR0 must be 64 bit, BAR2 is ok.
	// if BAR1 is not 64 bit memory, BAR2 is ok.
	if (!DEFINED("bar1")
	 || !(EVAL("bar1/type") == "mem"
	   && EVAL("bar1/width") == "bits64")) {
		BAR("bar2", 0x18);
	}

	// if BAR2 is not defined, BAR1 must be 64 bit, BAR3 is ok.
	// if BAR2 is not 64 bit memory, BAR3 is ok.
	if (!DEFINED("bar2")
	 || !(EVAL("bar2/type") == "mem"
	   && EVAL("bar2/width") == "bits64")) {
		BAR("bar3", 0x1c);
	}

	// if BAR3 is not defined, BAR2 must be 64 bit, BAR4 is ok.
	// if BAR3 is not 64 bit memory, BAR4 is ok.
	if (!DEFINED("bar3")
	 || !(EVAL("bar3/type") == "mem"
	   && EVAL("bar3/width") == "bits64")) {
		BAR("bar4", 0x20);
	}

	// if BAR4 is not defined, BAR3 must be 64 bit, BAR5 is ok.
	// if BAR4 is not 64 bit memory, BAR5 is ok.
	if (!DEFINED("bar4")
	 || !(EVAL("bar4/type") == "mem"
	   && EVAL("bar4/width") == "bits64")) {
		BAR("bar5", 0x24);
	}

	FIELD("cisptr", "addr32_t", REG32(0x28));

	// Subsystem IDs
	FIELD("subvendor", "pci_vendor_t", REG16(0x2c));
	FIELD("subsystem", "hex16_t", REG16(0x2e));

	// Capabilities
	explore_capabilities();

	// Expansion ROM
	REG32("%rombase", 0x30);
	SCOPE("rombase", ${
		FIELD("enabled", "yesno_t", BITS("../%rombase", 0));
		FIELD("address", "addr32_t",
				BITS("../%rombase", 31, 11) +
				BITS("%0", 10, 0));
	});

	FIELD("mingnt", INT("1/4 usecs"), REG8(0x3e));
	FIELD("maxlat", INT("1/4 usecs"), REG8(0x3f));
}

// populate the current scope with generic PCI device fields
pci_generic_device()
{
	BOOKMARK("pci");

	// all PCI devices have a 256 Byte config space
	for (int i = 0; i < 256; i += 4) {
		REG32(sprintf("%%PCI.%03x", i), i);
	}
	// if 0x100 is not FFFFFFFF, we have a 4 KByte config space
	if (READ(REG32(0x100)) != 0xffffffff) {
		for (int i = 256; i < 4096; i += 4) {
			REG32(to_string("%%PCI.%03x", i), i);
		}
	}

	FIELD("vendor", "pci_vendor_t", REG16(0x00));
	FIELD("device", "hex16_t", REG16(0x02));

	REG16("%command", 0x04);
	SCOPE("command", ${
		FIELD("io", "yesno_t", BITS("../%command", 0));
		FIELD("mem", "yesno_t", BITS("../%command", 1));
		FIELD("bm", "yesno_t", BITS("../%command", 2));
		FIELD("special", "yesno_t", BITS("../%command", 3));
		FIELD("mwinv", "yesno_t", BITS("../%command", 4));
		FIELD("vgasnoop", "yesno_t", BITS("../%command", 5));
		FIELD("perr", "yesno_t", BITS("../%command", 6));
		FIELD("step", "yesno_t", BITS("../%command", 7));
		FIELD("serr", "yesno_t", BITS("../%command", 8));
		FIELD("fbb", "yesno_t", BITS("../%command", 9));
		FIELD("intr", "yesno_t", BITS("../%command", 10));
	});

	REG16("%status", 0x06);
	SCOPE("status", ${
		FIELD("intr", "yesno_t", BITS("../%status", 3));
		FIELD("caps", "yesno_t", BITS("../%status", 4));
		FIELD("cap66", "yesno_t", BITS("../%status", 5));
		FIELD("user", "yesno_t", BITS("../%status", 6));
		FIELD("fbb", "yesno_t", BITS("../%status", 7));
		FIELD("mdperr", "yesno_t", BITS("../%status", 8));
		FIELD("devsel", ENUM(
					[0, "FAST"],
					[1, "MEDIUM"],
					[2, "SLOW"]),
			 BITS("../%status", 10, 9));
		FIELD("sigtabort", "yesno_t", BITS("../%status", 11));
		FIELD("rcvtabort", "yesno_t", BITS("../%status", 12));
		FIELD("rcvmabort", "yesno_t", BITS("../%status", 13));
		FIELD("sigserr", "yesno_t", BITS("../%status", 14));
		FIELD("perr", "yesno_t", BITS("../%status", 15));
	});

	FIELD("class", "pci_class_t", REG8(0x0b));

	//FIXME: figure the best way to use the types in pci.c
	if (EVAL("class") == "pre_classcode") {
		FIELD("subclass", "pci_subclass_pre_classcode_t", REG8(0x0a));
	} else if (EVAL("class") == "mass_storage") {
		FIELD("subclass", "pci_subclass_mass_storage_t", REG8(0x0a));
	} else if (EVAL("class") == "network") {
		FIELD("subclass", "pci_subclass_network_t", REG8(0x0a));
	} else if (EVAL("class") == "display") {
		FIELD("subclass", "pci_subclass_display_t", REG8(0x0a));
	} else if (EVAL("class") == "multimedia") {
		FIELD("subclass", "pci_subclass_multimedia_t", REG8(0x0a));
	} else if (EVAL("class") == "memory") {
		FIELD("subclass", "pci_subclass_memory_t", REG8(0x0a));
	} else if (EVAL("class") == "bridge") {
		FIELD("subclass", "pci_subclass_bridge_t", REG8(0x0a));
	//FIXME: not implemented yet
	//} else if (EVAL("class") == "simple_comm") {
		//FIELD("subclass", "pci_subclass_simple_comm", REG8(0x0a));
	//} else if (EVAL("class") == "base_system") {
		//FIELD("subclass", "pci_subclass_base_system", REG8(0x0a));
	//} else if (EVAL("class") == "input") {
		//FIELD("subclass", "pci_subclass_input", REG8(0x0a));
	//} else if (EVAL("class") == "docking") {
		//FIELD("subclass", "pci_subclass_docking", REG8(0x0a));
	//} else if (EVAL("class") == "processor") {
		//FIELD("subclass", "pci_subclass_processor", REG8(0x0a));
	//} else if (EVAL("class") == "serial") {
		//FIELD("subclass", "pci_subclass_serial", REG8(0x0a));
	//} else if (EVAL("class") == "wireless") {
		//FIELD("subclass", "pci_subclass_wireless", REG8(0x0a));
	//} else if (EVAL("class") == "intelligent_io") {
		//FIELD("subclass", "pci_subclass_intelligent_io", REG8(0x0a));
	//} else if (EVAL("class") == "satellite") {
		//FIELD("subclass", "pci_subclass_satellite", REG8(0x0a));
	//} else if (EVAL("class") == "crypto") {
		//FIELD("subclass", "pci_subclass_crypto", REG8(0x0a));
	//} else if (EVAL("class") == "dsp") {
		//FIELD("subclass", "pci_subclass_dsp", REG8(0x0a));
	} else {
		FIELD("subclass", "hex8_t", REG8(0x0a));
	}

	//FIXME: it would be better to have detailed progintfs
	FIELD("progintf", "hex8_t", REG8(0x09));

	FIELD("revision", "hex8_t", REG8(0x08));

	FIELD("cacheline", INT("DWORDs"), REG8(0x0c));

	REG8("%hdrtype", 0x0e);
	FIELD("hdrtype", ENUM(
				[0, "device"],
				[1, "pci_bridge"],
				[2, "cardbus_bridge"]),
			BITS("%hdrtype", 6, 0));
	FIELD("multifunc", "yesno_t", BITS("%hdrtype", 7));

	REG8("%bist", 0x0f);
	SCOPE("bist", ${
		FIELD("capable", "yesno_t", BITS("../%bist", 7));
		FIELD("start", "yesno_t", BITS("../%bist", 6));
		FIELD("code", "hex8_t", BITS("../%bist", 3, 0));
	});

	FIELD("intline", "int_t", REG8(0x3c));

	FIELD("intpin", ENUM(
				[0, "none"],
				[1, "inta"],
				[2, "intb"],
				[3, "intc"],
				[4, "intd"]),
			REG8(0x3d));

	// Base Address Registers - all devices have at least 2 BARs.
	BAR("bar0", 0x10);

	// if BAR0 is not 64 bit memory, declare BAR1
	if (!(EVAL("bar0/type") == "mem"
	   && EVAL("bar0/width") == "bits64")) {
		BAR("bar1", 0x14);
	}

	if (EVAL("hdrtype") == "device") {
		create_device();
	} else if (EVAL("hdrtype") == "pci_bridge") {
		create_pci_bridge();
	} else if (EVAL("hdrtype") == "cardbus_bridge") {
		//TODO: need a spec
	}
}

//NOTE: the func arg is arg-checked at build time
public
PCI_SCOPE(string name, int seg, int bus, int dev, int fn, func tail_fn)
{
	SCOPE(name, BIND("pci", seg, bus, dev, fn), ${
		pci_generic_device();
		tail_fn();
	});
}

// Example code in compiled-to-C++ form
// for (int i = 0; i < 256; i += 4) {
//    ...
// }
//
// {
// 	symtab->new_context();
// 	Variable i(Value(0));
// 	symtab->Add("i", &i);
// 
// 	for (; i.int_value() < 256; i.int_value()++) {
// 		...
// 	}
// 
// 	symtab->pop_context();
// }
