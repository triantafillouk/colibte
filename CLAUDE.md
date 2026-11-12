# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

Colibte (program names `cte`/`ctg2`/`ctg3`/`ctg4`/`ctxe`/`ce`) is "Colibri", a lightweight C text editor with
console (ncurses/panel), GTK2/3/4 and raw X11 (Xlib) drivers, a directory/file browser, syntax highlighting,
and a custom C-like macro/scripting language ("mlang") used both for user macros and for internal key-binding
dispatch. See `Readme.md` for end-user features and keybindings — it is comprehensive and not duplicated here.

## Build

This is a plain `make`-based C project, no autotools/cmake. There is one `makefile` with multiple targets that
each compile the *same* core sources with different preprocessor flags/libs to produce different driver
variants:

```sh
make cte      # console driver (ncurses/panel), with notes (sqlite3)
make ce       # console driver, no notes/sqlite3 dependency (lighter build)
make ctg2     # GTK2 driver
make ctg3     # GTK3 driver
make ctg4     # GTK4 driver
make ctxe     # raw X11/Xlib driver (no utf8 support), no plotting
make all      # cleanall + cte ctg3 ctg2 ctg4 ctxe
make clean    # remove *.o, *~, tagx
make cleanall # clean + remove built binaries (cte, ctxe, ctg2, ctg3, ce, *.exe)
make install  # builds cte/ctg2/ctg3 and installs rc/ext/menu/colors files + binaries system-wide (uses sudo)
```

Each target sets its own combination of `PCURSES`, `GTK`/`GTK3`/`GTK4`, `XLIB`, `_X11_`, `TNOTES`, `USE_GLIB`,
`XPLOT`, `EMBED_ICONS`, all passed as `-D` defines (see `FLAGS1`/`FLAGS3`/`FLAGS4` in the makefile) and tested
via `#if` in `xe.h` and elsewhere. `TNOTES=1` pulls in sqlite3 (`pkg-config sqlite3`) and compiles `notes.c`;
`ce` is the one variant built without it. OS is detected via `uname` (`Darwin`/`Linux`/`SunOS`/`CYGWIN_NT-10.0`)
and WSL is detected by grepping `uname -a` for `icrosoft` — this changes library flags (e.g. `ncursesw` vs
`ncurses`, `X11lib`) and which `.colibte_ext_*` file gets installed.

Per-object-file rules exist because the *same* `.c` file is compiled multiple times with different flags/output
names for different targets (e.g. `screen.c` → `screen.o`/`screen_ce.o`/`screen3.o`/`screen4.o`,
`input.c` → `input.o`/`input_ce.o`/`ginput.o`/...). When adding a new source file that needs to be built for
several variants, follow this existing pattern rather than introducing a new build system.

`find_tags` is a small standalone ctags-like helper also built from this tree (`make find_tags`).

There is no automated CI config in this repo — building/testing is manual, see below.

## Versioning convention — IMPORTANT

`xe.h` defines:
```c
#define VERSION  "#01.8T613 (10/11/2026)"
```
Every single commit in this repo bumps this string (sequential `T` number + commit date), and the commit
message is literally the new version string plus a short note, e.g. `#01.8T613 (10/11/2026) .ps highlight fix`.
When asked to commit a code change here, bump `VERSION` in `xe.h` (increment the `T` number, update the date)
and use that string as the commit message prefix, matching `git log` history.

## Testing

There is no unit test framework. Testing is done by running the editor's own macro language (`.cmd` files)
through the built binary in batch mode and diffing output:

- `cte -x somefile.cmd` executes a macro file and quits (used for both macro-language test scripts and real
  user macros). `-X` runs a file as a startup script instead. `-d` suppresses timing/time-based output so runs
  are diffable; `-D` enables debug messages on stderr (redirect it: `cte -D file.txt 2> out.err`); `-g` forces
  the BNF evaluator and dumps bnf tokens. `-N`/`-T`/`-L` open the notes/todo/calendar window, `-b` opens as
  binary, `-m` uses mmap for large read-only files, `-C`/`-8` set ncurses color depth, `-n` disables mouse,
  `-s file` replays a saved keyboard session.
- Macro-language regression scripts/fixtures live in `cmds2/`, `cmds3/`, `cmds_err/`, `nt/`, `nt2/`, `file_tests/`
  (plain `.cmd` macro scripts, plus `.tdc`/`.py` fixtures for arrays etc.). `nt.lst`/`nt2.lst` are curated lists
  of which `.cmd` files to run.
- The `check*.sh` scripts in the repo root iterate one of these lists/dirs, run the binary with `-x -d`
  (optionally `-D`) against each `.cmd`, and `tee` combined stdout/stderr to a `*.out`/`*.err` file for
  comparison against a previous known-good run (there are many such captured `*.out` files checked into the
  repo root, e.g. `check4_995_f.out`, used as before/after baselines when changing the macro engine).
- `cdif.sh` / `check3.sh` specifically diff the **BNF** evaluator output (`-x`, the `TBNF` path) against the
  normal/legacy evaluator output (`-f`) for the same `.cmd` file — use this when touching `bnf_expr.c` or
  `mlang_expr.c` to confirm the two engines still agree.
- `test_files/` and `UTF-8`/encoding sample files are fixtures for file-reading/encoding edge cases (BOM,
  UTF-16, compressed `.gz`/`.bz2`, CRLF, etc.), exercised through `file.c`/`convert.c`/`utf8_support.c`.

When changing editor behavior, prefer adding/adjusting a `.cmd` macro script under the relevant directory and
running it with `-x -d` rather than inventing a new test mechanism.

## Architecture

### Core data model (`xe.h`)

- `FILEBUF` — one open file/buffer. Holds the actual text in a gap buffer (`buffer`, `BufferSize`, `GapSize` —
  implemented in `filebuf.c`), undo state (`main_undo`, `undo.h`), highlight state, dir-mode state
  (`rdir`/`cdir`/`dir_list_str` when the buffer is a directory listing), and — when the macro language has
  parsed it — a token table (`tok_table` / `tok_table_bnf` when `TBNF`), a local symbol table (`symbol_tree`),
  and notes/tags data (`b_note`, `b_tag_list`) when `TNOTES`.
- `TextPoint` — a position within a `FILEBUF` (`offset`/`line`/`col`), the unit cursors, marks and selections
  are expressed in. Several well-known points are cached on `FILEBUF`/`WINDP` (`tp_base`, `tp_text_end`,
  `tp_hline` = top-of-window line, `smark`/`emark` = selection bounds) to avoid re-scanning the buffer.
- `WINDP` — a window viewing a `FILEBUF` (a buffer can have multiple windows). Owns per-window highlight state
  (`hs[2]`/`MLQUOTES`), the virtual screen (`vs`, built from `VIDEO`/`vchar`), and a `GWINDP *gwp` for
  driver-specific window data.
- `GWINDP` is defined three different ways under `#if GTK` / `#if PCURSES` / `#if XLIB` — it's the same struct
  name holding completely different driver-native window handles (GTK widgets, curses `WINDOW`/`PANEL`, or
  Xlib `Drawable`/`GC`). Window tree layout (splits) is tracked via `BTWE`, a binary tree of window regions.
- Multiple windows/buffers are managed through the generic linked-list type in `alist.c`/`alist.h` (`file_list`,
  `window_list`, `macro_list`, etc., created in `main.c:init_lists()`), plus an AVL/RB tree (`avl_tree.c`,
  `rb_tree.c`, `btreei.h`/`btree_iavl.h`) used for symbol/variable lookups.

### Driver abstraction

One shared core (editing logic, `filebuf.c`, `highlight.c`, `search.c`, `word.c`, `dir.c`, `eval.c`, `mlang*.c`,
`notes.c`, ...) is rendered through driver-specific display modules selected at compile time via
`display_driver.h`/`screen_driver.h`/`edit_edfunctions.h`:

- `panel_curses.c` + `tldisplay.c` — ncurses/panel console driver (`cte`, `ce`).
- `gtkterm.c`/`gtkterm3.c`/`gtkterm4.c` + `geditdisplay*.c` + `gldisplay.c` + `gplotc*.c`/`gcanvas*.c`
  (cairo-based plotting canvas) — GTK2/3/4 drivers (`ctg2`/`ctg3`/`ctg4`).
- `xlib.c` — raw X11 driver (`ctxe`), no plotting, no utf8.

All drivers render through the same `VIDEO`/`vchar` "virtual screen" abstraction in `screen.c`, so
driver-independent code should update the virtual screen rather than talking to a toolkit directly.
`xthemes.c`/`color.h`/`rgb_colors.h` + the `.colors`/`.colors16` files and `colors/*.lua` theme files define
color schemes shared across drivers; GTK drivers also get a live scheme editor.

### Macro language ("mlang")

A C-like scripting/macro language is both the user-facing automation feature (`.cmd` files, ALT-1..ALT-5,
F7/F8 keyboard macros) and the mechanism used internally to bind names to editor actions: `func.h`'s `ftable[]`
maps macro function names (e.g. `"goto_line"`) straight to C functions, and `assign_function`/`assign_sub`
let users rebind keys to either.

There are **two parallel expression-evaluator implementations**, selected by the `TBNF` flag in `xe.h`
(currently `TBNF=1`, i.e. the BNF engine is the active default engine), both parsing the same token stream
produced by `mlang_parser.c` (`tok_struct`, defined in `mlang.h`):

- Legacy/generic evaluator: `mlang.c`, `mlang_expr.c`, `mlang_err.c`, `mlang_array.c`, `mlang_functions.c`,
  `mlangf.c` — a conventional recursive-descent expression evaluator.
- BNF evaluator (`bnf_expr.c`, `bnf_mlangf.c`, `bnf_mlang_f.c`, declared in `bnf_functions.h`) — compiles each
  token to a specific function pointer (`tok_struct.bnf_factor_function`) chosen per concrete type combination
  for speed, e.g. separate functions for num+num vs pointer+num vs pointer+pointer addition
  (`bnf_factor_nn_plus`, `bnf_factor_pn_plus`, `bnf_factor_pp_plus`, ...). This is the engine under active
  development (see the `bnf` branch); `-g` on the command line forces it and dumps its token stream, and
  `cdif.sh`/`check3.sh` diff its output (`-x`) against the legacy engine's (`-f`) for the same macro file.

Variable storage during evaluation uses a fixed-size `MVAR` stack (`bnf_vars[MAX_VARS]` in `bnf_expr.c`), with
`VTYPE_*` constants in `xe.h` tagging what a variable currently holds (number, numeric/string array or list,
buffer reference, proc pointer, editor option/function reference, etc.).

### Highlighting

`highlight.c`/`highlight.h` implement syntax highlighting per language, driven by a `SHLIGHT` table entry per
language/file type (word lists, a highlight state-machine function, a word-boundary predicate, a comment
predicate, associated file extensions) — see the `H_QUOTE*`/`H_WORD*` bit flags in `xe.h` for the quoting/
comment state machine shared by all languages. adding support for a new language means adding a new `SHLIGHT` 
entry and state function rather than touching the per-window update path.

### Notes / tags (`TNOTES`)

`notes.c`/`notes.h`, built only into `cte`/`ctg2`/`ctg3`/`ctg4` (not `ce`/`ctxe`), persist notes/todo/calendar
entries and ctags-derived tags via sqlite3, surfaced through a dedicated notes/tag buffer type
(`b_type` = `DIR_NOTES`/`NOTE_TYPE`/`NOTE_TODO_TYPE`/...).

### Config/user files

- `.colibte_rc` — default rc file installed to `~/.colibte` / shared dir; `config_init.c` loads it.
- `.colibte_mnu` — the user-editable ALT-A menu.
- `.colibte_ext_linux` / `.colibte_ext_mac` / `.colibte_ext_wsl` — per-OS external viewer/editor-by-file-type
  associations; `install` picks the right one based on `OSYSTEM`/`WSL`.
- `.colors` / `.colors16` (+ `_ori` originals) and `colors/*.lua` — console and scheme-editor color themes.
