/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.
	
	Highlight definitions
*/

void update_highlight(WINDP *wp);
void update_highlight_line(WINDP *wp);
void update_highlight_none(WINDP *wp);
int c_incword(int c);
int c_injword(int c);
int c_incobol_word(int c);
int c_in_txt_word(int c);
int comment_c();
int comment_cc(int n);
int comment_perl(int n);
int comment_html(int n);
int comment_css(int n);
int comment_ecl(int n);
int comment_matlab(int n);
int comment_none(int n);
int comment_basic(int n);
int comment_sql(int n);


/* character states */
#if	1
#define HS_LINESTART	1
#define HS_PREVAST		2
#define HS_PREVSLASH	4
#define HS_PREVESC		8
#define HS_PSMALLER		16
#define HS_END1			32
#define HS_END2			64
#define	HS_SPEC			128	// special
#define HS_QMARK		256
#define HS_ES2			512
#define HS_LETTER		1024	// alpha
#define HS_PREVSPACE	2048
#define HS_TAG			4096
#else
#define HS_LINESTART	1
#define HS_PREVAST		2
#define HS_PREVSLASH	4
#define HS_PREVESC		5
#define HS_PSMALLER		6
#define HS_END1			7
#define HS_END2			8
#define	HS_SPEC			9	// special
#define HS_QMARK		10
#define HS_ES2			25
#define HS_LETTER		30	// alpha
#define HS_PREVSPACE	64
#define HS_TAG			128
#endif
	/* inserted script language  */

/* flags */
// Escape character
// ignore case

// operators
// line comment
// line comment2
// block comment, nested flag
// block comment2, nested flag
// preprocessor , continuation, only start line flag
// strings1, raw prefix
// strings2, raw prefix
// words1
// words2
// words3

int highlight_note(int c);
void highlight_c(int c);
void highlight_julia(int c);
void highlight_perl(int c);
void highlight_json(int c);
void highlight_terraform(int c);
void highlight_yaml(int c);
void highlight_html(int c);
void highlight_jscript(int c);
void highlight_pascal(int c);
void highlight_python(int c);
void highlight_other(int c);
void highlight_shell(int c);
void highlight_cobol(int c);
void highlight_text(int c);
void highlight_css(int c);
void highlight_ecl(int c);
void highlight_sql(int c);
void highlight_matlab(int c);
void highlight_cmd(int c);
void highlight_gtext(int c);
void highlight_rust(int c);
void highlight_tags(int c);
void highlight_sln(int c);
void highlight_md(int c);

char *c_w[] = { "if","else","do","while","switch","case","for","return","break","goto","continue","typedef",
	"namespace","this","throw","try","catch","@property","" };
char *c_w1[] = { "long", "int", "short","float", "double", "char", "union","struct","register", "unsigned","bool",
	"void","short","extern","static","const","enum","inline","class","public","private",
	"@interface","@end","@class","@public","@private","@implementation","self",
	"protected","template","" };

char *julia_w[] = { "if","elseif","else","function","for","in","while","begin","end",
	"return","do","try","catch","map","finally","break","continue","global","" };
char *julia_w1[] = { "print","println","abs","complex","const","typeof",
	"Int8","Int32","Int64","UInt8","UInt32","UInt64","Int128","UInt128",
	"Float32","Float64","Float128","BigInt","BigFloat","string","true","false",
	"NaN","Inf","Char","max","min","mean","exp",
	"typemax","typemin",
	"" };
// strings are in double quotes or triple double quotes



char *v_w[] = { "if","else","do","while","match","case","for","in","return","break","goto","continue","typedef","import",
	"module","this","throw","try","catch","fn","assert","lock","" };
char *v_w1[] = { "mut","long", "int","i32","i64", "short","u32","f32", "f64", "string", "voidptr", "struct", "unsigned","bool","sql","pub",
	"void","short","extern","static","const","enum","inline","class","public","private","false","true","println","eprintln","table",
	"self","result","or","" };

char *go_w[] = { "import","package","func","if","else","do","while","go", "switch","case","for","return","break","goto","continue","typedef",
	"select","this","try","default","defer","interface","map","range", "fallthrough", "" };
char *go_w1[] = { "var","type","long", "int", "short","float", "double", "char", "chan", "struct","register", "unsigned","bool","string","uint64",
	"void","short","extern","static","const","enum","inline","class","public","private","new",
	"self","true","false",
	"protected","template","" };

char *rust_w[] = { "fn","let","loop","if","else","do","while","in","match","for","return","break","goto","continue",
	"loop","where","FnOnce","use","match","typedef",
	"assert_eq!","Iterator","panic!","Ok","Err","parse::",
	"" };
char *rust_w1[] = { "i8","i32", "i64","i128","isize","u8","u32","u64","u128","usize", "str","f32","f64","bool","char","mut",
	"Vec::","type","vec!","&mut","ref","None",
	"extern","static","struct","enum","const","true","false","class","self","Some","format!","macrot_rules!",
	"pub","any","impl","crate::","super","mod","my_mod::","deeply::","nested::","iter::","result::","error::","fmt::","DoubleError::","mem::",
	"trait","String::","MathError::","Box::","HashMap::","collect::","thread::","Path::",
	"unsafe","try",
	""};

char *java_w[] = { "import","if","else","goto", "while","switch","case","for","do","return","break","continue",
		"default","try","catch","assert","finally","throws","throw","" };
char *java_w1[] = { "short","long", "int", "float", "double", "byte", "boolean", "static","strictfp","public", "String",
		"class","void","char","const","native","volatile","enum","extends","implements","instanceof","interface","transient",
		"abstract", "package", "private", "protected", "final", "new", "Integer", "synchronized","this", "true","false","null","" };

char *none_w[] = { "" };
char *text_w[] = { "" };

char *shell_w[] = { "if","in","then","fi","while","for","case","done","do", "elif","else","esac","until","break", "select","" };
char *shell_w1[] = { "function","export","set","shift","test","exit","echo","return","" };

char *sln_w[] = { "EndProjectSection","ProjectSection","EndProject","Project","EndGlobalSection","EndGlobal","GlobalSection","Global","" };
char *sln_w1[] = { "Debug","Presolution","" };

// from gnu make
char *make_w[] = { "ifeq","ifneq","ifdef","ifndef","else","endif","or","and","foreach","if","fi","" };
char *make_w1[] = { "subst","patsubst","strip","findstring","filter","filter-out","sort","call","words","firstword","lastword","notdir","suffix","basename",
	"addsuffix","addprefix","join","wildcard","realpath","abspath","dir","word","wordlist","eval","origin","flavor","error","warning","info","shell","guile","" };

char *html_w[] = { "<?php","php","if","elseif","do","else","return", "while","break","switch","case","default","foreach","continue","for","" };
char *html_w1[] = { "function","class","include","defined","define","echo","print", "title", "meta","var","array","this","object", "" };

char *xml_w[]= { "<?XML","HEAD", "BODY", "CARD", "BR","A","CARD","DO","" };
char *xml_w1[] = { "INPUT", "FONT", "OPTION","SMALL","" };

char *jscript_w[] = { "function","if","else","return", "try","catch","throws","throw","finally","final","goto","while","break","switch","implements","case","foreach","continue","for","debugger","do","with","yield","return","in","of","await","pipeline","stages","stage","steps", "agent","when","class","" };
char *jscript_w1[] = { "def","this","constructor","var","package","private","namespace","using","protected","public","import","export","extends","typeof","void","char","int","short","long","static","transient","volatile","super","native","boolean","float","enum","syncronized", "eval","echo","print","let","const", "abstract","arguments", "title", "meta", "true","false","null","new","null","instanceof","" };

char *terraform_w[] = { "variable","resource","type","proviser","module","output",""};
char *terraform_w1[] = { "string","number","list", "tags","tag","source","description","name","var","key","value",""};

char *tcl_w[] = { "IF", "ELSE", "PROC", "SET", "WHILE","PRINT","SWITCH","CASE","BREAK", "" };
char *tcl_w1[] = { "EVAL","GLOBAL","" };

char *struct_w[] = { "html","" };
char *struct_w1[] = { "if","else", "entity","code","spec","header","" };

char *fortran_w[] = { "DO", "FOR", "" };
char *fortran_w1[] = { "INT", "DOUBLE", "FLOAT" ,"" };
char *pascal_w[] = { "if","while","do","for","repeat","to","until","with","then","else","begin", "end","downto", "" };
char *pascal_w1[] = { "boolean","char","pointer","real","text","cardinal","long", "integer","byte","word","single","double","" };
char *cobol_w[] = { "PROGRAM-ID","PROCEDURE","IDENTIFICATION","DATA","SUBSCHEMA","ENVIRONMENT","DIVISION","CONFIGURATION","WORKING-STORAGE","COMMON-STORAGE","EXIT","PROC",
	"LINKAGE","SECTION","ACCEPT","ACCESS","ADVANCING","AFTER","CANCEL","CONTINUE","ELSE","END","END-CALL","CALL","END-COMPUTE","END-IF","REDEFINES","IF","GO TO","NEXT SENTENCE","SECTION","" };
char *cobol_w1[] = { "INVOKE","MOVE","COPY","ADD","SUBSTRACT","DIVIDE","MULTIPLY","PIC","AND","CLASS","CLOSE","OPEN",
	"FILLER","VALUE","COMPUTE","CONTAINS","WRITE","READ","BY","TO","OR","AND","NOT","PERFORM","THRU","DISPLAY","" };
char *perl_w[] = { "while","foreach","elsif","do","if","else","end","next","unless","return","goto","last",
	"sub","until","for","package","use","require","switch","die","" };
char *perl_w1[] = { "my","our","printf","print","open","close","mkdir","exit","sprintf","push","chomp","shift","split","local",
	"or","not","xor","le","lt","ne","eq","and","gt","ge","$self", "" };
char *ps1_w[] = { "WHILE","FOREACH","ELSIF","DO","IF","ELSE","END","NEXT","UNLESS","CONTINUE",
	"FUNCTION","UNTIL","FOR","SWITCH","DIE","RETURN","" };
char *ps1_w1[] = { "GLOBAL","WRITE-HOST","OPEN","CLOSE","MKDIR","RM","PARAM","STRING","INT",
	"OR","NOT","XOR","LE","LT","NE","EQ","AND","GT","GE","CONTAINS", "LIKE","NOTCONTAINS",
	"GET-CONTENT","GET-CHILDREN","GET-HOST","READ-HOST","" };
char *ssg_w[] = { "SSG","SGS","DEFINE","ENDDEFINE","SKEL","IF","ENDIF","ELSE","EOF","EXIT","RETURN","" };
char *ssg_w1[] = { "COPY","REMOVE","ADD","SET","DISPLAY","CSF","INCREMENT","LOOP","" };
char *ps_w[] = { "begin", "end", "" };
char *ps_w1[] = { "def", "" };
char *basic_w[] = { "IF","THEN","ELSE", "FOR", "" };
char *basic_w1[] = { "CHAR", "PRINT", "READ", "WRITE", "" };
char *wml_w[]= { "WML","HEAD", "BODYY", "CARD", "BR","A","CARD","DO","" };
char *wml_w1[] = { "INPUT", "FONT", "OPTION","SMALL","" };
char *cmd_w[] = { "if","while","fori","for","else","break","return","function","" };
char *cmd_w1[] = { "message","getchar", "input","print","initialize","chr$","mid$","strlen","dinput",""};
char *python_w[] = { "def","if","elif","else","while","try","except","or","with","for","in","and","not","assert","yield","continue","break","return","exit","" };
char *python_w1[] = { "print","from","import","as","exec","class","raise","None","self","property","tuple","isinstance","len","bool","set","int","iter","next","range","enumerate","sum","input","" };
char *mail_w[] = { "From:","Subject:","" };
char *mail_w1[] = { "To:", "Date:", "Subset:", "Status:","" };
char *ecl_w[] = { "BK1","BK2","RUN","START","FIN","XQT","ADD","BRKPT","SYM","ACOB","UCOB","MASM","UC","EOF","JUMP","LOG","PRT","SETC","START","TEST","CKPT","FILE","ENDF","LOG","MSG","QUAL","" };
char *ecl_w1[] = { "ASG","COPY" ,"CAT","USE","FREE","DELETE","MARK","MOVE","REWIND","CHG","CLOSE","COPIN","COPOUT","ENABLE","DISABLE","CYCLE","ERS","FIND","MARK","MODE","PACK","MASM","UCOB","LINK","PREP","MAP","" };
char *log_w[] = { "ERROR","fail",""};
char *log_w1[] = { "warning","warn",""};
char *sql_w[] = { "SQL","ABORT","ALTER","BEGIN","END","EXPLAIN","FETCH","LISTEN",
	"LOAD","CLOSE","COMMENT","SELECT","UPDATE","INSERT","DELETE","CREATE",
	"GRANT","DROP","VIEW","COMMIT","ROLLBACK","LOCK","MOVE","NOTIFY",
	"REINDEX","RESET","REVOKE","SHOW","TRUNCATEe","UNLISTEN","VALUES","INTO","" };
char *sql_w1[] = { "INT","CHAR","GROUP","TABLE","USER","FROM","INTO","TO","WHERE","NOT",
	"IN","CHECK","AS","AND","SET","TRANSACTION","DATABSE","INDEX","SEQUENCE",
	"TRIGGER","TYPE","ORDER","BY","DISPLAY","" };

char *ruby_w[] = {"if","for","elseif","else","unless","def","while","begin","end","class","module","do","case",""};
char *ruby_w1[] = {"method","singleton","require","when","configure","yield","return","exit",""};

char *matlab_w[] = {"if","for","elseif","else","while","begin","endif","end","class","module","do","case",""};
char *matlab_w1[] = {"endfunction","function","return","plot","hold","pause","clear","set",""};

char *objc_w[] = { "if","else","do","while","switch","case","for","return","break","goto","continue","typedef",
	"namespace","this","throw","try","catch","@property","" };
char *objc_w1[] = { "long", "int", "short","float", "double", "char", "struct","register", "unsigned","bool",
	"void","short","extern","static","const","enum","inline","class","public","private",
	"@interface","@end","@class","@public","@private","@implementation","self",
	"protected","template","" };

char *yaml_w[] = { "command:","shell:","azure_rm_deployment:","lineinfile:","yum:","template:","for","" };
char *yaml_w1[] = { "name:","with_items:","when:","block:","tasks:","vars:","set_fact:","register:","msg:","debug:","with_items:","include_tasks:",
					"delagate_to:","hosts:","gather_facts:","import_playbook:","include_role:","connection:","" };

char *dir_w[] = { "/bin","/etc","" };
char *dir_w1[] = { "/home","Downloads","Desktop","" };

char *tag_w[] = { NULL,NULL,NULL,NULL,NULL};
char *tag_w1[] = { NULL,NULL,NULL,NULL,NULL};

/* extensions defined  */
char *no_extensions[] = {"" };
char *bin_extensions[] = {"BIN","a","so",""};
char *c_extensions[] = { "C","c","h","cc","xpm","glade","m4","cpp","hpp","hh","hxx","ii","ino","i","" };
char *julia_extensions[] = { "JL","jl",""};
char *go_extensions[] = { "GO","go","" };
char *v_extensions[] = { "V","v","" };
char *rust_extensions[] = { "RS","rs","" };
char *txt_extensions[] = { "TEXT","txt","help","readme","" };
char *gtxt_extensions[] = { "GTEXT","tdc","cal","" };
char *perl_extensions[] = { "PL","pl","perl","cgi","pm","t","" };
char *ps1_extensions[] = { "PS1","ps1","" };
char *tcl_extensions[] = { "TCL","tcl","" };
char *html_extensions[] = { "HTML","htm","html","shtml","shtm","cfml","cfm","cfc","xht",
		"tpl","kit","jsp","aspx","ascx","asp","schtml","vbhtml","phtml","cfm","dtd",
		"erb",	// embedded ruby
		"ejs","dust",	//	embedded EJS
		"" };
char *md_extensions[] = {"md","markdown","mdown","mkdn",""};
char *wml_extensions[] = { "WML","wml",""};
char *info_extensions[] = { "INFO","inf",""};
char *xml_extensions[] = { "XML","xml","gla","wxs","wxl","wsdl","rss","atom","rdf","xslt","xsl","xul","xbl","xsd","mathml","xaml",""};
char *cobol_extensions[] = { "COBOL","cob","cbl","cpy",""};
char *ecl_extensions[] = { "ECL","ecl",""};
char *ssg_extensions[] = { "SSG","SSG","ssg","ssg-call",""};
char *fortran_extensions[] = { "FORTRAN","f",""};
char *shell_extensions[] = {"SHELL","sh","bash","command","profile","bashrc","bashlogout",""};
char *make_extensions[] = {"MAKE","makefile","Makefile",""};
char *cmd_extensions[] = {"CMD","cmd","xerc4",""};
char *man_extensions[] = {"MAN","man","1",""};
char *pascal_extensions[] = {"PASCAL","pas",""};
char *postscript_extensions[] = {"POSTSCRIPT","ps",""};
char *python_extensions[] = {"PYTHON","py","pyw","wsgi",""};
char *basic_extensions[] = {"BASIC","bas",""};
char *jscript_extensions[] = {"JAVASCRIPT","js","jenkinsfile","cs","jsm","_js",""};
char *asp_extensions[] = {"ASP","asp",""};
char *java_extensions[] = {"JAVA","java","jnlp",""};
char *php_extensions[] = {"PHP","php","php3","php4","phtm","ctp",""};
char *css_extensions[] = {"CSS","css","sccs","css.erb","sccs.erb",""};
char *objc_extensions[] = {"OBJC","m","mm","pch",""};
char *ruby_extensions[] = {"RUBY","rb","ru","gemspec","rake",""};
char *matlab_extensions[] = {"MATLAB","m",""};
char *m4_extensions[] = {"M4","m4",""};
char *dot_extensions[] = {"DOT",".",""};
char *mail_extensions[] = {"MAIL","mail",""};
char *sql_extensions[] = {"SQL","sql",""};
char *log_extensions[] = {"LOG","log","syslog",""};
char *compress_extensions[] = { "COMPRESS","gz","zip","bz2","tgz","bgz","bjz","Z",""};
char *tags_extensions[] = { "TAGS","tags","ctags",""};
char *json_extensions[] = { "JSON","json","gyp","gypi","tfstate",""};		// files .jshintrc, bowerrc
char *yaml_extensions[] = { "YAML","YML","yaml","yml",""};
char *dir_extensions[] = { "" };
char *tag_extensions[] = { "tag",""};
char *sln_extensions[] = { "SLN","sln",""};
char *terraform_extensions[] = {"tfvars","tf",""};

SHLIGHT hts[] = {
 { "NONE",0,0,none_w,none_w, highlight_text,update_highlight_none,c_in_txt_word,no_extensions,comment_perl },
 { "CMD",0,0,cmd_w,cmd_w1, highlight_cmd,update_highlight,c_incword,cmd_extensions,comment_perl },
 { "TEXT",1,0,text_w,none_w, highlight_text,update_highlight_line,c_in_txt_word,txt_extensions,comment_perl },
 { "C", 0,0,c_w,c_w1,highlight_c,update_highlight,c_incword,c_extensions,comment_cc },
 { "SHELL",0,0,shell_w,shell_w1, highlight_shell,update_highlight,c_incword,shell_extensions,comment_perl },
 { "HTML",0,0,html_w,html_w1,highlight_html,update_highlight,c_incword,html_extensions,comment_html },
 { "STRUCT",0,0,struct_w,struct_w1, highlight_other,update_highlight,c_incword, html_extensions,comment_cc },
 { "TCL",1,0,tcl_w,tcl_w1,highlight_c,update_highlight,c_incword,tcl_extensions,comment_perl },
 { "FORTRAN",1,0,fortran_w,fortran_w1, highlight_other,update_highlight_line,c_incword,fortran_extensions,comment_none },
 { "PASCAL",0,0,pascal_w,pascal_w1,highlight_pascal,update_highlight_line,c_incword,pascal_extensions,comment_none },
 { "COBOL",1,1,cobol_w,cobol_w1, highlight_cobol,update_highlight_line,c_incobol_word,cobol_extensions,comment_none },
 { "PERL",0,0,perl_w,perl_w1,highlight_perl,update_highlight,c_incword,perl_extensions,comment_perl },
 { "POSTSCRIPT",0,0,ps_w,ps_w1, highlight_other,update_highlight,c_incword,postscript_extensions,comment_none },
 { "BIN",0,0,none_w,none_w, highlight_other,update_highlight_none,c_incword,bin_extensions,comment_none },
 { "TAGS",0,0,none_w,none_w, highlight_other,update_highlight_none,c_incword,no_extensions,comment_none },
 { "XPROP",1,0,none_w,none_w, highlight_text,update_highlight,c_incword,no_extensions,comment_none },
 { "PROP",1,0,none_w,none_w, highlight_text,update_highlight,c_incword,no_extensions,comment_none },
 { "MAN",1,0,none_w,none_w, highlight_text,update_highlight,c_incword,man_extensions,comment_none },
 { "M4",0,0,none_w,none_w, highlight_text,update_highlight,c_incword,m4_extensions,comment_none },
 { "INFO",1,0,none_w,none_w, highlight_text,update_highlight,c_incword,info_extensions,comment_none },
 { "GTEXT",1,0,none_w,none_w, highlight_gtext,update_highlight_line,c_in_txt_word,gtxt_extensions,comment_perl },
 { "PYTHON",0,1,python_w,python_w1, highlight_python,update_highlight,c_incword,python_extensions,comment_perl },
 { "BASIC",1,0,basic_w,basic_w1, highlight_other,update_highlight,c_incword,basic_extensions,comment_basic },
 { "WML",1,0,wml_w,wml_w1, highlight_html,update_highlight,c_incword,wml_extensions,comment_html },
 { "DOT",1,0,none_w,none_w, highlight_other,update_highlight,c_incword,dot_extensions,comment_perl },
 { "XML",1,0,xml_w,xml_w1, highlight_html,update_highlight,c_incword,xml_extensions,comment_html },
 { "MAIL",0,0,mail_w,mail_w1, highlight_text,update_highlight,c_incword,no_extensions,comment_none },
 { "JAVA",0,0,java_w,java_w1,highlight_c,update_highlight,c_incword,java_extensions,comment_cc },
 { "SQL",1,0,sql_w,sql_w1, highlight_sql,update_highlight,c_incword,sql_extensions,comment_sql },
 { "PHP",0,0,html_w,html_w1,highlight_html,update_highlight,c_incword,php_extensions,comment_cc },
 { "LOG",1,0,log_w,log_w1, highlight_text,update_highlight_line,c_incword,log_extensions,comment_perl },
 { "JSCRIPT",0,0,jscript_w,jscript_w1,highlight_jscript,update_highlight,c_injword,jscript_extensions,comment_cc },
 { "CSS",0,0,html_w,html_w1,highlight_css,update_highlight,c_incword,css_extensions,comment_css },
 { "ECL",1,1,ecl_w,ecl_w1,highlight_ecl,update_highlight_line,c_incobol_word,ecl_extensions,comment_ecl },
 { "OBJC",0,0,objc_w,objc_w1,highlight_c,update_highlight,c_incword,objc_extensions,comment_cc },
 { "RUBY",0,0,ruby_w,ruby_w1,highlight_other,update_highlight,c_incword,ruby_extensions,comment_perl},
 { "MATLAB",0,0,matlab_w,matlab_w1,highlight_matlab,update_highlight,c_incword,matlab_extensions,comment_matlab},
 { "COMPRESS",0,0,none_w,none_w,highlight_text,update_highlight,c_incword,compress_extensions,comment_none},
 { "TAGS",0,0,none_w,none_w, highlight_text,update_highlight_none,c_incobol_word,tags_extensions,comment_none },
 { "PS1",1,0,ps1_w,ps1_w1, highlight_other,update_highlight,c_incword,ps1_extensions,comment_perl },
 { "SSG",1,1,ssg_w,ssg_w1, highlight_ecl,update_highlight_line,c_incword,ssg_extensions,comment_ecl },
 { "YAML",0,1,yaml_w,yaml_w1, highlight_yaml,update_highlight_line,c_incword,yaml_extensions,comment_perl },
 { "JSON",0,0,jscript_w,jscript_w1,highlight_json,update_highlight,c_incword,json_extensions,comment_cc },
 { "TERRAFORM",0,0,terraform_w,terraform_w1,highlight_terraform,update_highlight,c_incword,terraform_extensions,comment_cc },
 { "DIR",0,0,dir_w,dir_w1,highlight_text,update_highlight_none,c_incword,dir_extensions,comment_cc },
 { "GO", 0,0,go_w,go_w1,highlight_c,update_highlight,c_incword,go_extensions,comment_cc },
 { "RUST", 0,0,rust_w,rust_w1,highlight_rust,update_highlight,c_incword,rust_extensions,comment_cc },
 { "TAGVIEW",0,0,tag_w,tag_w1, highlight_tags,update_highlight,c_incword,tag_extensions,comment_none },
 { "MAKE",0,0,make_w,make_w1, highlight_shell,update_highlight,c_incword,make_extensions,comment_perl },
 { "SLN",0,0,sln_w,sln_w1, highlight_sln,update_highlight,c_incword,sln_extensions,comment_perl },
 { "V", 0,0,v_w,v_w1,highlight_c,update_highlight,c_incword,v_extensions,comment_cc },
 { "JULIA",0,0,julia_w,julia_w1,highlight_julia,update_highlight,c_incword,julia_extensions,comment_perl },
 { "CAL",1,0,none_w,none_w, highlight_cmd,update_highlight_line,c_in_txt_word,gtxt_extensions,comment_perl },
 { "MD",0,0,none_w,none_w, highlight_md, update_highlight, c_in_txt_word,md_extensions,comment_html },
 { NULL,0,0,NULL,NULL,NULL,NULL,NULL }
};

/* Character types
	- normal		0
	- control		1
	- numeric		2
	- symbols		3
	- escape		4
	- white space	5
	- quote			6
	
	- comment1		8		0x8
	- comment2		16		0x10
	- preprocess	32		0x20
	- single quoted	64		0x40
	- double quoted	128		0x80
	- special1		256		0x100
	- special2		512		0x200

	- normal
	- control escape
	- control
	- escape		
	- white space	
	- symbol
	- start - end of comment1
	- start - end of comment2
	- start - end of single quote
	- start - end of double quote
	- start - end subtype1
	- start - end subtype2

	Types of highlight
	- no highlight)
	- 1 character type (normal)
	- word match (special1,special2)
	- start of line with a char to end of line (c++ comment)
	- start, end with a character (quoting)
	- start, end with words
	- language defines
*/
