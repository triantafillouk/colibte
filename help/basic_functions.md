# Colibri's text editor C-like macro language
==================================================

## text editor general commands
    record_macro()                    : start macro recording
    exec_macro()                      : execute a macro command
    quit()                            : quit program
    set_arg(num)                      : set multiple argument

## debug commands
    parse_check_current_buffer()       : parse check current buffer, the output goes to [out] buffer
    show_parse_buffer()                : parse buffer and show the result to [out] buffer

## File commands

    open_file(file_name)            : open a file for edit
    new_file()                      : New file
    save_file()                     : save the current file
    saveas_file(file_name)          : save as file
    close_file()                    : close current file buffer
    next_file()                     : fetch next file in current window
    rename_file(new_name)           : rename file
    quick_close()                   : quick close
    insert_file(file_name)          : insert file at current position
    set_key(encryption_key)         : set encryption key

## Help commands
    show_info()                       : show file info
    show_keys()                       : show keyboard assignement in a buffer
    show_version()                    : show version
    help()                            : show help
    describe_key()                    : describe a key (interactive)

## Window commands
    one_window()                        : maximize current window
    delete_window()                     : delete current window
    window_column_increase()            : increase window columns (console)
    window_column_decrease()            : decrease window columns (console)
    window_row_increase()               : icrease window rows (console)
    window_row_decrease()               : decrease window rows (console)
    next_window()                       : move to next window
    prev_window()                       : move to previous window
    window_row_resize(rows_number)      : window row resize (console)
    window_column_resize(column_number) : window column resize (console)
    split_window()                      : split window horizontally
    vsplit_window()                     : vertically split window

## Conversion commands

```
    dos2unix()                            : convert buffer to unix
    unix2dos()                            : convert buffer to dos
    convert_char(type)                    : buffer conversion
            type=0    - IBM437    to ISO8859-7
            type=1    - ISO8859-7 to IBM437
            type=2  - ISO8859-7 to utf8
```

## Movement commands
    move_window(lines)                   : move text in window up/donw number of lines
    bof()                                : goto beginning of file
    bol()                                : goto beginning of line
    eof()                                : goto end of file
    eol()                                : goto end of line
    swap_mark()                          : swap mark and current cursor position
    match_fence()                        : match fence
    hard_reposition()                    : hard cursor reposition
    redraw_display()                     : redraw display
    prev_character(num)                  : goto previous character
    next_character(num)                  : go to next character(s) 
    goto_line(line_number)               : goto line
    both_down(number_of_pages)           : previous page on both windows
    both_up(pages)                       : next page on move both windows
    page_move_up(pages)                  : move up page
    page_move_down(pages)                : mo down page
    next_paragraph(paragraphs)           : goto next paragraph
    next_page(pages)                     : goto next page(s)
    next_word(pages)                     : goto next word(s)
    goto_eow(words)                      : goto end of word(s)
    prev_page(pages)                     : goto previous page(s)
    prev_line(lines)                     : goto previous line(s)
    prev_paragraph(paragraphs)           : goto previous paragraph
    prev_word(words)                     : goto previous word
    toggle_move_lock()                   : toggle move lock 
    toggle_sync_lock()                   : toggle page synchronized lock in different windows
    scroll_up(scroll_lines)              : scroll up
    scroll_down(scroll_lines)            : scroll down

## Search/replace commands
    replace_string(search_string,replace_with)    : replace string
    replace_next(number_of_replaces)              : replace next
    find_previous(number_of_finds)                : find previous
    find_next(number_of_finds)                    : find next
    rgrep_command(search_string)                  : recursive find in files
    grep_command(search_string)                   : find in files 
    search_forward(search_string)                 : search forward
    search_reverse(search_string)                 : search backword
    find_tag(0)                                   : find current word in tags
    query_replace(search_string,replace_with)     : query replace (interactive replace)

## Edit commands
    twiddle()                                     : twiddle previous 2 characters
    trim_line()                                   : trim line current line
    undo()                                        : undo editing
    redo()                                        : redo editing
    region_lower()                                : Lower case a defined region
    region_upper()                                : Upper case a defined region
    copy_region()                                 : Copy region
    count_words()                                 : count words in marked region
    del_char(characters)                          : delete characters
    del_next_word(words)                          : delete next words
    del_prev_word(words)                          : delete previous words
    del_prev_char(characters)                     : delete previous character
    del_blank_lines(lines)                        : delete blank lines
    entab_line(lines)                             : change tabs to spaces for a number of lines
    detab_line(lines)                             : change spaces to tabs for a number of lines
    line_fill(indent)                             : Line fill (up to fillcol), left indent if indent is true.
    fill_paragraph(fill type)                     : fill current paragraph 
        n=0,2    use left_margin
    erase_eol()                                   : erase to end of line
    quote_character()                             : insert control characters instead of command (interactive only)
    insert_tab(tabs)                              : insert a tab (or corresponding spaces)
    remove_tab(tabs)                              : unindent region by number of tabs
    clear_buffer(file name)                       : empty file buffer
    unmark_buffer()                               : flag buffer as unchanged
    insert(string to insert)                      : insert string
    delete_line(lines)                            : delete line(s)
    newline(newlines)                             : insert new line
    insert_indent(lines)                          : insert new lines based on current indent
    tab(tabs)                                     : indent line
    next_line(lines)                              : goto next line(s)

## Dir mode commands
    listdir1()                                    : directory list 1
    listdir2()                                    : directory list 2
    dir_exec()                                    : dir exec
    dir_edit()                                    : dir edit
    dir_del1()                                    : delete file in dir mode
    dir_view()                                    : dir view file
    dir_copy(destination)                         : copy current file in dir mode to destination
    dir_move(destination)                         : move current file in dir mode to destination
    dir_rename(new name)                          : rename file in dir mode
    mkdir(dir name)                               : make a directory in dir mode
    touch(file name)                              : touch a file (new file) in dir mode
    link(file anme)                               : link a file in dir mode
    dir_right()                                   : dir right
    dir_left()                                    : dir left
    view_next()                                   : view next file in dir mode
    view_previous()                               : view previous file in dir mode
    dir_tag()                                     : select file in dir mode
    dir_reload()                                  : reload direcory view
    dir_showfileinfo()                            : show file info (dir mode)
    select_file(file name)                        : select file as current buffer

## Clipboard commands
    select_next_character(characters)             : select next character(s)
    select_prev_character(characters)             : select previous character(s)
    select_next_line(lines)                       : select next line(s) 
    select_prev_line(lines)                       : select previous line(s) 
    set_mark()                                    : set region mark
    set_no_mark()                                 : set no region mark
    set_line_mark()                               : set line mark
    set_column_mark()                             : set column mark
    system_paste()                                : paste from system clipboard
    paste_region()                                : paste region
    cut_region()                                  : cut region

## Bookmark/History marks commands
    prev_bmark()                                  : goto previous bookmark
    next_bmark()                                  : goto next bookmark
    select_bmark(num)                             : select bookmark (interactive)
    set_bmark()                                   : set bookmark
    clear_bmark()                                 : clear bookmarks
    prev_hmark()                                  : goto previous history mark (previous position)
    next_hmark()                                  : goto next history mark (next history position)
    select_hmark()                                : select history mark (interactive)

## Config commands
    menufile(menu)                                : menufile (interactive)
    toggle_bold()                                 : toggle bold fonts (on GTK)
    select_font_mono()                            : select mono font (on GTK)
    increase_font_size()                          : increase font size (GTK)
    decrease_font_size()                          : decrease font size (GTK)
    setbg()                                       : New background shell (interactive)
    save_colors()                                 : save color changes (GTK)
    read_colors()                                 : read colors
    color_scheme(schem number)                    : select color scheme
    assign_function(function,key sequence)        : assign editor function to keyboard sequence
    assign_sub(sub,key sequence)                  : assing subroutine to keyboard sequence
    select_font(font name)                        : select font (GTK)
    set_fontindex(font index)                     : set font index (GTK)
    set_color(color)                              : set color (GTK)
    color_scheme_read(scheme ind ??)              : read color scheme
    set_locale(local index)                       : set locale
    unassign_key(key sequence)                    : unassign keyboard sequence
    set_over()                                    : overwrite option
    set_case()                                    : case optiom
    set_regexp()                                  : regular expression option
    toggle_over()                                 : toggle overwrite flag
    set_crypt()                                   : set file as encrypted
    set_justify()                                 : set file as right justified
    toggle_crypt()                                : toggle encryption flag
    toggle_just()                                 : toggle justify flag
    set_option(option,value)                      : set option
    toggle_mouse()                                : enable/disable mouse control (console interactive)

## Macro commands
    exec_file(file name)                          : execute a macro file
    macro_line(macro string)                      : execute a macro string
    execsub(sub)                                  : execute a named subroutine
    refresh_cline()                               : refresh current line (execute it as a macro line)
    refresh_buffer()                              : execute current buffer as a macro buffer
    do_nothing()                                  : do nothing
    script_exec(script exec)                      : script execute
    filter_buffer(filter name)                    : filter buffer
    abort()                                       : abort
    new_shell()                                   : new external shell
    exec(exec name)                               : execute a shell command and return output to [command] buffer
    work()                                        : work menu

## Session commands
    load_session(file name)                       : load session and execute it
    save_session()                                : save current session
    show_session()                                : show session

## Note commands
    update_tags()                                 : update tags
    select_tag()                                  : select current tag 
    toggle_tagnote()                              : toggle note viewing
    edit_note()                                   : edit a note
    view_note()                                   : view a note
    toggle_tag()                                  : toggle note tag 
    unselect_tag()                                : unselect current tag
    remove_tags()                                 : remove all tags, show all notes
    delete_tagnote()                              : delete a tag note
    save_as_note()                                : save file as note
    recreate_notes_db()                           : recreate notes database
    set_notes_key(encryption_key)                 : set notes encryption key

# Plot drawing commands
 The following commands exist only in gtk driver. They are also totaly experimental.

## basic plot commands
    plot_init(title), start show plot window
    title(title), title at top of the screen (also shows plot window)
    move_to(x,y)    , set current position
    line_to(x,y)    , line from current position to x,y
    circle(x,y,r) , circle at position, radius
    rcircle(r)    , circle with radius at current position
    arc(r,a1,a2) , arc with radius, from angle a1 to a2
    rectangle(w,h) , rectangle from current position
    triangle(x2,y2,x3,y3), triangle at current position
    polyline(x1,y1,x2,y2, ....), polyline at current position
    make_solid    : polyline is only filled
    poly_nostroke: >>
    plot_set_color("type",r,g,b)
        type is "grid" "coord" "tik" "back" "title"
    plot_resize(w,h)    : resize plot window to width x height
    image("filename",scale,rotate)
    text("string")
    relative    : now coords are relative
    absolute    : now coords are absolute
    plot_frames(flag) : show or not frames
    alpha,set_alpha(alpha)    , set transparency
    rotate,set_rotate(angle)    , set current rotation
    translate,set_translate(dx,dy) , set current translation
    scale,set_scale(scalex,scaley), set current scale

## interactive key commands
    'm'    : togle mid, togle origin in rectanges, images (mid position or corners)
    '+','=' : zoom in
    '-' : zoom out
    'f' : toggle frames flag
    'g' : grid on off
    'n' : togle num mode, numerics on coordinate axes
    'z' : togle zoom mode , zoom on window resize or stay fixed
    'o' : togle origin , changes the start corner of images,rectangles (top left or bottom left)
    'l' : redraw



