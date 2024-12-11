
// set start time
void set_start_time();

int set_unique_tmp_file(char *str,char *name,int size);

// execute a system command, returns error status, shows messages, clean at the end
int sysexec(char *st);

/* update tag file */
int update_tags(num nused);


/*
 * Execute a shell command.
 * show stdout on [command] buffer, stderr on [error] buffer
 * emacs key ^X!, assigned to Alt-1
 */
int shell_cmd(num nused);

/* execute a background system command */
int bg_cmd(num n);

char *extension_string(FILEBUF *bp);


int grep_cmd(num  n);
int rgrep_cmd(num n);

/*
 * filter a buffer through an external program
 * emacs key ^X#, assigned also to alt-3
 */
int filter_buffer(num nuse);

#if	DARWIN
// check bpcopy/pbpaste on mac
int check_pbcopy();
#endif

int init_system_clipboard();

int ext_system_paste();
int ext_system_copy();

int mkdirRecursive(const char *path, mode_t mode);

