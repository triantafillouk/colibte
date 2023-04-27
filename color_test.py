import curses

START_COLOR=17
BG_COLORS=10
FG_COLORS=15

def main(stdscr):
    curses.start_color()
    curses.use_default_colors()
    for i in range(0,FG_COLORS):
        for j in range(0, BG_COLORS):
            pair=i*FG_COLORS+j+1;
            curses.init_pair(pair, i+BG_COLORS+START_COLOR,j+START_COLOR)
    try:
        for j in range (0,BG_COLORS):
            stdscr.move(j,0)
            for i in range(0, FG_COLORS):
                pair=i*FG_COLORS+j+1;
                stdscr.addstr(str(i), curses.color_pair(pair))
    except curses.ERR:
    #    print("--\n")
    # End of screen reached
        pass

    stdscr.getch()

curses.wrapper(main)