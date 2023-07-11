# Make file

# Last modified 2020/09/10

# default values for flags
UNAME := $(shell uname)
APP_NAME=colibte

PCURSES=0
XLIB=0
GTK=0
XPLOT=0
EMBED_ICONS=0
USE_GLIB=1
# include support for notes database in sqlite3
TNOTES=1
GVERS := $(shell git log -1 --pretty=tformat:%h,%s)

GTKINCLUDE3=`pkg-config gtk+-3.0 --cflags`
GTKINCLUDE3=`pkg-config gtk+-3.0 --cflags`
_X11_=0

OSYSTEM=OTHERUNIX
SHAREDDIR=/usr/share

ifeq ($(USE_GLIB),1)
GLIB_LIB=`pkg-config glib-2.0 --libs`
else
GLIB_LIB=
endif

# for Darwin
ifeq ($(UNAME), Darwin)
GLIBINCLUDE=`pkg-config glib-2.0 --cflags`
LPCURSES0=-lpanel -lncurses -L/opt/local/lib $(GLIB_LIB)
OSYSTEM=DARWIN
SHAREDDIR=/usr/local/share
X11include=-I/opt/X11/include/
X11lib0= -lX11 -L/opt/X11/lib $(GLIB_LIB)
EXTFILE=.$(APP_NAME)_ext_mac
WSL:=0
#CC=clang -DGVERS='"$(GVERS)"'
CC=gcc-12 -DGVERS='"$(GVERS)"'
endif

# for Linux
ifeq ($(UNAME), Linux)
GLIBINCLUDE=`pkg-config glib-2.0 --cflags`
LPCURSES0=-lpanelw -lncursesw  $(GLIB_LIB)
OSYSTEM=LINUX
X11include=-I/opt/X11/include/
X11lib0=-L/usr/X11R6/lib -lX11 -L/opt/X11/lib 
WSL:=`uname -a|grep "icrosoft" |wc -l`
EXTFILE:=.$(APP_NAME)_ext_$(WSL)
CC=gcc -DWSL=$(WSL) -DGVERS='"$(GVERS)"'
endif

# for solaris
ifeq ($(UNAME), SunOS)
GLIBINCLUDE=-I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include
X11include=
X11lib0=-L/usr/X11R6/lib -lX11 -L/opt/X11/lib 
LPCURSES0=-lpanel -lcurses -L/usr/lib -lglib-2.0
OSYSTEM=SOLARIS
EXTFILE=.$(APP_NAME)_ext_linux
WSL=0
CC=gcc
endif

# for cygwin
ifeq ($(UNAME), CYGWIN_NT-10.0)
GLIBINCLUDE=`pkg-config glib-2.0 --cflags`
LPCURSES0=-lpanelw -lncursesw  $(GLIB_LIB)
OSYSTEM=CYGWIN
WSL=0
##CC=gcc -DGVERS='"$(GVERS)"'
endif

ctxe : XLIB=1
ctxe : _X11_=1
ctxe : PCURSES=0 

cteg2 : XPLOT=1

cte  : PCURSES=1
ce : PCURSES=1
ce : TNOTES=0
ce : GLIB_LIB=
ce : USE_GLIB=0
ce : SQLITE3=

ifeq ($(TNOTES),1)
SQLITE3=`pkg-config sqlite3 --libs`
endif

ifeq ($(TNOTES), 1)
LPCURSES=$(LPCURSES0) ${SQLITE3}
X11lib=$(X11lib0) ${SQLITE3}
else
LPCURSES=$(LPCURSES0)
X11lib=$(X11lib0)
endif

ifeq ($(CC), clang)
# the following is needed for clang compiler
CPU_OPTIONS= -O3  -I/usr/include/x86_64-linux-gnu -D$(OSYSTEM)=1
else 
CPU_OPTIONS= -O3  -D$(OSYSTEM)=1  
endif

ifeq ($(WSL), 1)
LIN:=microsoft
else
LIN:=native
endif


ctg2 pixmp ctg3 : GTK=1 

ctg3: GTK3=1
ctg3: XPLOT=1

ctg2 ctg3 : _X11_=1 

ctg2 gldisplay.o : GTK2=1

FLAGS1 =  -DXLIB=$(XLIB) -D$(OSYSTEM)=1 -DWSL=$(WSL) -DPCURSES=$(PCURSES) -DTNOTES=$(TNOTES) -DGTK=$(GTK) $(GLIBINCLUDE) -DXPLOT=$(XPLOT) -DEMBED_ICONS=${EMBED_ICONS} -D_X11_=$(_X11_) -DTNOTES=$(TNOTES) -DUSE_GLIB=$(USE_GLIB) $(X11include) -I/usr/include/ncurses

FLAGS3 =  -DXLIB=$(XLIB) -D$(OSYSTEM)=1 -DXPLOT=$(XPLOT) -DEMBED_ICONS=${EMBED_ICONS} -D_X11_=$(_X11_) $(X11include) $(GLIBINCLUDE)

ctg2 pixmp gdk2 : GTKINCLUDE=`pkg-config gtk+-2.0 --cflags` -DGTK2=1 

tplot c3 ge3 ctg3: GTKINCLUDE=`pkg-config gtk+-3.0 --cflags` -DGTK3=1 
tplot c3 ge3 ctg3: GTKINCLUDE3=`pkg-config gtk+-3.0 --cflags` -DGTK3=1 

ifeq ($(TNOTES), 1)
GTK2_FLAGS=`pkg-config gtk+-2.0 --libs` $(X11lib) ${SQLITE3}
GTK3_FLAGS=`pkg-config gtk+-3.0 --libs` $(X11lib) ${SQLITE3}
else
GTK2_FLAGS=`pkg-config gtk+-2.0 --libs` $(X11lib) 
GTK3_FLAGS=`pkg-config gtk+-3.0 --libs` $(X11lib) 
endif

xe.h : 
	echo cleaning all
	echo $(FLAGS1)
	rm -f *.o

.c.o : $o
#	${CC} -c -O2 -ggdb -funsigned-char $*.c
#	${CC}  -c -Wall -I/usr/local/pgsql/src/include -funsigned-char $*.c
#	${CC}  -c -Wall -O2 -mcpu=pentium -I/usr/local/pgsql/src/include -funsigned-char $*.c 
#	${CC}  -c -Wall -O2 -mcpu=pentium -I/usr/include/gtk-1.2 -I/usr/include/glib-1.2 -funsigned-char $*.c 
	${CC} $(FLAGS1) -c  -Wall $(CPU_OPTIONS) $(GTKINCLUDE) -funsigned-char  $(GLIBINCLUDE) $*.c 
#	${CC}  -c -Wall -O2 -mcpu=pentium -I/usr/include/gtk-2.0 -I/usr/include/glib-1.2 -funsigned-char $*.c 

#	This for SCO. -J is for unsigned char
#	cc -c -J -O2 -b elf $*.c

main.o: main.c xe.h globals.h  keytable.h func.h

gmain.o: main.c xe.h globals.h  keytable.h func.h
	${CC} $(FLAGS1) -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE) -funsigned-char main.c -o gmain.o

input.o: keytable.h input.c 
	${CC} $(FLAGS1) -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE) -funsigned-char input.c 

ginput.o: keytable.h input.c
	${CC} $(FLAGS1) -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE) -funsigned-char input.c -o ginput.o

edit.o: edit.c xe.h

gldisplay.o: xe.h screen.c menus.h

screen.o: xe.h screen.c
	${CC} $(FLAGS1) -DGTK3=0 -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE) -funsigned-char screen.c -o screen.o

screen3.o: xe.h screen.c
	${CC} $(FLAGS1) -DGTK3=1 -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE)  -funsigned-char screen.c -o screen3.o

eval.o: xe.h eval.c eval.h alist.h 

config_init.o: config_init.c

mlang.o: mlang.c mlang_err.c mlang_parser.c mlang_array.c mlang_functions.c mlang.h alist.h xe.h func.h

mlangf.o: xe.h mlangf.c mlangf.h mlang.h

mlangg.o: mlang.c mlang_err.c mlang_parser.c mlang_array.c mlang_functions.c mlang.h alist.h xe.h
	${CC} $(FLAGS1) -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE) -funsigned-char mlang.c -o mlangg.o

tldisplay.o: xe.h tldisplay.c screen.c menus.h keytable.h
	${CC} tldisplay.c $(FLAGS1)  -c -Wall $(CPU_OPTIONS) -I/usr/include/ncursesw -funsigned-char -o tldisplay.o

xldisplay.o: xe.h tldisplay.c screen.c menus.h keytable.h
	${CC} tldisplay.c $(FLAGS1) -c -Wall $(CPU_OPTIONS) -funsigned-char -o xldisplay.o

gtkterm.o: gtkterm.c gtkterm.h xthemes.h color.h xkeys.h menus.h keytable.h  icon.h icons.h keytable.h gtk_common.c xthemes.c

highlight.o: highlight.c highlight.h

xlib.o: xe.h xlib.c menus.h keytable.h 

alist.o: alist.c alist.h avl_tree.c rb_tree.c

filebuf.o: xe.h filebuf.c undo.h replaceblock.c

search.o: xe.h search.c

curses.o: curses.c color.h menus.h keytable.h
	${CC} curses.c $(FLAGS1) -c -Wall $(CPU_OPTIONS) -I/usr/include/ncursesw -funsigned-char -o curses.o
 
panel_curses.o: panel_curses.c color.h menus.h keytable.h xthemes.h color.h rgb_colors.h xthemes.c
	${CC} panel_curses.c $(FLAGS1) -c -Wall $(CPU_OPTIONS) -I/usr/include/ncursesw -funsigned-char -o panel_curses.o 

gtk_support.o: gtk_support.c gtk_support.h
	${CC}  -c ${FLAGS1}  ${GTKINCLUDE} -o gtk_support.o gtk_support.c

# For GTK3
gtkterm3.o: xe.h gtkterm3.c gtkterm3.h xthemes.h color.h xkeys.h menus.h keytable.h  icon.h icons.h keytable.h geditdisplay3.h gtk_common.c xthemes.c
	${CC} $(FLAGS1) -c -DGTK=1 -Wall $(CPU_OPTIONS) $(GTKINCLUDE3) -funsigned-char gtkterm3.c -o gtkterm3.o

gplotc3.o: gplotc3.c gplot3.h plot_cairo3.c plot_commonc.c
	${CC}  -c ${FLAGS1}  ${GTKINCLUDE3} -o gplotc3.o  gplotc3.c

gcanvas3.o: gcanvas3.c 
	${CC}  -c ${FLAGS1}  ${GTKINCLUDE3} -o gcanvas3.o gcanvas3.c

gtk_support3.o: gtk_support3.c gtk_support.h
	${CC}  -c ${FLAGS1}  ${GTKINCLUDE3} -o gtk_support3.o gtk_support3.c

geditdisplay.o: geditdisplay.c geditdisplay.h gedit_common.c
#	${CC}  -c ${FLAGS1} -D${OSYSTEM}  ${GTKINCLUDE3} -o geditdisplay.o geditdisplay.c

geditdisplay3.o: geditdisplay3.c geditdisplay3.h gedit_common.c
	${CC}  -c ${FLAGS1} -D${OSYSTEM}  ${GTKINCLUDE3} -o geditdisplay3.o geditdisplay3.c

ifeq ($(TNOTES), 1)
tplot.o: main.c xe.h plot_cairo3.c
	${CC} -DKKK=1 -c ${FLAGS3} -DTNOTES=$(TNOTES) -DGTK=1 ${GTKINCLUDE3} -o tplot.o main.c
else
tplot.o: main.c xe.h plot_cairo3.c
	${CC} -DKKK=1 -c ${FLAGS3} -DGTK=1 ${GTKINCLUDE3} -o tplot.o main.c
endif

xinput.o: keytable.h input.c
	${CC} $(FLAGS1) -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE3) -funsigned-char input.c -o xinput.o

ginput3.o: keytable.h input.c
	${CC} $(FLAGS1) -c -Wall $(CPU_OPTIONS) $(GTKINCLUDE3)  -funsigned-char input.c -o ginput3.o

#	The following is with gtk3 library and cairo plot. gplotc(gcanvas)
ctg3: tplot.o system.o gldisplay.o edit.o gtkterm3.o dir.o screen3.o  eval.o mlangg.o  file.o ginput3.o help.o search.o  word.o window.o marks.o  utils.o alist.o filebuf.o gplotc3.o  support.o config_init.o convert.o  gtk_support3.o geditdisplay3.o  gcanvas3.o  highlight.o utf8_support.o notes.o
	${CC} tplot.o system.o gldisplay.o  edit.o gtkterm3.o dir.o screen3.o  eval.o mlangg.o  file.o ginput3.o help.o search.o  word.o window.o marks.o convert.o  utils.o alist.o filebuf.o gplotc3.o  gtk_support3.o config_init.o support.o geditdisplay3.o gcanvas3.o  highlight.o utf8_support.o notes.o -o ctg3  $(GLIB_LIB) $(GTK3_FLAGS) -lm -lX11

#	The following is with gtk2 library and cairo plot. gplotc(gcanvas)
ctg2 : gmain.o system.o edit.o  screen.o  gldisplay.o eval.o mlangg.o  file.o ginput.o help.o search.o  word.o window.o marks.o convert.o   gtkterm.o gplotc.o support.o geditdisplay.o gcanvasc.o highlight.o dir.o utils.o alist.o filebuf.o gtk_support.o plot_cairo.c  config_init.o utf8_support.o notes.o
	${CC} gmain.o system.o edit.o  screen.o  gldisplay.o eval.o mlangg.o  file.o ginput.o help.o search.o  word.o window.o marks.o convert.o  gtkterm.o gplotc.o support.o geditdisplay.o gcanvasc.o highlight.o dir.o utils.o alist.o filebuf.o gtk_support.o  config_init.o utf8_support.o notes.o -o ctg2  -lX11 $(GTK2_FLAGS)  $(GLIB_LIB) -lm

# This is with Xlib library, no plot !
ctxe : main.o system.o edit.o screen.o  xldisplay.o eval.o mlang.o  file.o  xinput.o help.o search.o  word.o window.o marks.o convert.o   xlib.o  highlight.o dir.o utils.o alist.o filebuf.o support.o config_init.o utf8_support.o notes.o
	${CC} main.o system.o edit.o screen.o   xldisplay.o eval.o mlang.o file.o  xinput.o help.o search.o  word.o window.o marks.o convert.o   xlib.o highlight.o dir.o utils.o alist.o  filebuf.o support.o config_init.o utf8_support.o notes.o -o ctxe -lm  $(GLIB_LIB) ${X11lib} 

#	This is for SCO and Xlib. -lsocket is needed at the end of every X application
#	cc -b elf main.o system.o edit.o  display.o eval.om lang.o file.o input.o help.o search.o  word.o window.o marks.o convert.o   xlib.o -o emacs  -lm -L/usr/X11R6/lib -lX11 -lsocket

# with curses, panel, no plot !
cte : main.o filebuf.o system.o edit.o screen.o  tldisplay.o eval.o mlang.o  file.o input.o help.o search.o  word.o window.o marks.o convert.o  panel_curses.o  highlight.o dir.o utils.o alist.o support.o config_init.o utf8_support.o notes.o mlangf.o xthemes.c
	${CC} main.o filebuf.o system.o edit.o screen.o  tldisplay.o eval.o mlang.o  file.o input.o help.o search.o  word.o window.o marks.o convert.o  panel_curses.o highlight.o dir.o utils.o alist.o support.o config_init.o utf8_support.o mlangf.o notes.o -o cte  $(GLIB_LIB) ${LPCURSES} -lm

ce : main.o filebuf.o system.o edit.o screen.o  tldisplay.o eval.o mlang.o  file.o input.o help.o search.o  word.o window.o marks.o convert.o  panel_curses.o  highlight.o dir.o utils.o alist.o support.o config_init.o utf8_support.o notes.o
	${CC} main.o filebuf.o system.o edit.o screen.o  tldisplay.o eval.o mlang.o  file.o input.o help.o search.o  word.o window.o marks.o convert.o  panel_curses.o highlight.o dir.o utils.o alist.o support.o config_init.o utf8_support.o notes.o -o ce  $(GLIB_LIB) ${LPCURSES} -lm

gplotc.o: gplotc.c plot_cairo.c plot_commonc.c gplot.h
	${CC}  -c ${FLAGS1}  ${GTKINCLUDE} -o $*.o  $*.c

#stroker: demos/stroker.c
#	gcc demos/stroker.c -o stroker `pkg-config gtk+-3.0 --cflags --libs`

#expose: demos/expose.c
#	gcc demos/expose.c -o expose `pkg-config gtk+-3.0 --cflags --libs`

#button_test : demos/button_test.c
#	gcc demos/button_test.c `pkg-config gtk+-3.0 --cflags --libs`  -o button_test

#listbox-dnd: demos/listbox-dnd.c
#	gcc demos/listbox-dnd.c -o listbox-dnd `pkg-config gtk+-3.0 --cflags --libs`

all : cleanall cte ctg3 ctg2 ctxe ce

# test make conditionals
test: 
	WSL1 := $(WsL)
	if [ $(WSL1) = '1' ]; then \
		echo "Microsoft Linux";\
	else \
		echo "Native LInux" ;\
	fi
	echo $(EXTFILE)
	echo $(WSL1)
	echo $(CC)

test1:
	ifeq ("$(WSL)" ,"1")
		EXTF="linux wsl"
	else
		EXTF="linux native"
	endif
	echl $(EXTF)

clean :
	echo clean all files!
	rm -f *.o
	rm -f *~
	rm -f tagx

cleanall :
	echo cleanall files
	rm -f *.o
	rm -f *~
	rm -f tagx.tags
	rm -f *.exe
	rm -f cte ctxe ctg2 ctg3 ce

install : cte ctg2 ctg3
	install .$(APP_NAME)_rc ~
	if [ "$(OSYSTEM)" = "DARWIN"]; then \
		install .$(APP_NAME)_ext_mac ~/.$(APP_NAME)/.$(APP_NAME)_ext;\
	else \
		if [ "$(WSL)" = "1" ]; then \
			install .$(APP_NAME)_ext_wsl ~/.$(APP_NAME)/.$(APP_NAME)_ext;\
		else \
			install .$(APP_NAME)_ext_linux ~/.$(APP_NAME)/.$(APP_NAME)_ext;\
		fi \
	fi
	install -d ~/.$(APP_NAME)
	install .colors16_ori ~/.$(APP_NAME)/.colors16
	install .$(APP_NAME)_mnu ~/.$(APP_NAME)/.$(APP_NAME)_mnu

	sudo install -d "$(SHAREDDIR)"/$(APP_NAME)
	sudo install .$(APP_NAME)_rc "$(SHAREDDIR)"/$(APP_NAME)
	if [ "$(WSL)" = "1" ]; then \
		sudo install .$(APP_NAME)_ext_wsl "$(SHAREDDIR)"/$(APP_NAME)/.$(APP_NAME)_ext;\
	else \
		sudo install .$(APP_NAME)_ext_linux "$(SHAREDDIR)"/$(APP_NAME)/.$(APP_NAME)_ext;\
	fi
	sudo install -d "$(SHAREDDIR)"/$(APP_NAME)/pixmaps
	sudo install -d "$(SHAREDDIR)"/$(APP_NAME)/help
	sudo install help/$(APP_NAME).md "$(SHAREDDIR)"/$(APP_NAME)/$(APP_NAME).md
	sudo install pixmap/*.xpm "$(SHAREDDIR)"/$(APP_NAME)/pixmaps
	sudo install ctg2.desktop "$(SHAREDDIR)"/applications
	sudo install ctg3.desktop /"$(SHAREDDIR)"/applications
	sudo install ctg2 /usr/local/bin
	sudo install ctg3 /usr/local/bin
	sudo install cte /usr/local/bin
#	sudo install ctxe /usr/local/bin

install_files:
	install .$(APP_NAME)_rc ~
	if [ "$(WSL)" = "1" ]; then \
		install .$(APP_NAME)_wsl ~/.$(APP_NAME)/.$(APP_NAME)_ext;\
	else \
		install .$(APP_NAME)_ext_linux ~/.$(APP_NAME)/.$(APP_NAME)_ext;\
	fi
	sudo install -d "$(SHAREDDIR)"/$(APP_NAME)
	sudo install .$(APP_NAME)_rc "$(SHAREDDIR)"/$(APP_NAME)
	if [ "$(WSL)" = "1" ]; then \
		sudo install .$(APP_NAME)_ext_wsl "$(SHAREDDIR)"/$(APP_NAME)/.$(APP_NAME)_ext;\
	else \
		sudo install .$(APP_NAME)_ext_linux "$(SHAREDDIR)"/.$(APP_NAME)_ext;\
	fi
	sudo install -d "$(SHAREDDIR)"/$(APP_NAME)/pixmaps
	sudo install -d "$(SHAREDDIR)"/$(APP_NAME)/help
	sudo install help/$(APP_NAME).md "$(SHAREDDIR)"/$(APP_NAME)/$(APP_NAME).md
	sudo install pixmap/*.xpm "$(SHAREDDIR)"/$(APP_NAME)/pixmaps
	sudo install ctg2.desktop "$(SHAREDDIR)"/applications
	sudo install ctg3.desktop "$(SHAREDDIR)"/applications

