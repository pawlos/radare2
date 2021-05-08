
/* radare - LGPL - Copyright 2021 - Siguza, pancake */

// Context: https://raw.githubusercontent.com/Siguza/misc/master/xref.c

#include <r_core.h>

static void siguza_xrefs(RCore *core, ut64 search, bool all) {
	const ut8 *mem = core->block;
	const size_t fs = core->blocksize;
	ut64 addr = core->offset;
	ut32 *p = (ut32*)((uint8_t*)mem);
	ut32 *e = (ut32*)(p + (fs / 4));
	for (; p < e; p++, addr += 4) {
		ut32 v = *p;
		if((v & 0x1f000000) == 0x10000000) // adr and adrp
		{
			ut32 reg = v & 0x1f;
			bool is_adrp = (v & 0x80000000) != 0;
			int64_t base = is_adrp ? (addr & 0xfffffffffffff000) : addr;
			int64_t off  = ((int64_t)((((v >> 5) & 0x7ffff) << 2) | ((v >> 29) & 0x3)) << 43) >> (is_adrp ? 31 : 43);
			ut64 target = base + off;
			if (search == 0) {
				r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x" # %s\n", addr, target, is_adrp? "adrp": "adr");
			} else if (target == search) {
				r_cons_printf ("%#"PFMT64x": %s x%u, %#"PFMT64x"\n", addr, is_adrp ? "adrp" : "adr", reg, target);
			}
			// More complicated cases - up to 3 instr
			else
			{
				ut32 *q = p + 1;
				while(q < e && *q == 0xd503201f) // nop
				{
					q++;
				}
				if(q < e) {
					v = *q;
					ut32 reg2 = reg;
					ut32 aoff = 0;
					bool found = false;
					if((v & 0xff8003e0) == (0x91000000 | (reg << 5))) // 64bit add, match reg
					{
						reg2 = v & 0x1f;
						aoff = (v >> 10) & 0xfff;
						if (v & 0x400000) {
							aoff <<= 12;
						}
						if (search == 0) {
							r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x"\n", addr, target + aoff);
						} else if(target + aoff == search) {
							r_cons_printf ("%#"PFMT64x": %s x%u, %#"PFMT64x"; add x%u, x%u, %#x\n", addr, is_adrp ? "adrp" : "adr", reg, target, reg2, reg, aoff);
							found = true;
						} else {
							do {
								q++;
							} while (q < e && *q == 0xd503201f); // nop
						}
					}
					if(!found && q < e) {
						v = *q;
						if((v & 0xff8003e0) == (0x91000000 | (reg2 << 5))) // 64bit add, match reg
						{
							ut32 xoff = (v >> 10) & 0xfff;
							if (v & 0x400000) {
								xoff <<= 12;
							}
							if (search == 0) {
								r_cons_printf("ax 0x%"PFMT64x" 0x%#"PFMT64x" # add\n", addr, target);
							} else if (target + aoff + xoff == search) {
								// If we get here, we know the previous add matched
								r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; add x%u, x%u, %#x; add x%u, x%u, %#x\n", addr, is_adrp ? "adrp" : "adr", reg, target, reg2, reg, aoff, v & 0x1f, reg2, xoff);
							}
						} else if((v & 0x3e0003e0) == (0x38000000 | (reg2 << 5))) // all of str[hb]/ldr[hb], match reg
						{
							const char *inst = NULL;
							uint8_t size;
							size = (v >> 30) & 0x3;
							uint8_t opc = (v >> 22) & 0x3;
							switch((opc << 4) | size)
							{
								case 0x00:            inst = "strb";  break;
								case 0x01:            inst = "strh";  break;
								case 0x02: case 0x03: inst = "str";   break;
								case 0x10:            inst = "ldrb";  break;
								case 0x11:            inst = "ldrh";  break;
								case 0x12: case 0x13: inst = "ldr";   break;
								case 0x20: case 0x30: inst = "ldrsb"; break;
								case 0x21: case 0x31: inst = "ldrsh"; break;
								case 0x22:            inst = "ldrsw"; break;
							}
							if(inst)
							{
								uint8_t regsize = opc == 2 && size < 2 ? 3 : size;
								const char *rs = regsize == 3 ? "x" : "w";
								if((v & 0x1000000) != 0) // unsigned offset
								{
									ut64 uoff = ((v >> 10) & 0xfff) << size;
									if (!search) {
										r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x" # add\n",
											addr, target+aoff+uoff);
									} else if (target + aoff + uoff == search) {
										if (aoff) // Have add
										{
											r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; add x%u, x%u, %#x; %s %s%u, [x%u, %#"PFMT64x"]\n", addr, is_adrp ? "adrp" : "adr", reg, target, reg2, reg, aoff, inst, rs, v & 0x1f, reg2, uoff);
										}
										else // Have no add
										{
											r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; %s %s%u, [x%u, %#"PFMT64x"]\n", addr, is_adrp ? "adrp" : "adr", reg, target, inst, rs, v & 0x1f, reg2, uoff);
										}
									}
								} else if((v & 0x00200000) == 0) {
									int64_t soff = ((int64_t)((v >> 12) & 0x1ff) << 55) >> 55;
									const char *sign = soff < 0 ? "-" : "";
									if (!search) {
										ut64 dst = target + aoff + soff;
										r_cons_printf("ax 0x%"PFMT64x" 0x%"PFMT64x" # add/adrp\n", addr, dst);
									} else if(target + aoff + soff == search) {
										if((v & 0x400) == 0) {
											if((v & 0x800) == 0) // unscaled
											{
												switch((opc << 4) | size)
												{
													case 0x00:            inst = "sturb";  break;
													case 0x01:            inst = "sturh";  break;
													case 0x02: case 0x03: inst = "stur";   break;
													case 0x10:            inst = "ldurb";  break;
													case 0x11:            inst = "ldurh";  break;
													case 0x12: case 0x13: inst = "ldur";   break;
													case 0x20: case 0x30: inst = "ldursb"; break;
													case 0x21: case 0x31: inst = "ldursh"; break;
													case 0x22:            inst = "ldursw"; break;
												}
											}
											else // unprivileged
											{
												switch((opc << 4) | size)
												{
													case 0x00:            inst = "sttrb";  break;
													case 0x01:            inst = "sttrh";  break;
													case 0x02: case 0x03: inst = "sttr";   break;
													case 0x10:            inst = "ldtrb";  break;
													case 0x11:            inst = "ldtrh";  break;
													case 0x12: case 0x13: inst = "ldtr";   break;
													case 0x20: case 0x30: inst = "ldtrsb"; break;
													case 0x21: case 0x31: inst = "ldtrsh"; break;
													case 0x22:            inst = "ldtrsw"; break;
												}
											}
											if(aoff) // Have add
											{
												r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; add x%u, x%u, %#x; %s %s%u, [x%u, %s%"PFMT64d"]\n",
													addr, is_adrp ? "adrp" : "adr", reg, target, reg2, reg, aoff, inst, rs, v & 0x1f, reg2, sign, (st64)soff);
											}
											else // Have no add
											{
												r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; %s %s%u, [x%u, %s%"PFMT64d"]\n", addr, is_adrp ? "adrp" : "adr", reg, target, inst, rs, v & 0x1f, reg2, sign, (st64)soff);
											}
										}
										else // pre/post-index
										{
											if((v & 0x800) != 0) // pre
											{
												if(aoff) // Have add
												{
													r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; add x%u, x%u, %#x; %s %s%u, [x%u, %s%"PFMT64d"]!\n", addr, is_adrp ? "adrp" : "adr", reg, target, reg2, reg, aoff, inst, rs, v & 0x1f, reg2, sign, (st64)soff);
												}
												else // Have no add
												{
													r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; %s %s%u, [x%u, %s%"PFMT64d"]!\n", addr, is_adrp ? "adrp" : "adr", reg, target, inst, rs, v & 0x1f, reg2, sign, (st64)soff);
												}
											}
											else // post
											{
												if(aoff) // Have add
												{
													r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; add x%u, x%u, %#x; %s %s%u, [x%u], %s%"PFMT64d"\n", addr, is_adrp ? "adrp" : "adr", reg, target, reg2, reg, aoff, inst, rs, v & 0x1f, reg2, sign, (st64)soff);
												}
												else // Have no add
												{
													r_cons_printf("%#"PFMT64x": %s x%u, %#"PFMT64x"; %s %s%u, [x%u], %s%"PFMT64d"\n", addr, is_adrp ? "adrp" : "adr", reg, target, inst, rs, v & 0x1f, reg2, sign, (st64)soff);
												}
											}
										}
									}
								}
							}
						}
						// TODO: pairs, SIMD ldr/str, atomics
					}
				}
			}
		}
		else if((v & 0xbf000000) == 0x18000000 || (v & 0xff000000) == 0x98000000) // ldr and ldrsw literal
		{
			int64_t off = ((int64_t)((v >> 5) & 0x7ffff) << 45) >> 43;
			if (!search) {
				r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x"\n", addr, addr + off);
			} else if(addr + off == search) {
				ut32 reg  = v & 0x1f;
				bool is_ldrsw = (v & 0xff000000) == 0x98000000;
				bool is_64bit = (v & 0x40000000) != 0 && !is_ldrsw;
				r_cons_printf("%#"PFMT64x": %s %s%u, %#"PFMT64x"\n", addr, is_ldrsw ? "ldrsw" : "ldr", is_64bit ? "x" : "w", reg, search);
			}
		}
		else if((v & 0x7c000000) == 0x14000000) // b and bl
		{
			int64_t off = ((int64_t)(v & 0x3ffffff) << 38) >> 36;
			bool is_bl = (v & 0x80000000) != 0;
			if (!search) {
				r_cons_printf("ax 0x%"PFMT64x" 0x%"PFMT64x" # %s %#"PFMT64x"\n", addr, addr + off, is_bl ? "bl" : "b", addr + off);
			} else if (addr + off == search) {
				r_cons_printf("%#"PFMT64x": %s %#"PFMT64x"\n", addr, is_bl ? "bl" : "b", search);
			}
		}
		else if((v & 0xff000010) == 0x54000000) // b.cond
		{
			int64_t off = ((int64_t)((v >> 5) & 0x7ffff) << 45) >> 43;
			if (!search) {
				r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x"\n", addr, addr + off);
			} else if(addr + off == search) {
				const char *cond;
				switch(v & 0xf)
				{
					case 0x0: cond = "eq"; break;
					case 0x1: cond = "ne"; break;
					case 0x2: cond = "hs"; break;
					case 0x3: cond = "lo"; break;
					case 0x4: cond = "mi"; break;
					case 0x5: cond = "pl"; break;
					case 0x6: cond = "vs"; break;
					case 0x7: cond = "vc"; break;
					case 0x8: cond = "hi"; break;
					case 0x9: cond = "ls"; break;
					case 0xa: cond = "ge"; break;
					case 0xb: cond = "lt"; break;
					case 0xc: cond = "gt"; break;
					case 0xd: cond = "le"; break;
					case 0xe: cond = "al"; break;
					case 0xf: cond = "nv"; break;
				}
				r_cons_printf("%#"PFMT64x": b.%s %#"PFMT64x"\n", addr, cond, search);
			}
		}
		else if((v & 0x7e000000) == 0x34000000) // cbz and cbnz
		{
			int64_t off = ((int64_t)((v >> 5) & 0x7ffff) << 45) >> 43;
			if (!search) {
				r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x"\n", addr, addr + off);
			} else if(addr + off == search) {
				ut32 reg  = v & 0x1f;
				bool is_64bit = (v & 0x80000000) != 0;
				bool is_nz    = (v & 0x01000000) != 0;
				r_cons_printf ("%#"PFMT64x": %s %s%u, %#"PFMT64x"\n", addr, is_nz ? "cbnz" : "cbz", is_64bit ? "x" : "w", reg, search);
			}
		}
		else if((v & 0x7e000000) == 0x36000000) // tbz and tbnz
		{
			int64_t off = ((int64_t)((v >> 5) & 0x3fff) << 50) >> 48;
			if (!search) {
				r_cons_printf ("ax 0x%"PFMT64x" 0x%"PFMT64x"\n", addr, addr + off);
			} else if (addr + off == search) {
				ut32 reg  = v & 0x1f;
				ut32 bit  = ((v >> 19) & 0x1f) | ((v >> 26) & 0x20);
				bool is_64bit = bit > 31;
				bool is_nz    = (v & 0x01000000) != 0;
				r_cons_printf ("%#"PFMT64x": %s %s%u, %u, %#"PFMT64x"\n", addr, is_nz ? "tbnz" : "tbz", is_64bit ? "x" : "w", reg, bit, search);
			}
		}
	}
}

static int r_cmdsixref_call(void *user, const char *input) {
	if (r_str_startswith (input, "sixref")) {
		RCore *core = (RCore *)user;
		const char *arch = r_config_get (core->config, "asm.arch");
		const int bits = r_config_get_i (core->config, "asm.bits");
		if (!strstr (arch, "arm") || bits != 64) {
			eprintf ("This command only works on arm64. Please check your asm.{arch,bits}\n");
			return true;
		}
		ut64 search = r_num_math (core->num, input + 6);
		// TODO: honor search.in and such
		eprintf ("Finding xrefs to 0x%08"PFMT64x" in 0x%08"PFMT64x"-0x%08"PFMT64x"\n", search, core->offset, core->offset + core->blocksize);
		siguza_xrefs (core, search, true);
		return true;
	}
	return false;
}

RCorePlugin r_core_plugin_sixref = {
	.name = "sixref",
	.desc = "quickly find xrefs in arm64 buffer",
	.license = "MIT",
	.call = r_cmdsixref_call,
};

#ifndef R2_PLUGIN_INCORE
R_API RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_CORE,
	.data = &r_core_plugin_sixref,
	.version = R2_VERSION
};
#endif
