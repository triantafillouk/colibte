# Colibri text editor
# Notes and special handling for various operating systems

## Linux
> Has been tested on Ubuntu, centos, fedori, redhat
 
## WSL (Windows Linux Subsystem)

### Terminal conflicting keyboard assignements
 To use ALT+LEFT and ALT+RIGHT keyboard assignements (previous and next history mark) 
 add these lines in Terminal's settings.json in the "actions" table.
  
    {
            "command": "unbound",
            "keys": "alt+right"
    },
    {
            "command": "unbound",
             "keys": "alt+left"
    },

 Also ^V sometimes is assigned as paste command from the terminal.
 Use shift+^V for the terminal
	{
		"command": "paste",
		"keys":	"ctrl+shift+v"
	}

 For the correct extended cursor support (after column 128) a modification is needed in the terminfo entry.
 A modified terminfo entry is included in directory "terminfo". 
 Use the following command
 $ sudo tic ubuntu_wsl_ext.tic

 Also other modified terminfo entries are included that correct some keyboard functions and color support as follows:
 - cygkt.tic
 - mac_terminal.tic
 - suse_wsl.tic

 Use 
```
tic terminfo_file_name.tic
```
 to update/modify the terminfo database
 
## CYGWIN
 A compiled version in cygwin with gcc producces an executable that can be used also in powershell.
  
## Solaris
 Older console version have been tested on Solaris 10
 Compile 'em' to use without sqlite3 database support

## MacOS


### for Kitty terminal add the following in the kitty.conf configuration file
```
# to recognise ALT keys
macos_option_as_alt yes

# previous, next word 
map ctrl+left  send_text all \x1B[P
map ctrl+right  send_text all \x1B[N
# find previous, next
map ctrl+up  send_text all \x1B[17~
map ctrl+down  send_text all \x1B[15~
# go to history previous, next position
map alt+left   send_text all \x1BP
map alt+right  send_text all \x1BN
# previous, next page
map shift+UP send_text all \x1B[5~
map shift+DOWN send_text all \x1b[6~
```

### For mac terminal program, add the following keybindings
```
# alt - combinations
left	\033P
up		\033[w
right	\033N
down	\033[z

# control - combinations
left	\033[P
up		\033OM
right	\033[N
down	\033[15~

# shift - combinations
left	\033[d
up		\033[5~
right	\033[c
down	\033[6~

delete	\033[3~
control delete \033010
```