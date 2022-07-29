
# Colibri text editor compile from source requirements, notes

## To compile in Linux Ubuntu
  sudo apt-get update
  sudo apt-get install make
  sudo apt-get install gcc
  sudo apt-get install libglib2.0-dev
  sudo apt-get install libncurses-dev
  sudo apt-get install libsqlite3-dev
### For gtk3 driver
  sudo apt-get install libgtk-3-dev
### For gtk2 driver
  sudo apt-get install gtk2.0
 or
  sudo apt-get install libgtk2.0-dev
optionally
	sudo apt-get install libcanberra-gtk-module
### for clipboard support
	sudo apt install xclip
### for tag support
	sudo apt install exuberant-ctags

## To compile for RedHat/Centos Linux 
  yum install gcc
??  yum install glib*
 yum install glib2-devel
  yum install ncurses-devel
  yum install sqlite-devel
### for gtk2 driver
  yum install gtk2-devel
  yum install libcanberra-devel (optionally)
### for gtk3 driver
  yum install gtk3-devel
### for xclip 
   yum install libXmu-devel libX11-devel
   git clone https://github.com/astrand/xclip.git
	./bootstrap
	./configure
	make
	make install
### for tag support
	sudo yum install exuberant-ctags

### optional for centos minimal
  yum install git
  yum install bind-utils
  yum install bzip2
  yum install gtk2-devel
