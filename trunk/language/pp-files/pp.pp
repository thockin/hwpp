// primitives
public type int_t = INT();
public type hex_t = HEX();
public type hex4_t = HEX(BITS4);
public type hex8_t = HEX(BITS8);
public type hex12_t = HEX(BITS12);
public type hex16_t = HEX(BITS16);
public type hex20_t = HEX(BITS20);
public type hex32_t = HEX(BITS32);
public type hex64_t = HEX(BITS64);
public type hex128_t = HEX(BITS128);
public type addr16_t = HEX(BITS16);
public type addr32_t = HEX(BITS32);
public type addr64_t = HEX(BITS64);
public type string_t = STRING();

// various booleans
public type yesno_t = BOOL("yes", "no");
public type truefalse_t = BOOL("true", "false");
public type onoff_t = BOOL("on", "off");
public type enabledisable_t = BOOL("enabled", "disabled");
public type disableenable_t = BOOL("disabled", "enabled");

// generic bitmask
public type bitmask_t = BITMASK(
		  [0, "bit0"],     [1, "bit1"],
		  [2, "bit2"],     [3, "bit3"],
		  [4, "bit4"],     [5, "bit5"],
		  [6, "bit6"],     [7, "bit7"],
		  [8, "bit8"],     [9, "bit9"],
		 [10, "bit10"],   [11, "bit11"],
		 [12, "bit12"],   [13, "bit13"],
		 [14, "bit14"],   [15, "bit15"],
		 [16, "bit16"],   [17, "bit17"],
		 [18, "bit18"],   [19, "bit19"],
		 [20, "bit20"],   [21, "bit21"],
		 [22, "bit22"],   [23, "bit23"],
		 [24, "bit24"],   [25, "bit25"],
		 [26, "bit26"],   [27, "bit27"],
		 [28, "bit28"],   [29, "bit29"],
		 [30, "bit30"],   [31, "bit31"],
		 [32, "bit32"],   [33, "bit33"],
		 [34, "bit34"],   [35, "bit35"],
		 [36, "bit36"],   [37, "bit37"],
		 [38, "bit38"],   [39, "bit39"],
		 [40, "bit40"],   [41, "bit41"],
		 [42, "bit42"],   [43, "bit43"],
		 [44, "bit44"],   [45, "bit45"],
		 [46, "bit46"],   [47, "bit47"],
		 [48, "bit48"],   [49, "bit49"],
		 [50, "bit50"],   [51, "bit51"],
		 [52, "bit52"],   [53, "bit53"],
		 [54, "bit54"],   [55, "bit55"],
		 [56, "bit56"],   [57, "bit57"],
		 [58, "bit58"],   [59, "bit59"],
		 [60, "bit60"],   [61, "bit61"],
		 [62, "bit62"],   [63, "bit63"],
		 [64, "bit64"],   [65, "bit65"],
		 [66, "bit66"],   [67, "bit67"],
		 [68, "bit68"],   [69, "bit69"],
		 [70, "bit70"],   [71, "bit71"],
		 [72, "bit72"],   [73, "bit73"],
		 [74, "bit74"],   [75, "bit75"],
		 [76, "bit76"],   [77, "bit77"],
		 [78, "bit78"],   [79, "bit79"],
		 [80, "bit80"],   [81, "bit81"],
		 [82, "bit82"],   [83, "bit83"],
		 [84, "bit84"],   [85, "bit85"],
		 [86, "bit86"],   [87, "bit87"],
		 [88, "bit88"],   [89, "bit89"],
		 [90, "bit90"],   [91, "bit91"],
		 [92, "bit92"],   [93, "bit93"],
		 [94, "bit94"],   [95, "bit95"],
		 [96, "bit96"],   [97, "bit97"],
		 [98, "bit98"],   [99, "bit99"],
		[100, "bit100"], [101, "bit101"],
		[102, "bit102"], [103, "bit103"],
		[104, "bit104"], [105, "bit105"],
		[106, "bit106"], [107, "bit107"],
		[108, "bit108"], [109, "bit109"],
		[110, "bit110"], [111, "bit111"],
		[112, "bit112"], [113, "bit113"],
		[114, "bit114"], [115, "bit115"],
		[116, "bit116"], [117, "bit117"],
		[118, "bit118"], [119, "bit119"],
		[120, "bit120"], [121, "bit121"],
		[122, "bit122"], [123, "bit123"],
		[124, "bit124"], [125, "bit125"],
		[126, "bit126"], [127, "bit127"]);
