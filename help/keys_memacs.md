## Key assignements sorted by function (all key tables)
## emulation Micro Emacs

| function name          | key shortcut                  | Mode       | description                                    |
| ---------------------- | ----------------------------- | -----------| ---------------------------------------------- |
| menu                   | ALT-S          (       ALT-S) | Normal mode | Menu command
| menu                   | ALT-F          (       ALT-F) | Normal mode | Menu command
| menu                   | ALT-E          (       ALT-E) | Normal mode | Menu command
| menu                   | ALT-H          (       ALT-H) | Normal mode | Menu command
| menu                   | ALT-M          (       ALT-M) | Normal mode | Menu command
| menu                   | ALT-T          (       ALT-T) | Normal mode | Menu command
| menu                   | ALT-W          (       ALT-W) | Normal mode | Menu command
| work                   |      -                        | -           | Work menu
| show_version           |      -                        | -           | Show the current version on status line
| abort                  | ^G             (      CTRL-G) | Normal mode | Abort key
| abort                  | Escape         (  ALT-CTRL-\[) | Normal mode | Abort key
| color_scheme           |      -                        | -           | Change the color scheme currently used
| xcolor_scheme          |      -                        | -           | Change the xcolor scheme currently used
| prev_character         | KEY-LEFT       (      ALT-\[D) | Normal mode | move left
| prev_character         | Arrow left     (      ALT-OD) | Normal mode | move left
| prev_character         | Arrow1 left    (    spec2 33) | Normal mode | move left
| prev_character         | h              (           h) |   view mode | move left
| record_macro           | key F7         (    ALT-\[18~) | Normal mode | start and stop a key macro recording
| bof                    | ALT-\<          (       ALT-\<) | Normal mode | Go to the start of the file
| bof                    | ^HOME          (    spec2 11) | Normal mode | Go to the start of the file
| bof                    | ^numeric 7     (    spec2 51) | Normal mode | Go to the start of the file
| bof                    | 1              (           1) |   view mode | Go to the start of the file
| bof                    | 1              (           1) |  notes mode | Go to the start of the file
| bol                    | ^A             (      CTRL-A) | Normal mode | Go to the start of the line
| bol                    | Home key       (      ALT-OH) | Normal mode | Go to the start of the line
| bol                    | Home1 key      (      ALT-\[H) | Normal mode | Go to the start of the line
| bol                    | Home2 key      (      ALT-OM) | Normal mode | Go to the start of the line
| bol                    | 0              (           0) |   view mode | Go to the start of the line
| assign_function        | ALT-K          (       ALT-K) | Normal mode | Assign a function to a key
| assign_function        | ALT-^K         (  ALT-CTRL-K) | Normal mode | Assign a function to a key
| assign_sub             |      -                        | -           | Assign a macro subroutine to a key
| execsub                | page_loop      (       ALT-!) | Normal mode | execute a subroutine
| execsub                | show_out       (       ALT-,) | Normal mode | execute a subroutine
| execsub                | w_abbrev       (       ALT-/) | Normal mode | execute a subroutine
| execsub                | test1          (         ^XX) | Normal mode | execute a subroutine
| execsub                | clear_results  (         ^XC) | Normal mode | execute a subroutine
| execsub                | clear_results  (       ALT-?) | Normal mode | execute a subroutine
| execsub                | test1          (       ALT-~) | Normal mode | execute a subroutine
| region_lower           | ^X^L           (         ^XL) | Normal mode | Lower case the characters of a selected region
| region_upper           | ^X^U           (         ^XU) | Normal mode | Upper case the characters of a selected region
| convert_char           |      -                        | -           | Character set conversion
| insert_tab             | ALT-I          (       ALT-I) | Normal mode | Insert a tab
| remove_tab             | ^X^I           (         ^XI) | Normal mode | Remove a tab
| remove_tab             | Shift TAB      `(      ALT-[Z)` | Normal mode | Remove a tab
| new_file               |      -                        | -           | new file
| rename_file            | ^XN            (         ^XN) | Normal mode | rename current file
| copy_region            | ^C             (      CTRL-C) | Normal mode | Copy region to clipboard
| copy_region            | `ALT-[I`         (      `ALT-[I)` | Normal mode | Copy region to clipboard
| count_words            | ALT-^C         (  ALT-CTRL-C) | Normal mode | Count words of a selected region
| del_blank_lines        | ^X^O           (         ^XO) | Normal mode | Delete blanc lines before and after cursor, alsways left one blank line
| close_file             | ^XK            (         ^XK) | Normal mode | close current file
| close_file             | q              (           q) |   view mode | close current file
| close_file             | q              (           q) |  notes mode | close current file
| set_bg                 | ALT-Z          (       ALT-Z) | Normal mode | suspend
| set_bg                 | ^XZ            (         ^XZ) | Normal mode | suspend
| del_char               | delete key     (     ALT-\[3~) | Normal mode | delete the character under the cursor
| del_next_word          | ALT-D          (       ALT-D) | Normal mode | delete current word
| one_window             | ^X1            (         ^X1) | Normal mode | remove all other windows
| del_prev_word          | ALT-^H         (  ALT-CTRL-H) | Normal mode | delete previous word
| del_prev_char          | ^H             (      CTRL-H) | Normal mode | delete previous character
| del_prev_char          | backspace      (           ) | Normal mode | delete previous character
| delete_window          | ^X0            (         ^X0) | Normal mode | delete current window
| dos2unix               |      -                        | -           | convert dos file to unix
| unix2dos               |      -                        | -           | convert unix file to dos
| set_color              |      -                        | -           | set the background color
| show_keys              |      -                        | -           | show the current key assignements
| color_scheme_read      |      -                        | -           | color scheme read from file
| describe_key           | F4             (  `ALT-[14~`) | Normal mode | Show the function or subroutine assigned to a key
| describe_key           | key F4         (      ALT-OS) | Normal mode | Show the function or subroutine assigned to a key
| macro_line             | ALT-4          (       ALT-4) | Normal mode | execute a command line
| macro_line             | ALT-^X         (  ALT-CTRL-X) | Normal mode | execute a command line
| set_arg                | ALT space      (       ALT- ) | Normal mode | set a numeric argument for next function to execute
| set_arg                | F2             (  `ALT-[12~`) | Normal mode | set a numeric argument for next function to execute
| set_arg                | key F2         (      ALT-OQ) | Normal mode | set a numeric argument for next function to execute
| detab_line             | ^X^D           (         ^XD) | Normal mode | Convert tabs to spaces
| line_fill              | ^XF            (         ^XF) | Normal mode | fill the line, in right align mode insert spaces bettween characters
| eof                    | ALT->          (       ALT->) | Normal mode | Go to the end of file
| eof                    | ^END           (     spec2 9) | Normal mode | Go to the end of file
| eof                    | ^numeric 1     (   spec2 197) | Normal mode | Go to the end of file
| eof                    | G              (           G) |   view mode | Go to the end of file
| eof                    | G              (           G) |  notes mode | Go to the end of file
| eol                    | ^E             (      CTRL-E) | Normal mode | Go to the end of line
| eol                    | `ALT-[F`       (    `ALT-[F`) | Normal mode | Go to the end of line
| eol                    | End key        (    `ALT-OF`) | Normal mode | Go to the end of line
| eol                    | End1 key       (   spec2 213) | Normal mode | Go to the end of line
| eol                    | $              (           $) |   view mode | Go to the end of line
| entab_line             | ^X^E           (         ^XE) | Normal mode | Convert spaces to tabs in a line
| comment_line           | ALT-'          (       ALT-') | Normal mode | Comment line
| comment_line2          | ALT-;          (       ALT-;) | Normal mode | Comment line end
| swap_mark              | ^X^X           (         ^XX) | Normal mode | swap mark and cursor position
| exec_file              |      -                        | -           | execute a macro file
| exec_macro             | key F8         (  `ALT-[19~`) | Normal mode | execute a saved macro (F8)
| parse_check            | ^X\            (         ^X\) | Normal mode | parse_check_current_buffer
| show_tokens            |      -                        | -           | show token list of a parsed buffer
| undo                   | ^Z             (      CTRL-Z) | Normal mode | Undo changes
| redo                   | ^Y             (      CTRL-Y) | Normal mode | Redo changes
| redo                   | ^XU            (         ^XU) | Normal mode | Redo changes
| refresh_cline          | ALT-=          (       ALT-=) | Normal mode | refresh current line (macro) and show results after :
| refresh_buffer         | ALT-+          (       ALT-+) | Normal mode | execute current macro file
| refresh_buffer         | ^X=            (         ^X=) | Normal mode | execute current macro file
| quit                   | ALT-X          (       ALT-X) | Normal mode | quit the editor
| quit                   | ^X^C           (         ^XC) | Normal mode | quit the editor
| fill_paragraph         | ALT-Q          (       ALT-Q) | Normal mode | fill current paragraph filling lines
| open_file              | ^O             (      CTRL-O) | Normal mode | Open a file for editing
| clear_buffer           |      -                        | -           | Clear buffer named
| unmark_buffer          |      -                        | -           | unmark current buffer
| next_character         | KEY-RIGHT      (    `ALT-[C`) | Normal mode | move right
| next_character         | Arrow right    (      ALT-OC) | Normal mode | move right
| next_character         | Arrow1 right   (   spec2 233) | Normal mode | move right
| next_character         | l              (           l) |   view mode | move right
| next_character         | l              (            ) |   view mode | move right
| select_next_character  | SHIFT rigt     (      `ALT-[c`) | Normal mode | select and move right
| select_prev_character  | SHIFT left     (      `ALT-[d`) | Normal mode | select and move left
| select_next_line       | SHIFT down     (      `ALT-[b`) | Normal mode | select and move down
| select_prev_line       | SHIFT up       (      `ALT-[a`) | Normal mode | select and move up
| goto_line              | ALT-G          (       ALT-G) | Normal mode | Go to a specific line
| match_fence            | ^]             (      CTRL-]) | Normal mode | Find a correspondig fence and go there (C fences)
| tab                    | tab            (      CTRL-I) | Normal mode | insert a tab
| find_previous          | key F6         (    `ALT-[17~`) | Normal mode | Find previous occurense of a search
| find_previous          | ^Up arrow      (     spec2 4) | Normal mode | Find previous occurense of a search
| find_previous          | N              (           N) |   view mode | Find previous occurense of a search
| find_previous          | N              (           N) |  notes mode | Find previous occurense of a search
| find_next              | key F5         (    `ALT-[15~`) | Normal mode | Find next occurense of a search
| find_next              | ^Down arrow    (     spec2 5) | Normal mode | Find next occurense of a search
| find_next              | n              (           n) |   view mode | Find next occurense of a search
| find_next              | n              (           n) |  notes mode | Find next occurense of a search
| select_font            |      -                        | -           | Select a font
| set_fontindex          |      -                        | -           | select font from default list
| set_locale             |      -                        | -           | select localcodepage
| set_debug              |      -                        | -           | set debug flag
| toggle_bold            |      -                        | -           | togfle bold
| select_font_mono       |      -                        | -           | Select a monospace font
| increase_font_size     |      -                        | -           | increase font size
| decrease_font_size     |      -                        | -           | decrease font size
| window_column_increase | ALT-)          (       ALT-)) | Normal mode | window_column_increase
| window_column_decrease | ALT-(          (       ALT-() | Normal mode | window_column_decrease
| window_row_increase    | ALT-_          (       ALT-_) | Normal mode | window_row_increase
| window_row_decrease    | ALT--          (       ALT--) | Normal mode | window_row_decrease
| toggle_mouse           | ^Y             (       ALT-Y) | Normal mode | toggle mouse integration
| help                   |      -                        | -           | Show help text
| exec                   | ALT-1          (       ALT-1) | Normal mode | Execute a shell command, show output in [command] buffer
| exec                   | ^X!            (         ^X!) | Normal mode | Execute a shell command, show output in [command] buffer
| exec                   | key F9         (    `ALT-[20~`) | Normal mode | Execute a shell command, show output in [command] buffer
| bg_cmd                 | ^X&            (         ^X&) | Normal mode | Execute a background command, output in /tmp/cmd### file, error in /tmp/err### file
| new_shell              |      -                        | -           | start interactive shell (xterm like)
| rgrep_command          | ALT-%          (       ALT-%) | Normal mode | Recursive find insize current directory
| grep_command           | ALT-5          (       ALT-5) | Normal mode | Gr a word and show results in a new window
| script_exec            | ALT-2          (       ALT-2) | Normal mode | script exec buffer
| script_exec            | s              (           s) |    dir mode | script exec buffer
| filter_buffer          | ALT-3          (       ALT-3) | Normal mode | filter current file thought an external command
| filter_buffer          | ^X#            (         ^X#) | Normal mode | filter current file thought an external command
| insert_file            |      -                        | -           | insert a file at current position
| insert                 |      -                        | -           | insert a string to current position
| erase_eol              | ^K             (      CTRL-K) | Normal mode | remove characters till the end of line
| select_file            | ^B             (      CTRL-B) | Normal mode | Select an edited file to display. If interactive show a list first
| move_window            |      -                        | -           | Move window
| newline                | ^M             (      CTRL-M) | Normal mode | Insert a new line
| insert_indent          | ^J             (      CTRL-J) | Normal mode | insert indent
| insert_indent          | Enter key      (      ALT-OM) | Normal mode | insert indent
| next_file              | ^N             (      CTRL-N) | Normal mode | show next file in current window
| next_file              | key F10        (  `ALT-[21~`) | Normal mode | show next file in current window
| next_file              | ^PG-DOWN       (    `ALT-[z`) | Normal mode | show next file in current window
| next_file              | spec2 197      (   spec2 197) | Normal mode | show next file in current window
| next_file              | ^Page DN       (    spec2 21) | Normal mode | show next file in current window
| next_line              | KEY-DOWN       (    `ALT-[B`) | Normal mode | goto next line
| next_line              | Arrow down     (      ALT-OB) | Normal mode | goto next line
| next_line              | Arrow1 down    (   spec2 214) | Normal mode | goto next line
| next_line              | j              (           j) |   view mode | goto next line
| next_line              | j              (           j) |  notes mode | goto next line
| scroll_up              | SCROLL-UP      (mouse -32671) | Normal mode | move text up
| scroll_down            | SCROLL-DOWN    (mouse -32672) | Normal mode | move_text_down
| delete_line            | ^D             (      CTRL-D) | Normal mode | Delete current line
| next_paragraph         | ^X]            (         ^X]) | Normal mode | Go to end of paragraph
| next_paragraph         | ]              (           ]) |   view mode | Go to end of paragraph
| next_page              | ]              (           ]) |    dir mode | Goto next page
| next_page              | ]              (           ]) |  notes mode | Goto next page
| next_word              | ^KEY-RIGHT     (    `ALT-[N`) | Normal mode | move to next word
| next_word              | key crt        (     spec2 6) | Normal mode | move to next word
| next_word              | b              (           w) |   view mode | move to next word
| next_window            | ^P             (      CTRL-P) | Normal mode | Go to next window
| next_window            | ^XO            (         ^XO) | Normal mode | Go to next window
| next_window            | ^PG-UP         (    `ALT-[w`) | Normal mode | Go to next window
| next_window            | spec2 196      (   spec2 196) | Normal mode | Go to next window
| next_window            | Center key     (      ALT-OE) | Normal mode | Go to next window
| next_window            | ^numeric 9     (    spec2 53) | Normal mode | Go to next window
| next_window            | tab            (      CTRL-I) |    dir mode | Go to next window
| goto_eow               | ^\             (      CTRL-\) | Normal mode | move to end of word
| goto_eow               | ALT-\          (       ALT-\) | Normal mode | move to end of word
| goto_eow               | e              (           e) |   view mode | move to end of word
| prev_page              | \[             (          \[) |    dir mode | Move on page backwards
| prev_page              | \[             (          \[) |  notes mode | Move on page backwards
| prev_line              | KEY-UP         (     ALT-\[A) | Normal mode | move up
| prev_line              | Arrow up       (      ALT-OA) | Normal mode | move up
| prev_line              | k              (           k) |   view mode | move up
| prev_line              | k              (           k) |  notes mode | move up
| prev_paragraph         | `^X[`          (       `^X[`) | Normal mode | Move to the begin of previous paragraph
| prev_paragraph         | \[             (          \[) |   view mode | Move to the begin of previous paragraph
| prev_word              | ALT-|          (       ALT-|) | Normal mode | Move to previous word
| prev_word              | ALT-B          (       ALT-B) | Normal mode | Move to previous word
| prev_word              | ^KEY-LEFT      (    `ALT-[P`) | Normal mode | Move to previous word
| prev_word              | spec2 7        (     spec2 7) | Normal mode | Move to previous word
| prev_word              | b              (           b) |   view mode | Move to previous word
| prev_window            | ^XP            (         ^XP) | Normal mode | Move to previous window
| query_replace          | ^X^R           (         ^XR) | Normal mode | Interacive replace
| query_replace          | ALT-^R         (  ALT-CTRL-R) | Normal mode | Interacive replace
| quick_close            | F3             (  `ALT-[13~`) | Normal mode | Save file and close it
| quick_close            | key F3         (      ALT-OR) | Normal mode | Save file and close it
| quick_close            | v              (           v) |   view mode | Save file and close it
| quote_character        | ^Q             (      CTRL-Q) | Normal mode | Insert control character at current position
| redraw_display         | ^L             (      CTRL-L) | Normal mode | Redrawy display
| hard_reposition        | ALT-^P         (  ALT-CTRL-P) | Normal mode | Redraw display reseting textpoints
| window_row_resize      | ^XR            (         ^XR) | Normal mode | Resize window rows
| window_column_resize   | ^XV            (         ^XV) | Normal mode | Resize window cols
| replace_string         | ALT-R          (       ALT-R) | Normal mode | Replace a string
| replace_next           | ^X^R           (         ^XR) | Normal mode | replace next ??
| save_file              | ^S             (      CTRL-S) | Normal mode | Save the file
| save_file              | ^X^S           (         ^XS) | Normal mode | Save the file
| both_down              | ALT }          (       ALT-}) | Normal mode | Scroll files in both windows down
| both_up                | ALT {          (       ALT-{) | Normal mode | Scroll files in both windows up
| search_forward         | ^F             (      CTRL-F) | Normal mode | Ask for a word and search forward
| search_forward         | /              (           /) |   view mode | Ask for a word and search forward
| search_forward         | /              (           /) |  notes mode | Ask for a word and search forward
| search_reverse         | ^R             (      CTRL-R) | Normal mode | Ask for a word and search backward
| search_reverse         | ?              (           ?) |   view mode | Ask for a word and search backward
| search_reverse         | ?              (           ?) |  notes mode | Ask for a word and search backward
| save_colors            | ALT-*          (       ALT-*) | Normal mode | Save colors
| read_colors            | ALT-&          (       ALT-&) | Normal mode | read colors
| set_key                |      -                        | -           | Set the encryption key
| set_notes_key          |      -                        | -           | Set the encryption key
| set_mark               | ^SPACE         (      CTRL-@) | Normal mode | set normal mark at current position
| set_no_mark            |      -                        | -           | remove selection marks
| set_line_mark          | ALT-L          (       ALT-L) | Normal mode | set line mark at current position
| set_column_mark        | ALT-C          (       ALT-C) | Normal mode | set column mark at current position
| split_window           | ^X2            (         ^X2) | Normal mode | Split current window horizontally
| vsplit_window          | ^X3            (         ^X3) | Normal mode | Split current window vertically
| system_paste           | ALT-V          (       ALT-V) | Normal mode | Past the system clipboard contents in current position
| system_paste           | SHIFT-INSERT   (  ALT-CTRL-Y) | Normal mode | Past the system clipboard contents in current position
| twiddle                | ^T             (      CTRL-T) | Normal mode | Replace the two characters left of cursor
| trim_line              | ^X^T           (         ^XT) | Normal mode | Remove trailing spaces of a line
| unassign_key           | ALT-^U         (  ALT-CTRL-U) | Normal mode | Unassign a function or subroutine from a key
| saveas_file            | ^X^W           (         ^XW) | Normal mode | Save the file as another name
| paste_region           | ^V             (      CTRL-V) | Normal mode | Past the clipboard contents in current position
| cut_region             | ^W             (      CTRL-W) | Normal mode | Cut the region and put it in clipboard
| cut_region             | `ALT-[W`       (    `ALT-[W`) | Normal mode | Cut the region and put it in clipboard
| update_screen          |      -                        | -           | update_screen
| update_tags            | ^XG            (         ^XG) | Normal mode | Update C-Tags
| find_tag               | ALT-.          (       ALT-.) | Normal mode | Find the function under cursor in other files of current dir
| view_functions         | ALT-@          (       ALT-@) | Normal mode | view functions
| view_functions         | ALT-#          (       ALT-#) | Normal mode | view functions
| prev_bmark             | ALT-6          (       ALT-6) | Normal mode | Go to previous bookmark
| next_bmark             | ALT-7          (       ALT-7) | Normal mode | Go to next bookmark
| select_bmark           | ALT-9          (       ALT-9) | Normal mode | Select a bookark to go to
| set_bmark              | ALT-8          (       ALT-8) | Normal mode | Bookmark current position
| set_bmark              | ^XT            (         ^XT) | Normal mode | Bookmark current position
| clear_bmark            | ALT-0          (       ALT-0) | Normal mode | Clear all bookmarks
| prev_hmark             | ^U             (      CTRL-U) | Normal mode | Go to previous histoty mark
| prev_hmark             | ALT-P          (       ALT-P) | Normal mode | Go to previous histoty mark
| prev_hmark             | ALT left arrow (      ALT-\[p) | Normal mode | Go to previous histoty mark
| prev_hmark             | Previous mark  (   spec2 199) | Normal mode | Go to previous histoty mark
| next_hmark             | ALT-N          (       ALT-N) | Normal mode | Goto next history mark
| next_hmark             | ALT right arrow(      ALT-\[n) | Normal mode | Goto next history mark
| next_hmark             | Next mark      (   spec2 198) | Normal mode | Goto next history mark
| select_hmark           | ALT-J          (       ALT-J) | Normal mode | Select history mark
| set_option             | ^XS            (         ^XS) | Normal mode | set option. If interactive select option from a list
| toggle_move_lock       | ^X^M           (         ^XM) | Normal mode | toggle movement lock
| toggle_move_lock       | ^XM            (         ^XM) | Normal mode | toggle movement lock
| toggle_move_lock       | ALT-^M         (  ALT-CTRL-M) | Normal mode | toggle movement lock
| toggle_move_lock       | F12            (      ALT-Oa) | Normal mode | toggle movement lock
| toggle_move_lock       | key F12        (    ALT-\[24~) | Normal mode | toggle movement lock
| toggle_sync_lock       | ^XL            (         ^XL) | Normal mode | toggle page movement lock
| toggle_sync_lock       | ALT-^L         (  ALT-CTRL-L) | Normal mode | toggle page movement lock
| toggle_sync_lock       | key F11        (    ALT-\[23~) | Normal mode | toggle page movement lock
| toggle_sync_lock       | key shift F11  (      ALT-OY) | Normal mode | toggle page movement lock
| page_move_up           | Pg Dn          (     ALT-\[6~) | Normal mode | page move up
| page_move_up           | Pg1 Dn         (    spec2 89) | Normal mode | page move up
| page_move_down         | Pg Up          (     ALT-\[5~) | Normal mode | page move down
| page_move_down         | Pg1 Up         (      ALT-Oy) | Normal mode | page move down
| set_over               |      -                        | -           | set/unset overwrite
| set_case               |      -                        | -           | set case sensitive search flag
| set_regexp             |      -                        | -           | set regular expression search flag
| toggle_over            | ^XI            (         ^XI) | Normal mode | toggle overwrite
| toggle_over            | ALT-^O         (  ALT-CTRL-O) | Normal mode | toggle overwrite
| toggle_over            | Insert key     (     ALT-\[2~) | Normal mode | toggle overwrite
| toggle_over            | OVERWRITE      (     ALT-\[0~) | Normal mode | toggle overwrite
| toggle_over            | Inser1 key     (   spec2 212) | Normal mode | toggle overwrite
| show_tag_view          |      -                        | -           | show tag view
| show_calendar_list     |      -                        | -           | show calendar list
| show_todo_list         |      -                        | -           | show todo list
| new_note               |      -                        | -           | new note
| new_cal_note           |      -                        | -           | new calendar note
| new_todo_note          |      -                        | -           | new todo note
| set_crypt              |      -                        | -           | set encryption mode
| set_justify            |      -                        | -           | set right justify mode
| toggle_crypt           |      -                        | -           | change encryption mode
| toggle_just            | ^XJ            (         ^XJ) | Normal mode | change right justify mode
| menufile               | ALT-A          (       ALT-A) | Normal mode | Show a menu from file and select a function to execute, also execute n'th element in macro mode
| listdir1               | ^XD            (         ^XD) | Normal mode | Show current dir
| listdir2               |      -                        | -           | Show new dir
| dir_exec               | ^M             (      CTRL-M) |    dir mode | Execute current file (dir mode)
| dir_exec               | x              (           x) |    dir mode | Execute current file (dir mode)
| dir_edit               | e              (           e) |    dir mode | Edit current file (dir mode)
| dir_edit               | i              (           i) |   view mode | Edit current file (dir mode)
| dir_copy               | c              (           c) |    dir mode | Copy current file (dir mode)
| dir_move               | m              (           m) |    dir mode | Move current file (dir mode)
| dir_rename             | n              (           n) |    dir mode | Rename current file (dir mode)
| dir_del1               | ^D             (      CTRL-D) |    dir mode | Delete current file (dir mode)
| dir_view               | v              (           v) |    dir mode | View current file (dir mode)
| mkdir                  | d              (           d) |    dir mode | Create new dir (dir mode)
| touch                  | f              (           f) |    dir mode | create new file (dir mode)
| link                   | l              (           l) |    dir mode | symbolic link (dir mode)
| dir_right              | i              (           i) |    dir mode | Dir right: view file on enter directory
| dir_right              | Arrow right    (      ALT-\[C) |    dir mode | Dir right: view file on enter directory
| dir_right              | spec c         (      ALT-OC) |    dir mode | Dir right: view file on enter directory
| dir_left               | b              (           b) |    dir mode | Dir left: exit viewing file or goto to parent directory
| dir_left               | Arrow left     (      ALT-\[D) |    dir mode | Dir left: exit viewing file or goto to parent directory
| dir_left               | spec d         (      ALT-OD) |    dir mode | Dir left: exit viewing file or goto to parent directory
| dir_left               | KEY-LEFT       (      ALT-\[D) |   view mode | Dir left: exit viewing file or goto to parent directory
| view_next              | .              (           .) |   view mode | view next file
| view_previous          | ,              (           ,) |   view mode | view previous file
| dir_tag                | space          (            ) |    dir mode | Tag current file (dir mode)
| dir_reload             | r              (           r) |    dir mode | refresh dir (dir mode)
| in_view                |      -                        | -           | buffer in view
| dir_showfileinfo       | j              (           j) |    dir mode | Show file info (dir mode)
| show_info              | key F1         (      ALT-OP) | Normal mode | Show info
| load_session           | ^X^B           (         ^XB) | Normal mode | load, replay session keys
| save_session           | ^X^A           (         ^XA) | Normal mode | save session keys
| show_session           | ^X^V           (         ^XV) | Normal mode | show session keys
| select_tag             | +              (           +) |  notes mode | 
| select_tag             | =              (           =) |  notes mode | 
| select_tag             | s              (           s) |  notes mode | 
| toggle_tagnote         | tab            (      CTRL-I) |  notes mode | 
| edit_note              | e              (           e) |  notes mode | 
| view_note              | v              (           v) |  notes mode | 
| view_note              | KEY-RIGHT      (      ALT-\[C) |  notes mode | 
| toggle_tag             |                (            ) |  notes mode | 
| toggle_tag             | t              (           t) |  notes mode | 
| unselect_tag           | u              (           u) |  notes mode | 
| remove_tags            | -              (           -) |  notes mode | 
| remove_tags            | *              (           *) |  notes mode | 
| remove_tags            | r              (           r) |  notes mode | 
| delete_tagnote         | del            (      CTRL-D) |  notes mode | 
| save_as_note           |      -                        | -           | 
| recreate_notes_db      |      -                        | -           | 
| do_nothing             |      -                        | -           | do nothing

## Sorted by key
### Normal mode
| key shortcut    | macro name           | description                          |
| --------------- | -------------------- | ------------------------------------ |
| ^A              | bol                  |  CTRL-A
| ^B              | select_file          |  CTRL-B
| ^C              | copy_region          |  CTRL-C
| ^D              | delete_line          |  CTRL-D
| ^E              | eol                  |  CTRL-E
| ^F              | search_forward       |  CTRL-F
| ^G              | abort                |  CTRL-G
| ^H              | del_prev_char        |  CTRL-H
| tab             | tab                  |  CTRL-I
| ^J              | insert_indent        |  CTRL-J
| ^K              | erase_eol            |  CTRL-K
| ^L              | redraw_display       |  CTRL-L
| ^M              | newline              |  CTRL-M
| ^N              | next_file            |  CTRL-N
| ^O              | open_file            |  CTRL-O
| ^P              | next_window          |  CTRL-P
| ^Q              | quote_character      |  CTRL-Q
| ^R              | search_reverse       |  CTRL-R
| ^S              | save_file            |  CTRL-S
| ^T              | twiddle              |  CTRL-T
| ^U              | prev_hmark           |  CTRL-U
| ^V              | paste_region         |  CTRL-V
| ^W              | cut_region           |  CTRL-W
| ^Y              | redo                 |  CTRL-Y
| ^Z              | undo                 |  CTRL-Z
| ^]              | match_fence          |  CTRL-]
| ^SPACE          | set_mark             |  CTRL-@
| ^\              | goto_eow             |  CTRL-\
| SCROLL-UP       | scroll_up            |  mouse -32671
| SCROLL-DOWN     | scroll_down          |  mouse -32672
| MOUSE_POS       |                      |  mouse -32701
| MOUSE_LEFT      |                      |  mouse -32736
| MOUSE_RELEASE   |                      |  mouse -32733
| MOUSE_RIGHT     |                      |  mouse -32734
| mouse error     |                      |  mouse -32768
| ALT space       | set_arg              |  ALT- 
| ALT-.           | find_tag             |  ALT-.
| ALT-@           | view_functions       |  ALT-@
| ALT-#           | view_functions       |  ALT-#
| ALT-1           | exec                 |  ALT-1
| ALT-2           | script_exec          |  ALT-2
| ALT-3           | filter_buffer        |  ALT-3
| ALT-4           | macro_line           |  ALT-4
| ALT-5           | grep_command         |  ALT-5
| ALT-%           | rgrep_command        |  ALT-%
| ALT-0           | clear_bmark          |  ALT-0
| ALT-9           | select_bmark         |  ALT-9
| ALT-8           | set_bmark            |  ALT-8
| ALT-7           | next_bmark           |  ALT-7
| ALT-6           | prev_bmark           |  ALT-6
| ALT-|           | prev_word            |  ALT-|
| ALT-\           | goto_eow             |  ALT-\
| ALT-&           | read_colors          |  ALT-&
| ALT-*           | save_colors          |  ALT-*
| ALT-A           | menufile             |  ALT-A
| ALT-B           | prev_word            |  ALT-B
| ALT-C           | set_column_mark      |  ALT-C
| ALT-D           | del_next_word        |  ALT-D
| ALT-G           | goto_line            |  ALT-G
| ALT-I           | insert_tab           |  ALT-I
| ALT-J           | select_hmark         |  ALT-J
| ALT-K           | assign_function      |  ALT-K
| ALT-L           | set_line_mark        |  ALT-L
| ALT-N           | next_hmark           |  ALT-N
| ALT-O           |                      |  ALT-o
| ALT-P           | prev_hmark           |  ALT-P
| ALT-Q           | fill_paragraph       |  ALT-Q
| ALT-R           | replace_string       |  ALT-R
| ALT-V           | system_paste         |  ALT-V
| ALT-X           | quit                 |  ALT-X
| ^Y              | toggle_mouse         |  ALT-Y
| ALT-Z           | set_bg               |  ALT-Z
| ALT->           | eof                  |  ALT->
| ALT-<           | bof                  |  ALT-<
| ALT {           | both_up              |  ALT-{
| ALT }           | both_down            |  ALT-}
| ALT-=           | refresh_cline        |  ALT-=
| ALT-+           | refresh_buffer       |  ALT-+
| ALT-'           | comment_line         |  ALT-'
| ALT-;           | comment_line2        |  ALT-;
| ^X^I            | remove_tab           |  ^XI
| ^X^A            | save_session         |  ^XA
| ^X^B            | load_session         |  ^XB
| ^X^C            | quit                 |  ^XC
| ^X^D            | detab_line           |  ^XD
| ^X^E            | entab_line           |  ^XE
| ^X^T            | trim_line            |  ^XT
| ^X^L            | region_lower         |  ^XL
| ^XD             | listdir1             |  ^XD
| ^XF             | line_fill            |  ^XF
| ^X^M            | toggle_move_lock     |  ^XM
| ^X^O            | del_blank_lines      |  ^XO
| ^X^S            | save_file            |  ^XS
| ^X^U            | region_upper         |  ^XU
| ^X^V            | show_session         |  ^XV
| ^X^W            | saveas_file          |  ^XW
| ^X^X            | swap_mark            |  ^XX
| ^X0             | delete_window        |  ^X0
| ^X1             | one_window           |  ^X1
| ^X2             | split_window         |  ^X2
| ^X3             | vsplit_window        |  ^X3
| ^XH             |                      |  ^XH
| ^XI             | toggle_over          |  ^XI
| ^XJ             | toggle_just          |  ^XJ
| ^XL             | toggle_sync_lock     |  ^XL
| ^XK             | close_file           |  ^XK
| ^XM             | toggle_move_lock     |  ^XM
| ^XN             | rename_file          |  ^XN
| ^XO             | next_window          |  ^XO
| ^XP             | prev_window          |  ^XP
| ^XR             | window_row_resize    |  ^XR
| ^XV             | window_column_resize |  ^XV
| ^X^R            | replace_next         |  ^XR
| ^XS             | set_option           |  ^XS
| ^XG             | update_tags          |  ^XG
| ^XT             | set_bmark            |  ^XT
| ^XU             | redo                 |  ^XU
| ^XZ             | set_bg               |  ^XZ
| ^X[             | prev_paragraph       |  ^X[
| ^X]             | next_paragraph       |  ^X]
| ^X!             | exec                 |  ^X!
| ^X#             | filter_buffer        |  ^X#
| ^X&             | bg_cmd               |  ^X&
| ^X=             | refresh_buffer       |  ^X=
| ^X\             | parse_check          |  ^X\
| ^X^R            | query_replace        |  ^XR
| ALT-^C          | count_words          |  ALT-CTRL-C
| ALT-^H          | del_prev_word        |  ALT-CTRL-H
| ALT-^K          | assign_function      |  ALT-CTRL-K
| ALT-^L          | toggle_sync_lock     |  ALT-CTRL-L
| ALT-^M          | toggle_move_lock     |  ALT-CTRL-M
| ALT-^U          | unassign_key         |  ALT-CTRL-U
| ALT-^O          | toggle_over          |  ALT-CTRL-O
| ALT-^P          | hard_reposition      |  ALT-CTRL-P
| ALT-^R          | query_replace        |  ALT-CTRL-R
| ALT-^X          | macro_line           |  ALT-CTRL-X
| Escape          | abort                |  ALT-CTRL-\[
| backspace       | del_prev_char        |  
| KEY-UP          | prev_line            |  ALT-\[A
| KEY-LEFT        | prev_character       |  ALT-\[D
| KEY-RIGHT       | next_character       |  ALT-\[C
| KEY-DOWN        | next_line            |  ALT-\[B
| Pg Up           | page_move_down       |  ALT-\[5~
| Pg Dn           | page_move_up         |  ALT-\[6~
| Insert key      | toggle_over          |  ALT-\[2~
| OVERWRITE       | toggle_over          |  ALT-\[0~
| ^KEY-LEFT       | prev_word            |  ALT-\[P
| ^KEY-RIGHT      | next_word            |  ALT-\[N
| SHIFT-INSERT    | system_paste         |  ALT-CTRL-Y
| ALT-S           | menu                 |  ALT-S
| ALT-F           | menu                 |  ALT-F
| ALT-E           | menu                 |  ALT-E
| ALT-H           | menu                 |  ALT-H
| ALT-M           | menu                 |  ALT-M
| ALT-T           | menu                 |  ALT-T
| ALT-W           | menu                 |  ALT-W
| F2              | set_arg              |  ALT-\[12~
| F3              | quick_close          |  ALT-\[13~
| F4              | describe_key         |  ALT-\[14~
| key F5          | find_next            |  ALT-\[15~
| key F6          | find_previous        |  ALT-\[17~
| key F7          | record_macro         |  ALT-\[18~
| key F8          | exec_macro           |  ALT-\[19~
| key F9          | exec                 |  ALT-\[20~
| key F10         | next_file            |  ALT-\[21~
| key F11         | toggle_sync_lock     |  ALT-\[23~
| ALT-\[I          | copy_region          |  ALT-\[I
| ALT-\[W          | cut_region           |  ALT-\[W
| ^HOME           | bof                  |  spec2 11
| ^END            | eof                  |  spec2 9
| ALT-(           | window_column_decrease |  ALT-(
| ALT-)           | window_column_increase |  ALT-)
| ALT--           | window_row_decrease  |  ALT--
| ALT-_           | window_row_increase  |  ALT-_
| ^PG-DOWN        | next_file            |  ALT-\[z
| ^PG-UP          | next_window          |  ALT-\[w
| SHIFT rigt      | select_next_character |  ALT-\[c
| SHIFT down      | select_next_line     |  ALT-\[b
| SHIFT left      | select_prev_character |  ALT-\[d
| SHIFT up        | select_prev_line     |  ALT-\[a
| ALT right arrow | next_hmark           |  ALT-\[n
| ALT left arrow  | prev_hmark           |  ALT-\[p
| Shift TAB       | remove_tab           |  ALT-\[Z
| F12             | toggle_move_lock     |  ALT-Oa
| key F1          | show_info            |  ALT-OP
| ALT-\[F          | eol                  |  ALT-\[F
| spec2 7         | prev_word            |  spec2 7
| spec2 197       | next_file            |  spec2 197
| spec2 196       | next_window          |  spec2 196
| ALT-!           | page_loop            |  subroutine
| ALT-,           | show_out             |  subroutine
| ALT-/           | w_abbrev             |  subroutine
| ^XX             | test1                |  subroutine
| ^XC             | clear_results        |  subroutine
| ALT-?           | clear_results        |  subroutine
| ALT-~           | test1                |  subroutine
| Home key        | bol                  |  ALT-OH
| Home1 key       | bol                  |  ALT-\[H
| Home2 key       | bol                  |  ALT-OM
| End key         | eol                  |  ALT-OF
| End1 key        | eol                  |  spec2 213
| Arrow right     | next_character       |  ALT-OC
| Arrow1 right    | next_character       |  spec2 233
| Arrow left      | prev_character       |  ALT-OD
| Arrow1 left     | prev_character       |  spec2 33
| ^Page DN        | next_file            |  spec2 21
| Center key      | next_window          |  ALT-OE
| key crt         | next_word            |  spec2 6
| Arrow down      | next_line            |  ALT-OB
| Arrow1 down     | next_line            |  spec2 214
| Pg1 Dn          | page_move_up         |  spec2 89
| Pg1 Up          | page_move_down       |  ALT-Oy
| Arrow up        | prev_line            |  ALT-OA
| Inser1 key      | toggle_over          |  spec2 212
| ^Down arrow     | find_next            |  spec2 5
| key F2          | set_arg              |  ALT-OQ
| key F3          | quick_close          |  ALT-OR
| key F4          | describe_key         |  ALT-OS
| ^Up arrow       | find_previous        |  spec2 4
| ^numeric 9      | next_window          |  spec2 53
| key F12         | toggle_move_lock     |  ALT-\[24~
| key shift F11   | toggle_sync_lock     |  ALT-OY
| delete key      | del_char             |  ALT-\[3~
| Next mark       | next_hmark           |  spec2 198
| Previous mark   | prev_hmark           |  spec2 199
| Enter key       | insert_indent        |  ALT-OM
| ^numeric 7      | bof                  |  spec2 51
| ^numeric 1      | eof                  |  spec2 197
### dir mode
| key shortcut    | macro name           | description                          |
| --------------- | -------------------- | ------------------------------------ |
| ^M              | dir_exec             |  CTRL-M
| e               | dir_edit             |  e
| c               | dir_copy             |  c
| l               | link                 |  l
| m               | dir_move             |  m
| n               | dir_rename           |  n
| f               | touch                |  f
| d               | mkdir                |  d
| ^D              | dir_del1             |  CTRL-D
| b               | dir_left             |  b
| i               | dir_right            |  i
| j               | dir_showfileinfo     |  j
| r               | dir_reload           |  r
| x               | dir_exec             |  x
| v               | dir_view             |  v
| s               | script_exec          |  s
| Arrow right     | dir_right            |  ALT-\[C
| Arrow left      | dir_left             |  ALT-\[D
| spec c          | dir_right            |  ALT-OC
| spec d          | dir_left             |  ALT-OD
| space           | dir_tag              |   
| tab             | next_window          |  CTRL-I
| \[               | prev_page            |  \[
| \]               | next_page            |  \]
### view mode
| key shortcut    | macro name           | description                          |
| --------------- | -------------------- | ------------------------------------ |
| b               | prev_word            |  b
| e               | goto_eow             |  e
| 0               | bol                  |  0
| $               | eol                  |  $
| h               | prev_character       |  h
| l               | next_character       |  l
| l               | next_character       |   
| i               | dir_edit             |  i
| k               | prev_line            |  k
| j               | next_line            |  j
| n               | find_next            |  n
| N               | find_previous        |  N
| b               | next_word            |  w
| /               | search_forward       |  /
| ?               | search_reverse       |  ?
| G               | eof                  |  G
| 1               | bof                  |  1
| v               | quick_close          |  v
| KEY-LEFT        | dir_left             |  ALT-\[D
| q               | close_file           |  q
| \[               | prev_paragraph       |  \[
| \]               | next_paragraph       |  \]
| ,               | view_previous        |  ,
| .               | view_next            |  .
### notes mode
| key shortcut    | macro name           | description                          |
| --------------- | -------------------- | ------------------------------------ |
|                 | toggle_tag           |   
| +               | select_tag           |  +
| =               | select_tag           |  =
| s               | select_tag           |  s
| t               | toggle_tag           |  t
| u               | unselect_tag         |  u
| -               | remove_tags          |  -
| *               | remove_tags          |  *
| r               | remove_tags          |  r
| tab             | toggle_tagnote       |  CTRL-I
| del             | delete_tagnote       |  CTRL-D
| e               | edit_note            |  e
| v               | view_note            |  v
| KEY-RIGHT       | view_note            |  ALT-\[C
| k               | prev_line            |  k
| j               | next_line            |  j
| n               | find_next            |  n
| N               | find_previous        |  N
| /               | search_forward       |  /
| ?               | search_reverse       |  ?
| G               | eof                  |  G
| 1               | bof                  |  1
| q               | close_file           |  q
| \[               | prev_page            |  \[
| \]               | next_page            |  \]
