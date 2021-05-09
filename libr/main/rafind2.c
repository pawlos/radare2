/* radare - LGPL - Copyright 2009-2020 - pancake */

#include <stdio.h>
#include <stdlib.h>

#include <r_main.h>
#include <r_types.h>
#include <r_search.h>
#include <r_util.h>
#include <r_util/r_print.h>
#include <r_cons.h>
#include <r_lib.h>
#include <r_io.h>


typedef struct {
	bool showstr;
	bool rad;
	bool identify;
	bool quiet;
	bool hexstr;
	bool widestr;
	bool nonstop;
	bool json;
	int mode;
	int align;
	ut8 *buf;
	ut64 bsize;
	ut64 from;
	ut64 to;
	ut64 cur;
	RPrint *pr;
	RList *keywords;
	const char *mask;
	const char *curfile;
	PJ *pj;
} RafindOptions;

static void rafind_options_fini(RafindOptions *ro) {
	free (ro->buf);
	ro->cur = 0;
}

static void rafind_options_init(RafindOptions *ro) {
	memset (ro, 0, sizeof (RafindOptions));
	ro->mode = R_SEARCH_STRING;
	ro->bsize = 4096;
	ro->to = UT64_MAX;
	ro->keywords = r_list_newf (NULL);
	ro->pj = NULL;
}

static int rafind_open(RafindOptions *ro, const char *file);

static int hit(RSearchKeyword *kw, void *user, ut64 addr) {
	RafindOptions *ro = (RafindOptions*)user;
	int delta = addr - ro->cur;
	if (ro->cur > addr && (ro->cur - addr == kw->keyword_length - 1)) {
		// This case occurs when there is hit in search left over
		delta = ro->cur - addr;
	}
	if (delta < 0) {
		delta = 0;
	}
	if (delta >= ro->bsize) {
		eprintf ("Invalid delta %d from 0x%08"PFMT64x"\n", delta, addr);
		return 0;
	}
	char _str[128];
	char *str = _str;
	*_str = 0;
	if (ro->showstr) {
		if (ro->widestr) {
			str = _str;
			int i, j = 0;
			for (i = delta; ro->buf[i] && i < sizeof (_str) - 1; i++) {
				char ch = ro->buf[i];
				if (ch == '"' || ch == '\\') {
					ch = '\'';
				}
				if (!IS_PRINTABLE (ch)) {
					break;
				}
				str[j++] = ch;
				i++;
				if (j > 80) {
					strcpy (str + j, "...");
					j += 3;
					break;
				}
				if (ro->buf[i]) {
					break;
				}
			}
			str[j] = 0;
		} else {
			size_t i;
			for (i = 0; i < sizeof (_str) - 1; i++) {
				char ch = ro->buf[delta + i];
				if (ch == '"' || ch == '\\') {
					ch = '\'';
				}
				if (!ch || !IS_PRINTABLE (ch)) {
					break;
				}
				str[i] = ch;
			}
			str[i] = 0;
		}
	} else {
		size_t i;
		for (i = 0; i < sizeof (_str) - 1; i++) {
			char ch = ro->buf[delta + i];
			if (ch == '"' || ch == '\\') {
				ch = '\'';
			}
			if (!ch || !IS_PRINTABLE (ch)) {
				break;
			}
			str[i] = ch;
		}
		str[i] = 0;
	}
	if (ro->json) {
		pj_o (ro->pj);
		pj_ks (ro->pj, "file", ro->curfile);
		pj_kn (ro->pj, "offset", addr);
		pj_ks (ro->pj, "type", "string");
		pj_ks (ro->pj, "data", str);
		pj_end (ro->pj);
	} else if (ro->rad) {
		printf ("f hit%d_%d 0x%08"PFMT64x" ; %s\n", 0, kw->count, addr, ro->curfile);
	} else {
		if (!ro->quiet) {
			printf ("%s: ", ro->curfile);
		}
		if (ro->showstr) {
			printf ("0x%"PFMT64x" %s\n", addr, str);
		} else {
			printf ("0x%"PFMT64x"\n", addr);
			if (ro->pr) {
				r_print_hexdump (ro->pr, addr, (ut8*)ro->buf + delta, 78, 16, 1, 1);
				r_cons_flush ();
			}
		}
	}
	return 1;
}

static int show_help(const char *argv0, int line) {
	printf ("Usage: %s [-mXnzZhqv] [-a align] [-b sz] [-f/t from/to] [-[e|s|S] str] [-x hex] -|file|dir ..\n", argv0);
	if (line) {
		return 0;
	}
	printf (
	" -a [align] only accept aligned hits\n"
	" -b [size]  set block size\n"
	" -e [regex] search for regex matches (can be used multiple times)\n"
	" -f [from]  start searching from address 'from'\n"
	" -F [file]  read the contents of the file and use it as keyword\n"
	" -h         show this help\n"
	" -i         identify filetype (r2 -nqcpm file)\n"
	" -j         output in JSON\n"
	" -m         magic search, file-type carver\n"
	" -M [str]   set a binary mask to be applied on keywords\n"
	" -n         do not stop on read errors\n"
	" -r         print using radare commands\n"
	" -s [str]   search for a specific string (can be used multiple times)\n"
	" -S [str]   search for a specific wide string (can be used multiple times). Assumes str is UTF-8.\n"
	" -t [to]    stop search at address 'to'\n"
	" -q         quiet - do not show headings (filenames) above matching contents (default for searching a single file)\n"
	" -v         print version and exit\n"
	" -x [hex]   search for hexpair string (909090) (can be used multiple times)\n"
	" -X         show hexdump of search results\n"
	" -z         search for zero-terminated strings\n"
	" -Z         show string found on each search hit\n"
	);
	return 0;
}

static int rafind_open_file(RafindOptions *ro, const char *file, const ut8 *data, int datalen) {
	RListIter *iter;
	RSearch *rs = NULL;
	const char *kw;
	bool last = false;
	int ret, result = 0;

	ro->buf = NULL;
	char *efile = r_str_escape_sh (file);

	if (ro->identify) {
		char *cmd = r_str_newf ("r2 -e search.show=false -e search.maxhits=1 -nqcpm \"%s\"", efile);
		r_sandbox_system (cmd, 1);
		free (cmd);
		free (efile);
		return 0;
	}

	RIO *io = r_io_new ();
	if (!io) {
		free (efile);
		return 1;
	}

	if (!r_io_open_nomap (io, file, R_PERM_R, 0)) {
		eprintf ("Cannot open file '%s'\n", file);
		result = 1;
		goto err;
	}

	if (data) {
		r_io_write_at (io, 0, data, datalen);
	}

	rs = r_search_new (ro->mode);
	if (!rs) {
		result = 1;
		goto err;
	}

	ro->buf = calloc (1, ro->bsize);
	if (!ro->buf) {
		eprintf ("Cannot allocate %"PFMT64d" bytes\n", ro->bsize);
		result = 1;
		goto err;
	}
	rs->align = ro->align;
	r_search_set_callback (rs, &hit, ro);
	ut64 to = ro->to;
	if (to == -1) {
		to = r_io_size (io);
	}

	if (!r_cons_new ()) {
		result = 1;
		goto err;
	}

	if (ro->mode == R_SEARCH_STRING) {
		/* TODO: implement using api */
		r_sys_cmdf ("rabin2 -q%szzz \"%s\"", ro->json? "j": "", efile);
		goto done;
	}
	if (ro->mode == R_SEARCH_MAGIC) {
		/* TODO: implement using api */
		char *tostr = (to && to != UT64_MAX)?  r_str_newf ("-e search.to=%"PFMT64d, to): strdup ("");
		r_sys_cmdf ("r2"
			    " -e search.in=range"
			    " -e search.align=%d"
			    " -e search.from=%" PFMT64d
			    " %s -qnc/m%s \"%s\"",
			ro->align, ro->from, tostr, ro->json? "j": "", efile);
		free (tostr);
		goto done;
	}
	if (ro->mode == R_SEARCH_ESIL) {
		/* TODO: implement using api */
		r_list_foreach (ro->keywords, iter, kw) {
			r_sys_cmdf ("r2 -qc \"/E %s\" \"%s\"", kw, efile);
		}
		goto done;
	}
	if (ro->mode == R_SEARCH_KEYWORD) {
		r_list_foreach (ro->keywords, iter, kw) {
			if (ro->hexstr) {
				if (ro->mask) {
					r_search_kw_add (rs, r_search_keyword_new_hex (kw, ro->mask, NULL));
				} else {
					r_search_kw_add (rs, r_search_keyword_new_hexmask (kw, NULL));
				}
			} else if (ro->widestr) {
				r_search_kw_add (rs, r_search_keyword_new_wide (kw, ro->mask, NULL, 0));
			} else {
				r_search_kw_add (rs, r_search_keyword_new_str (kw, ro->mask, NULL, 0));
			}
		}
	}
	if (ro->mode == R_SEARCH_REGEXP) {
		r_list_foreach (ro->keywords, iter, kw) {
			r_search_kw_add (rs, r_search_keyword_new_regexp (kw, NULL));
		}
	}

	ro->curfile = file;
	r_search_begin (rs);
	(void)r_io_seek (io, ro->from, R_IO_SEEK_SET);
	result = 0;
	ut64 bsize = ro->bsize;
	for (ro->cur = ro->from; !last && ro->cur < to; ro->cur += bsize) {
		if ((ro->cur + bsize) > to) {
			bsize = to - ro->cur;
			last = true;
		}
		ret = r_io_pread_at (io, ro->cur, ro->buf, bsize);
		if (ret == 0) {
			if (ro->nonstop) {
				continue;
			}
			result = 1;
			break;
		}
		if (ret != bsize && ret > 0) {
			bsize = ret;
		}
		if (r_search_update (rs, ro->cur, ro->buf, ret) == -1) {
			eprintf ("search: update read error at 0x%08"PFMT64x"\n", ro->cur);
			break;
		}
	}
done:
	r_cons_free ();
err:
	free (efile);
	r_search_free (rs);
	r_io_free (io);
	rafind_options_fini (ro);
	return result;
}

static int rafind_open_dir(RafindOptions *ro, const char *dir) {
	RListIter *iter;
	char *fullpath;
	char *fname = NULL;

	RList *files = r_sys_dir (dir);

	if (files) {
		r_list_foreach (files, iter, fname) {
			/* Filter-out unwanted entries */
			if (*fname == '.') {
				continue;
			}
			fullpath = r_str_newf ("%s"R_SYS_DIR"%s", dir, fname);
			(void)rafind_open (ro, fullpath);
			free (fullpath);
		}
		r_list_free (files);
	}
	return 0;
}

static int rafind_open(RafindOptions *ro, const char *file) {
	if (!strcmp (file, "-")) {
		int sz = 0;
		ut8 *buf = (ut8 *)r_stdin_slurp (&sz);
		if (!buf) {
			return 0;
		}
		char *ff = r_str_newf ("malloc://%d", sz);
		int res = rafind_open_file (ro, ff, buf, sz);
		free (ff);
		free (buf);
		return res;
	}
	return r_file_is_directory (file)
		? rafind_open_dir (ro, file)
		: rafind_open_file (ro, file, NULL, -1);
}


R_API int r_main_rafind2(int argc, const char **argv) {
	RafindOptions ro;
	rafind_options_init (&ro);

	int c;
	const char *file = NULL;

	RGetopt opt;
	r_getopt_init (&opt, argc, argv, "a:ie:b:jmM:s:S:x:Xzf:F:t:E:rqnhvZ");
	while ((c = r_getopt_next (&opt)) != -1) {
		switch (c) {
		case 'a':
			ro.align = r_num_math (NULL, opt.arg);
			break;
		case 'r':
			ro.rad = true;
			break;
		case 'i':
			ro.identify = true;
			break;
		case 'j':
			ro.json = true;
			break;
		case 'n':
			ro.nonstop = 1;
			break;
		case 'm':
			ro.mode = R_SEARCH_MAGIC;
			break;
		case 'e':
			ro.mode = R_SEARCH_REGEXP;
			ro.hexstr = 0;
			r_list_append (ro.keywords, (void*)opt.arg);
			break;
		case 'E':
			ro.mode = R_SEARCH_ESIL;
			r_list_append (ro.keywords, (void*)opt.arg);
			break;
		case 's':
			ro.mode = R_SEARCH_KEYWORD;
			ro.hexstr = false;
			ro.widestr = false;
			r_list_append (ro.keywords, (void*)opt.arg);
			break;
		case 'S':
			ro.mode = R_SEARCH_KEYWORD;
			ro.hexstr = false;
			ro.widestr = true;
			r_list_append (ro.keywords, (void*)opt.arg);
			break;
		case 'b':
			{
			int bs = (int)r_num_math (NULL, opt.arg);
			if (bs < 2) {
				eprintf ("Invalid blocksize <= 1\n");
				return 1;
			}
			ro.bsize = bs;
			}
			break;
		case 'M':
			// XXX should be from hexbin
			ro.mask = opt.arg;
			break;
		case 'f':
			ro.from = r_num_math (NULL, opt.arg);
			break;
		case 'F':
			{
				size_t data_size;
				char *data = r_file_slurp (opt.arg, &data_size);
				if (!data) {
					eprintf ("Cannot slurp '%s'\n", opt.arg);
					return 1;
				}
				char *hexdata = r_hex_bin2strdup ((ut8*)data, data_size);
				if (hexdata) {
					ro.mode = R_SEARCH_KEYWORD;
					ro.hexstr = true;
					ro.widestr = false;
					r_list_append (ro.keywords, (void*)hexdata);
				}
				free (data);
			}
			break;
		case 't':
			ro.to = r_num_math (NULL, opt.arg);
			break;
		case 'x':
			ro.mode = R_SEARCH_KEYWORD;
			ro.hexstr = 1;
			ro.widestr = 0;
			r_list_append (ro.keywords, (void*)opt.arg);
			break;
		case 'X':
			ro.pr = r_print_new ();
			break;
		case 'q':
			ro.quiet = true;
			break;
		case 'v':
			return r_main_version_print ("rafind2");
		case 'h':
			return show_help(argv[0], 0);
		case 'z':
			ro.mode = R_SEARCH_STRING;
			break;
		case 'Z':
			ro.showstr = true;
			break;
		default:
			return show_help (argv[0], 1);
		}
	}
	if (opt.ind == argc) {
		return show_help (argv[0], 1);
	}
	/* Enable quiet mode if searching just a single file */
	if (opt.ind + 1 == argc && !r_file_is_directory (argv[opt.ind])) {
		ro.quiet = true;
	}
	if (ro.json && (ro.mode == R_SEARCH_KEYWORD || ro.mode == R_SEARCH_REGEXP)) {
		// TODO: remove mode check when all modes use api
		ro.pj = pj_new ();
		pj_a (ro.pj);
	}
	for (; opt.ind < argc; opt.ind++) {
		file = argv[opt.ind];

		if (file && !*file) {
			eprintf ("Cannot open empty path\n");
			return 1;
		}
		rafind_open (&ro, file);
	}
	r_list_free (ro.keywords);
	if (ro.pj) {
		pj_end (ro.pj);
		printf ("%s\n", pj_string (ro.pj));
		pj_free (ro.pj);
	}
	return 0;
}
