
// set start time
void set_start_time();

int set_unique_tmp_file(char *str,char *name,int size);

// execute a system command, returns error status, shows messages, clean at the end
int sysexec(char *st);

/* update tag file */
int update_tags(int nused);


/*
 * Execute a shell command.
 * show stdout on [command] buffer, stderr on [error] buffer
 * emacs key ^X!, assigned to Alt-1
 */
int shell_cmd(int  nused);

/* execute a background system command */
int bg_cmd(int n);

char *extension_string(FILEBUF *bp);


int grep_cmd(int  n);
int rgrep_cmd(int n);

/*
 * filter a buffer through an external program
 * emacs key ^X#, assigned also to alt-3
 */
int filter_buffer(int nuse);

#if	DARWIN
// check bpcopy/pbpaste on mac
int check_pbcopy();
#endif

int init_system_clipboard();

int ext_system_paste();
int ext_system_copy();

int mkdirRecursive(const char *path, mode_t mode);

