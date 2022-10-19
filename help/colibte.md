# Help for colibte editor.

 Colibte is a lightweight, easy, fast, low memory consuption text editor for Linux.
 It also works in WSL, in powershell if compiled with cygnu and in MAC.
 It has a menu and support for multiple windows, both vertically or horizontally splited.
 It has color themes and highlight for various programming languages.
 With its DIR mode we can navigate easily on the file structure. 
 External programs can be defined to be used as viewer or editors for specific files.
 A hex mode with editing is also provided.
 editable user menu. (ALT-A)

## Basic characteristics
	- A console and gtk driver
	- A directory and file viewer of text docuemets
	- Highlight support for many programming languages
	- Multiple windows (vertical and horizontal splitting).
	- An easy menu system
	- His own and micro emacs keyboard assignment support.
	- A C-like macro language.
	- Notes taking handling with tags and todo,  calendar modes
	- Decent mouse support for console and Gtk drivers.
	- ctags support for code search.
	- Color scheme support.
		- 10 predefined color schemes
	- function keys assignements
	- Keyboard recording and replay.
	- Use of previous and next history keys (like in a browser)

## Driver and OS support

For the moment there are the following drivers.
  - For text teminal mode using ncurses/panel driver. Mouse support is also well functioning.
    Some modifications there may be needed for terminfo. 
    Terminfo samples are provided for better platform support.
    - Linux console, WSL console, putty, gnome-terminal, MAC iTerm,terminal,kitty. 
    	They all are using variations of 'xterm-256color' definition. 
  	Mouse support definition modification may be needed.
  - Gtk2 driver for X11. 
  - Gtk3 driver for X11.
  - X11 xlib driver (but without utf8 support).

 
  - This is a Linux editor written in C language with utf8 support. It has been compiled and tested on various Linux, Solaris, SCO unix, WSL and Cygwin, and Mac OSX.  
  - Console driver compiled under Cygwin can also been used under MS Windows powershell console.

## Clipboard support

  Clipoard support through 'xclip' in Linux and 'pbpaste/pbcopy' on Mac.  
  For WSL an X11 server must be installed and be active before starting the console version. 
  	Vcxsrv server is open source and free. 
  	Put the following line  
```
  	start "" "C:\Program Files\VcXsrv\vcxsrv.exe" -multiwindow
  	in the file
  		"C:\Users\<User Name>\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\vcxsrv.bat"
  	to start automatically
```

## Keyboard handling

### Basic keyboard keys :

	Use normal arrows to move arround.
	Left arrow is used to go to parent directory or out of a view file in DIR mode.
	HOME:	(also Control-A)
		moves to the start of the line, 
		pressed a second time position the cursor on the top left of the screen.
		pressed a third time position the cursor at the begining of the file
	END:	(alse Control-E)
		move to the end of the line.
		pressed a second time position the cursor at the bottom of the screen.
		pressed a third time position the cursor at the end of the file.
	PgUP:
		previous page. (In case we use 2 windows and sync mode (F11) both files go up)
	PgDN:
		next page. (In case we use 2 windows and sync mode (F11) both files go down)
	DEL:
		removes character at cursor position.
	INS:
		Changes from insert to overwrite and back. While in overwrite mode a letter 'O' is shown on the message line)

### Simple editing

	 DEL		: delete current letter
	 Back or ^H	: delete previous letter
	 ^D			: delete current line
	 ^K			: delete the rest of the line

### Moving arround

	 Control-P	go to the next window
	 Control-N	go to the next buffer/file
	 Control-b  opens a window with a list of buffers to select.
	 Control-]	go to matching character (parenthesis, curls, brackets).

### FUNCTION KEYS

 - F1	: Show file info.
 - F2	: Insert argument count 
 - F3	: Quick close, saves the file and close it.
 - F4	: Describe next key.
 - F5	: Find next.
 - F6	: Find previous.
 - F7	: Start, End keyboard macro.
 - F8	: Replay keyboard macro
 - F9	: Execute a shell command
 - F10	: Next file in the same window.
 - F11 : Toggle page movement (syncronizes page movement on multiple windows when using PGUP, PGDN).
 - F12 : Toggle movement lock


### Assign function to a key.

- ALT-K	With this key combination an arbritary function can be assigned to a key combination. 

### For Multiple Windows

We can work on multiple windows with the same or multiple files at the same time.
Window Menu is the main way to access this functionality.  
  
New files open in current window.  

 - ^P	            :   go to next window
 - ^N	            :   next file in current window.
 - ^X2 or ALT-W2		:	Split window horizontally
 - ^X3 or ALT-W3		:	Split window vertically
 - ^X1 or Alt-W1		:	Zoom current window.
 - ^X0 or ALT-W0		:	Close current window
 - ^B				:	Select current buffer from list

### Search - replace

 - ^S, F5 or ^F in memacs mode : Search forward.
 - ^R, F6		              : Search backward
 -  Alt-R 	Replace a pattern with a new one from current position to the end of the file.
 -  Alt-^R	Query replace with the following options.
    >  (Υ)	replace one.  
    >  (Ν)	Skip one.  
    >  (!)	replace all.  
    >  (U)	Correct last replace.  
    >  (.)	Abandon replacements.  
    >  (^G)	Abandon replacements.  
-   Search Menu starts other functions also.


### Editing keys.

- DEL remove current character.
- Backspace (or Control-H) removes previous letter.
- Control-D	deletes current line.
- Control-K	remove character till the end of the line.
- Control-Backspace removes previous word.

### Region operations/Clipboard.

- CTRL-space	starts/ends character selection mode.
- ALT-C			starts/ends column selection mode.
- ALT-L			starts/ends line selection mode.
- ^C			saves current selection to clipboard.
- ^X (^W in emacs mode)	cuts current selection.
- ^V 			Insert clipboard contents at current position (Paste).
- ALT-V			Insert system clipboard in current position 

During column selection characters are inserted or deleted on all lines at the same column.

### Undo - Redo

 ^U or ^Z		: undo
 ^Y				: redo

#### Special functions

 ALT-1	:	Execute a shell command, output appears to [command] buffer.  
 ALT-2	:	script exec. Execute the current buffer with the given command.  Output goes to [out] buffer, error goes to [error] buffer.  
 ALT-3	:	Filter buffer with external command. Output goes to [filter] buffer.  
 ALT-4	:	execute a macro line  
 ALT-5	search in files in current dir.   
>    : It greps the given pattern in curret dir files and produces an output buffer.  
>    : Using ALt-FG in a line the corresponding file is opened at the denoted line.  
 ALT-%  search recursively in current dir.   
> 	The same functionality as above can be used.  
 ALT-6	:	goto previous bookmark.  
 ALT-7	:	goto next bookmark.  
 ALT-8	:	set bookmark.  
 ALT-9	:	select bookmark from list  

## Directory and file viewer

- Basic directory viewer
- Direct viewing of text files (uses memory map for better memory handling)
- Can initiate program launching depending from file type

- ALT-WD open current dir in DIR mode
- ALT-WS opens secondary dir file.
 We can have both dirs at the same time and have file operations

### Keyboard handling in DIR mode
 - v,i or right arrow shows current file in current window. Also it can initiate view with an external program (like an viewer)
 - b of left arrow exits view file of goes to parent dir.
 - c	copy current file to secondary directory
 - d	create a new dir
 - ^D	delete current file (Be carefull !! still no verification is needed!!)
 - f	new file (touch)
 - TAB	go to other dir 
 - x (or ^M (enter)) exec current file
 - e	edit current file
 - m	move current file
 - n	rename current file
 - .	view next file (in view mode)
 - ,	view previous file (in view mode)

## Color themes.

There are 9 embeded color themes. 
 For the console driver they can be modified and new themes can be created
 by modify the file ~/.colibte/.colors16.
 For the GTK2,GTK3 drivers a scheme editor is provided for the modification (ALT-MODE - scheme eDit)

 Language highlight is supported for many programming languages. New ones can be easilly added 
 by modifiyng the editor source.

## Embeded macro.

A Macro script language is provided. Some basic scripts are provided and assigned to key combinations.  
Basic arithmetic and macro variables definitions can be done in text mode while editing.  
 The result is shown after the ':' at the end of the line.
 While editing a mcaro file:  
 Use ALT-= to evaluate the current line. Like the following:  
```
variable_a = 10+20 :  30.0
```
 Use ALT-+  to evaluate/execute current macro buffer. The output is printed on buffer '[out]'  
 Using the -x flag when started a macro file can be executed instead starting the editor.  

## C-mode utilities

 ALT-.	find tag (C mode) Find the function under the cursor in current dir.
 ALT-;	Add a C comment at the end of the line
 ALT-'	C++ comment/uncomment the current line
 ALT-@	view and select function in current file

## History marks. (just like in a browser)

A browser like history is maintainted.
 ALT-P (ALT-left arrow) 	: go to previous position in same file or other file
 ALT-N (ALT-right arrow) 	: go to next position in same file or other file


## Crypt file support.

 A very basic encryption is used. Do not really believe that your files are
 trully safe with this encryption. It is not tested and probably easily
 defeated with brute force methods!.

## User Preferences

 ALT-Edit - Preferences allows to edit user preferences.
 - Fill column (default is 72). To be used with ALT-Q to break a big line.
 - Left margin (default is 4).	Left margin used for converting big lines to small ones. (ALt-Q).
 - keydebug on/off
 - Tabsize (default is 4). Tabs are converted automatically to spaces for python and ansible files.
 - files to be greped for pattern search.
 - Print precision for the macro language.
 - Show HEX values for the macro language.
 - keyboard emulation (emacs like or native). (needs a restart to be activated)
 - make a backup while saving a file.
 - Embed icons for GTK drivers (Use its own icons).
 - Show position information on status line.
 - show vmlines (line numbers)
 - Show current offset on status line.
 - Show current character on status line.
 - Save file history (used to open recent files!).
 - Use classic status instead of color to show current window.

## Program command line names
| program name | description |
| ------------ | ----------- |
| cte	| console ncurses/panel driver with notes |
| ctg2  | gtk2 driver with notes |
| ctg3  | gtk3 driver with notes |
| cxe	| X11 driver with notes (no utf8 support) |
| ce    | console ncurses/panel driver witouth notes. No need for sqlite3 library |

## Command line flags



 -k	or  
 -K		edit with encryption (asks encryption code before saving the file).  
 -D		show debug messages on the stderr.  
 		redirection of stderr is suggested to a file like:  
		$ cte -D afile.txt 2> file.out  
 -x		execute a macro file  
 		$ cte -x a_macro_file  
 -C		set max colors to 256 for ncurses driver.  
 -8		set max colors to 8 for ncurses driver.  
 -b		open as binary file  
 -X		use a different startup file.  
 -n		Do not use mouse support in ncurses.  
 -N		Start with note tag window.  
 -m		use memory map for big read only files  
 -s		use a session file to repeat previous saved keyboard session.  

