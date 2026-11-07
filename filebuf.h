
int GetBlock(FILEBUF *fp,char *copy,offs from,offs size);

void  CheckPoint(FILEBUF *fp);
offs  ScanForCharForward(FILEBUF *fp,offs start,byte ch);
void  EmptyText(FILEBUF *);

offs  FUtfCharAt_nocheck(FILEBUF *bf, offs offset, utfchar *uc);
offs   get_text_offs(FILEBUF *,char *,offs,offs);

void   ToLineEnd();
offs   FNextLine(FILEBUF *fp,offs ptr);
int BolAt(offs o);
int FBolAt(FILEBUF *fp,offs o);
int EolAt(offs o);
int Eol();
byte FCharAt(FILEBUF *fp,offs offset);
byte FCharAt_NoCheck(FILEBUF *,offs offset);
offs FUtfCharAt(FILEBUF *,offs,utfchar *);
byte   Char();
void   MoveLineCol(num l,num c);
void   HardMove(num l,num c);
void   NewLine();
int  InsertChar(FILEBUF *fp,char ch);
void   ExpandTab();
void    DeleteChar();

void   PreModify(FILEBUF *);
void show_points(FILEBUF *bf,FILE *fp);
offs   LineBegin(offs ptr);
offs   FLineBegin(FILEBUF *fp,offs ptr);
offs   FPrevLine(FILEBUF *fp,offs ptr);
offs   LineEnd(offs ptr);
offs   FLineEnd(FILEBUF *fp,offs ptr);
num utf_FLineLen(FILEBUF *fp, offs ptr);

FILEBUF *current_file_buffer();
void set_current_file_buffer(FILEBUF *bp);

void MESG_time(const char *fmt, ...);
void MESG_time_start(const char *fmt, ...);
void *emalloc(long size,char *des);
void efree(void *p,char *des);
void  FError(const char *s);
bool test_b_flag(int flag);
void set_b_flag(int flag);
void set_modified(FILEBUF *bp);
void inodeinfo_set(FILEBUF *bf,struct stat *st);
void set_end_line(FILEBUF *fp,num line);
TextPoint *textpoint_init(int tp_type,int window_id,FILEBUF *fp);

int show_info(num n);

int is_utf_accent(FILEBUF *fp, offs o);
int utf8charlen_nocheck(int ch);
int FUtfCharLen(FILEBUF *fp,offs o);

offs check_next_char(FILEBUF *fp,offs o,int *col);
int DiffColumns(FILEBUF *fp, offs start_col,offs col_offs,char *from);
int DiffColumn(FILEBUF *fp, offs *dbo,offs col_offs,char *from);
void textpoint_move(TextPoint *tp,offs shift);
offs tp_offset(TextPoint *tp);
num tp_line(TextPoint *tp);
num	tp_col(TextPoint *tp);

void  ResetTextPoints(FILEBUF *bp,int);
void  FindOffset(TextPoint *tp);

void  textpoint_link(TextPoint *tp,FILEBUF *fp);
void  textpoint_unlink(TextPoint *tp);
void  textpoint_delete(TextPoint *tp);
void textpoint_free(TextPoint *tp);
TextPoint *textpoint_o(FILEBUF *bf,offs o,int tp_type,int window);
TextPoint *textpoint_new(FILEBUF *fp,int tp_type,int window);
TextPoint *new_textpoint_at(FILEBUF *bf,int tp_type,offs o);
void textpoint_set_lc(TextPoint *tp,num l, num c);
void tp_copy(TextPoint *tp1,TextPoint *tp2);
TextPoint *tp_dup(TextPoint *tp,int window_id);
void  FindOffset(TextPoint *tp);
void textpoint_set(TextPoint *tp,offs o);
void  ResetTextPoints(FILEBUF *bp,int flag);
void  textpoint_OrFlags(FILEBUF *fp,int mask);
offs  ScanForCharForward(FILEBUF *fp,offs start,byte ch);
int clipboard_copy(ClipBoard *clip);
int clipboard_paste(ClipBoard *clip);
void reset_region_textpoints();
void check_line_mode(FILEBUF *bf);
offs MoveToColumn(int go_col);
void   HardMove(num l,num c);
void   NewLine();
void   MoveLineCol(num line,num col);
void set_Char(offs offset,byte b);
byte  FCharAt(FILEBUF *bf, offs offset);
byte FChar(FILEBUF *fp);
byte  CharAt(offs offset);
offs  FUtfCharAt(FILEBUF *bf, offs offset, utfchar *uc);
offs  FUtfCharAt_nocheck(FILEBUF *bf, offs offset, utfchar *uc);
byte  FCharAt_NoCheck(FILEBUF *fp,offs offset);
byte tp_char(TextPoint *tp);
offs FSize(FILEBUF *fp);
offs CSize();
offs    Offset();
offs    FOffset(FILEBUF *fp);
void   ExpandTab();
int FEof(FILEBUF *fp);
int FEofAt(FILEBUF *fp,offs o);
int FBof(FILEBUF *fp);
byte    Char();
int EofAt(offs o);
long int utf_value();
long int unicode_point();
long int utf_value_len(int *len);
num physical_column(num vcol);
num WGetCol();
num GetCol();
num GetLine();
num LastLine();
num FColumn(FILEBUF *fp,offs o);
num Column(offs o);
num  FGetLine(FILEBUF *fp);
void refresh_textpoint(TextPoint *scan);
void refresh_textpoints(FILEBUF *fp);
void    DeleteChar();
int EolAt(offs o);
int FEolAt(FILEBUF *bf,offs o);
int BolAt(offs o);
int FBolAt(FILEBUF *bf,offs o);
int FEol(FILEBUF *fp);
int Eol();
int Bol();
int  InsertChar(FILEBUF *fp,char ch);

int    LockFile(int fd,bool drop);
off_t  GetDevSize(int fd);

void update_lines(FILEBUF *bp);
num get_lines(FILEBUF *bp);
num get_utf2to16_size(char *file_name);
int ifile(FILEBUF *bf,char *name,int ir_flag);
int CreateBak(FILEBUF *bp);
int CheckMode(mode_t mode);
int	writeout(char *name, FILEBUF *bf);


bool undo_change_join(undo_Change *c1, undo_Change *c);
size_t undo_GetSize(undo_Change *uc);
void undo_init(UNDOS *u);
UNDOS *undo_new();
bool undo_enabled(FILEBUF *fp);
bool undo_locked(FILEBUF *fp);
void undo_set_lock(FILEBUF *fp);
void undo_clear_lock(FILEBUF *fp);
void undo_clear(UNDOS *u);
void beginundogroup(UNDOS *u);
void undo_set_glue();
void undo_set_noglue();
void undo_AddChange(FILEBUF *fp,undo_Change *c);
void EndUndoGroup(UNDOS *u);
void undo_CheckSize(UNDOS *u);
void undo_set_Change(undo_Change *uc,type_t t, char *l,num ls, char *r,num rs);
undo_Change * undo_new_Change(type_t t, char *l,num ls, char *r,num rs);
void UndoGroup(UNDOS *u);
void RedoGroup(UNDOS *u);
void UndoOne(UNDOS *u);
void UndoOne(UNDOS *u);
void RedoOne(UNDOS *u);
void undo_change_undo(undo_Change *uc);
void undo_change_redo(undo_Change *uc);
int undo(num n);
int redo(num n);

int StartColumn(offs o);
void	ToStartColumn();
void	ToLineBegin();
void	ToLineEnd();

void   MoveLeftChar(FILEBUF *fp);
offs   FPrevUtfCharAt(FILEBUF *fp,offs o, utfchar *uc);
void set_Offset(offs offset);
void   MoveRightChar(FILEBUF *fp);
int next_utf8_error(num n);
int   InsertBlock(FILEBUF *fp, char *block_left,offs size_left,char *block_right,offs size_right);
int   ReadBlock(char *fname,int fd,offs size,offs *act_read);

int set_view_mode(num n);
int set_doc_lang(num n);
int set_default_local(num n);

int set_bom(num bom_type);
void show_textpoints(char *title,FILEBUF *fp);
int   DeleteBlock(FILEBUF *fp,offs left,offs right);

void show_points(FILEBUF *bf,FILE *fp);

FILEBUF *current_filebuf();
