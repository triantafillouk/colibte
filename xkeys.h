/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* xkeys.c */
/* The first character is the length of the string */
#define MAX_KEYSYM_STRING_LEN 8

typedef char KeySym_Mapping_Type[MAX_KEYSYM_STRING_LEN + 1];

	/* normal  */
static KeySym_Mapping_Type KeySym_Mapping[256] =
{
   "\004\033[3~",	/* 0xFF00                        Remove */
   "",			/* 0xFF01 */
   "",			/* 0xFF02 */
   "",			/* 0xFF03 */
   "",			/* 0xFF04 */
   "",			/* 0xFF05 */
   "",			/* 0xFF06 */
   "\001w",			/* 0xFF07 */
   "\001\010",		/* 0xFF08 	 XK_BackSpace */
   "\001\t",		/* 0xFF09 	 XK_Tab */
   "\001\n",		/* 0xFF0A 	 XK_Linefeed */
   "",			/* 0xFF0B 	 XK_Clear */
   "",			/* 0xFF0C */
   "\001\r",		/* 0xFF0D 	 XK_Return */
   "",			/* 0xFF0E */
   "",			/* 0xFF0F */
   "",			/* 0xFF10 */
   "",			/* 0xFF11 */
   "",			/* 0xFF12 */
   "",			/* 0xFF13 	 XK_Pause */
   "",			/* 0xFF14 	 XK_Scroll_Lock */
   "",			/* 0xFF15 */
   "",			/* 0xFF16 */
   "",			/* 0xFF17 */
   "",			/* 0xFF18 */
   "",			/* 0xFF19 */
   "",			/* 0xFF1A */
   "\001\033",		/* 0xFF1B 	 XK_Escape */
   "",			/* 0xFF1C */
   "",			/* 0xFF1D */
   "",			/* 0xFF1E */
   "",			/* 0xFF1F */
   "",			/* 0xFF20 	 XK_Multi_key */
   "",			/* 0xFF21 	 XK_Kanji */
   "",			/* 0xFF22 	 XK_Muhenkan */
   "",			/* 0xFF23 	 XK_Henkan_Mode 	 XK_Henkan */
   "",			/* 0xFF24 	 XK_Romaji */
   "",			/* 0xFF25 	 XK_Hiragana */
   "\004\033[5~",			/* 0xFF26 	 XK_Katakana */
   "\004\033[6~",			/* 0xFF27 	 XK_Hiragana_Katakana */
   "",			/* 0xFF28 	 XK_Zenkaku */
   "",			/* 0xFF29 	 XK_Hankaku */
   "",			/* 0xFF2A 	 XK_Zenkaku_Hankaku */
   "",			/* 0xFF2B 	 XK_Touroku */
   "",			/* 0xFF2C 	 XK_Massyo */
   "",			/* 0xFF2D 	 XK_Kana_Lock */
   "",			/* 0xFF2E 	 XK_Kana_Shift */
   "",			/* 0xFF2F 	 XK_Eisu_Shift */
   "",			/* 0xFF30 	 XK_Eisu_toggle */
   "",			/* 0xFF31 */
   "",			/* 0xFF32 */
   "",			/* 0xFF33 */
   "",			/* 0xFF34 */
   "",			/* 0xFF35 */
   "",			/* 0xFF36 */
   "",			/* 0xFF37 */
   "",			/* 0xFF38 */
   "",			/* 0xFF39 */
   "",			/* 0xFF3A */
   "",			/* 0xFF3B */
   "",			/* 0xFF3C */
   "\004ABCD",			/* 0xFF3D */
   "",			/* 0xFF3E */
   "",			/* 0xFF3F */
   "",			/* 0xFF40 */
   "",			/* 0xFF41 */
   "",			/* 0xFF42 */
   "",			/* 0xFF43 */
   "",			/* 0xFF44 */
   "",			/* 0xFF45 */
   "",			/* 0xFF46 */
   "",			/* 0xFF47 */
   "",			/* 0xFF48 */
   "",			/* 0xFF49 */
   "",			/* 0xFF4A */
   "",			/* 0xFF4B */
   "",			/* 0xFF4C */
   "",			/* 0xFF4D */
   "",			/* 0xFF4E */
   "",			/* 0xFF4F */
   "\001\001",		/* 0xFF50 	 XK_Home */
   "\003\033[D",	/* 0xFF51 	 XK_Left */
   "\003\033[A",	/* 0xFF52 	 XK_Up */
   "\003\033[C",	/* 0xFF53 	 XK_Right */
   "\003\033[B",	/* 0xFF54 	 XK_Down */
   "\004\033[5~",	/* 0xFF55 	 XK_Prior */
   "\004\033[6~",	/* 0xFF56 	 XK_Next */
   "\001\005",		/* 0xFF57 	 XK_End */
   "\001\001",		/* 0xFF58 	 XK_Begin */
   "\001\002",			/* 0xFF59 */
   "\001\003",			/* 0xFF5A */
   "\001\004",			/* 0xFF5B */
   "\001\005",			/* 0xFF5C */
   "\001\006",			/* 0xFF5D */
   "\001\007",			/* 0xFF5E */
   "\001\008",			/* 0xFF5F */
   "\004\033[4~",	/* 0xFF60 	 XK_Select */
   "",			/* 0xFF61 	 XK_Print */
   "",			/* 0xFF62 	 XK_Execute */
   "\004\033[2~",	/* 0xFF63 	 XK_Insert */
   "\001\009",			/* 0xFF64 */
   "\001\037",		/* 0xFF65 	 XK_Undo 	 ^_ */
   "\004redo",			/* 0xFF66 	 XK_Redo */
   "\005\033[29~",	/* 0xFF67 	 XK_Menu        Do */
   "\004\033[1~",	/* 0xFF68 	 XK_Find */
   "\001\007",		/* 0xFF69 	 XK_Cancel */
   "\005\033OP",	/* 0xFF6A 	 XK_Help */
   "",			/* 0xFF6B 	 XK_Break */
   "",			/* 0xFF6C */
   "",			/* 0xFF6D */
   "",			/* 0xFF6E */
   "",			/* 0xFF6F */
   "",			/* 0xFF70 */
   "",			/* 0xFF71 */
   "",			/* 0xFF72 */
   "",			/* 0xFF73 */
   "",			/* 0xFF74 */
   "",			/* 0xFF75 */
   "",			/* 0xFF76 */
   "",			/* 0xFF77 */
   "",			/* 0xFF78 */
   "",			/* 0xFF79 */
   "",			/* 0xFF7A */
   "",			/* 0xFF7B */
   "",			/* 0xFF7C */
   "",			/* 0xFF7D */
//   "\003\033[s",	/* 0xFF7E 	right alt */ 
   "\001\033",	/* 0xFF7E 	no key for mac */ 
   "",			/* 0xFF7F 	 XK_Num_Lock */
   "",			/* 0xFF80 	 XK_KP_Space */
   "",			/* 0xFF81 */
   "",			/* 0xFF82 */
   "",			/* 0xFF83 */
   "",			/* 0xFF84 */
   "",			/* 0xFF85 */
   "",			/* 0xFF86 */
   "",			/* 0xFF87 */
   "",			/* 0xFF88 */
   "",			/* 0xFF89 	 XK_KP_Tab */
   "",			/* 0xFF8A */
   "",			/* 0xFF8B */
   "",			/* 0xFF8C */
   "\001\015",		/* 0xFF8D 	 XK_KP_Enter */
   "",			/* 0xFF8E */
   "",			/* 0xFF8F */
   "",			/* 0xFF90 */
   "\003\033OP",	/* 0xFF91 	 XK_KP_F1        PF1 */
   "\003\033OQ",	/* 0xFF92 	 XK_KP_F2        PF2 */
   "\003\033OR",	/* 0xFF93 	 XK_KP_F3        PF3 */
   "\003\033OS",	/* 0xFF94 	 XK_KP_F4        PF4 */
   "\001\001",		/* 0xFF95 	HOME */
   "\003\033[D",			/* 0xFF96 */		/* 150 */
   "\003\033[A",			/* 0xFF97 */
   "\003\033[C",			/* 0xFF98 */
   "\003\033[B",			/* 0xFF99 */
   "\004\033[5~",	/* 0xFF9A */
   "\004\033[6~",	/* 0xFF9B */
   "\001\005",	/* 0xFF9C */
   "\005\033[15~",	/* 0xFF9D central key */
//   "\001\031",		/* 0xFF9E INS*/
   "\004\033[0~",	/* INSERT numeric (0)  */
   "\001\177",		/* 0xFF9F Numeric . */	
   "",			/* 0xFFA0 */				/* 160 */
   "",			/* 0xFFA1 */
   "",			/* 0xFFA2 */
   "",			/* 0xFFA3 */
   "",			/* 0xFFA4 */
   "",			/* 0xFFA5 */
   "",			/* 0xFFA6 */
   "",			/* 0xFFA7 */
   "",			/* 0xFFA8 */
   "",			/* 0xFFA9 */
   "\001*",		/* 0xFFAA 	 XK_KP_Multiply */
   "\001+",		/* 0xFFAB 	 XK_KP_Add */
   "\003\033Ol",	/* 0xFFAC 	 XK_KP_Separator KP , */
   "\001-",		/* 0xFFAD 	 XK_KP_Subtract  KP - */
   "\001.",	/* 0xFFAE 	 XK_KP_Decimal   KP . */
   "\001/",		/* 0xFFAF 	 XK_KP_Divide */
   "\0010",	/* 0xFFB0 	 XK_KP_0 */
   "\0011",	/* 0xFFB1 	 XK_KP_1 */
   "\0012",	/* 0xFFB2 	 XK_KP_2 */
   "\0013",	/* 0xFFB3 	 XK_KP_3 */
   "\0014",	/* 0xFFB4 	 XK_KP_4 */
   "\005\033[15~",	/* 0xFFB5	 XK_KP_5 */
   "\0016",	/* 0xFFB6 	 XK_KP_6 */
   "\0017",	/* 0xFFB7 	 XK_KP_7 */
   "\0018",	/* 0xFFB8 	 XK_KP_8 */
   "\0019",	/* 0xFFB9 	 XK_KP_9 */
   "\001a",		/* 0xFFBA */
   "\001b",		/* 0xFFBB */
   "\001c",		/* 0xFFBC */
   "\001=",		/* 0xFFBD 	 XK_KP_Equal */
   "\003\033OP",	/* 0xFFBE 	 XK_F1 */  /* xterm convention */
   "\005\033[12~",	/* 0xFFBF 	 XK_F2 */  /* xterm convention */ /* 190 */
   "\005\033[13~",	/* 0xFFC0 	 XK_F3 */  /* xterm convention */
   "\005\033[14~",	/* 0xFFC1 	 XK_F4 */  /* xterm convention */
   "\005\033[15~",	/* 0xFFC2 	 XK_F5 */  /* xterm convention */
   "\005\033[17~",	/* 0xFFC3 	 XK_F6 */
   "\005\033[18~",	/* 0xFFC4 	 XK_F7 */
   "\005\033[19~",	/* 0xFFC5 	 XK_F8 */
   "\005\033[20~",	/* 0xFFC6 	 XK_F9 */
   "\005\033[21~",	/* 0xFFC7 	 XK_F10 */
   "\005\033[23~",	/* 0xFFC8 	 XK_F11 	 XK_L1 */
   "\003\033Oa",	/* 0xFFC9 	 XK_F12 	 XK_L2 */	/* 200 */
   "\005\033[25~",	/* 0xFFCA 	 XK_F13 	 XK_L3 */
   "\005\033[26~",	/* 0xFFCB 	 XK_F14 	 XK_L4 */
   "\003\033OP",	/* 0xFFCC 	 XK_F15 	 XK_L5 */
   "\005\033[29~",	/* 0xFFCD 	 XK_F16 	 XK_L6 */
   "\005\033[31~",	/* 0xFFCE 	 XK_F17 	 XK_L7 */
   "\005\033[32~",	/* 0xFFCF 	 XK_F18 	 XK_L8 */
   "\005\033[33~",	/* 0xFFD0 	 XK_F19 	 XK_L9 */
   "\005\033[34~",	/* 0xFFD1 	 XK_F20 	 XK_L10 */
   "\005\033[35~",	/* 0xFFD2 	 XK_F21 	 XK_R1 */
   "\005\033[36~",	/* 0xFFD3 	 XK_F22 	 XK_R2 */	/* 210 */
   "\005\033[37~",	/* 0xFFD4 	 XK_F23 	 XK_R3 */
   "\005\033[38~",	/* 0xFFD5 	 XK_F24 	 XK_R4 */
   "\005\033[39~",	/* 0xFFD6 	 XK_F25 	 XK_R5 */
   "\005\033[40~",	/* 0xFFD7 	 XK_F26 	 XK_R6 */
   "\005\033[41~",	/* 0xFFD8 	 XK_F27 	 XK_R7 */
   "\005\033[42~",	/* 0xFFD9 	 XK_F28 	 XK_R8 */
   "\005\033[43~",	/* 0xFFDA 	 XK_F29 	 XK_R9 */
   "\005\033[44~",	/* 0xFFDB 	 XK_F30 	 XK_R10 */
   "\005\033[45~",	/* 0xFFDC 	 XK_F31 	 XK_R11 */
   "\005\033[46~",	/* 0xFFDD 	 XK_F32 	 XK_R12 */	/* 220 */
   "\005\033[47~",	/* 0xFFDE 	 XK_R13 	 XK_F33 */
   "\005\033[48~",	/* 0xFFDF 	 XK_F34 	 XK_R14 */
   "\005\033[49~",	/* 0xFFE0 	 XK_F35 	 XK_R15 */
   "",			/* 0xFFE1 	 XK_Shift_L 	 XK_Shift_L */
   "",			/* 0xFFE2 	 XK_Shift_R */
   "",			/* 0xFFE3 	 XK_Control_L */
   "",			/* 0xFFE4 	 XK_Control_R */
   "",			/* 0xFFE5 	 XK_Caps_Lock */
   "",			/* 0xFFE6 	 XK_Shift_Lock */
   "",			/* 0xFFE7 	 XK_Meta_L */
   "",			/* 0xFFE8 	 XK_Meta_R */
   "",			/* 0xFFE9 	 XK_Alt_L */
   "",			/* 0xFFEA 	 XK_Alt_R */
   "",			/* 0xFFEB 	 XK_Super_L */
   "",			/* 0xFFEC 	 XK_Super_R */
   "",			/* 0xFFED 	 XK_Hyper_L */
   "",			/* 0xFFEE 	 XK_Hyper_R */
   "",			/* 0xFFEF */
   "",			/* 0xFFF0 */
   "",			/* 0xFFF1 */
   "",			/* 0xFFF2 */
   "",			/* 0xFFF3 */
   "",			/* 0xFFF4 */
   "",			/* 0xFFF5 */
   "",			/* 0xFFF6 */
   "",			/* 0xFFF7 */
   "",			/* 0xFFF8 */
   "",			/* 0xFFF9 */
   "",			/* 0xFFFA */
   "",			/* 0xFFFB */
   "",			/* 0xFFFC */
   "",			/* 0xFFFD */
   "",			/* 0xFFFE */
   "\001\177"		/* 0xFFFF 	 XK_Delete */
};

 /* shift */
static KeySym_Mapping_Type Shift_KeySym_Mapping[256] =
{
   "\004\033[3$",	/* 0xFF00                        Remove */
   "",			/* 0xFF01 */
   "",			/* 0xFF02 */
   "",			/* 0xFF03 */
   "",			/* 0xFF04 */
   "",			/* 0xFF05 */
   "",			/* 0xFF06 */
   "",			/* 0xFF07 */
   "\001\010",		/* 0xFF08 	 XK_BackSpace */
   "\003\033[Z",	/* 0xFF09 	 XK_Tab */
   "\001\n",		/* 0xFF0A 	 XK_Linefeed */
   "",			/* 0xFF0B 	 XK_Clear */
   "",			/* 0xFF0C */
   "\001\r",		/* 0xFF0D 	 XK_Return */
   "",			/* 0xFF0E */
   "",			/* 0xFF0F */
   "",			/* 0xFF10 */
   "",			/* 0xFF11 */
   "",			/* 0xFF12 */
   "",			/* 0xFF13 	 XK_Pause */
   "",			/* 0xFF14 	 XK_Scroll_Lock */
   "",			/* 0xFF15 */
   "",			/* 0xFF16 */
   "",			/* 0xFF17 */
   "",			/* 0xFF18 */
   "",			/* 0xFF19 */
   "",			/* 0xFF1A */
   "\001\033",		/* 0xFF1B 	 XK_Escape */
   "",			/* 0xFF1C */
   "",			/* 0xFF1D */
   "",			/* 0xFF1E */
   "",			/* 0xFF1F */
   "",			/* 0xFF20 	 XK_Multi_key */
   "",			/* 0xFF21 	 XK_Kanji */
   "",			/* 0xFF22 	 XK_Muhenkan */
   "",			/* 0xFF23 	 XK_Henkan_Mode 	 XK_Henkan */
   "",			/* 0xFF24 	 XK_Romaji */
   "",			/* 0xFF25 	 XK_Hiragana */
   "\002\033<",			/* 0xFF26 	 XK_Katakana */
   "\002\033>",			/* 0xFF27 	 XK_Hiragana_Katakana */
   "",			/* 0xFF28 	 XK_Zenkaku */
   "",			/* 0xFF29 	 XK_Hankaku */
   "",			/* 0xFF2A 	 XK_Zenkaku_Hankaku */
   "",			/* 0xFF2B 	 XK_Touroku */
   "",			/* 0xFF2C 	 XK_Massyo */
   "",			/* 0xFF2D 	 XK_Kana_Lock */
   "",			/* 0xFF2E 	 XK_Kana_Shift */
   "",			/* 0xFF2F 	 XK_Eisu_Shift */
   "",			/* 0xFF30 	 XK_Eisu_toggle */
   "",			/* 0xFF31 */
   "",			/* 0xFF32 */
   "",			/* 0xFF33 */
   "",			/* 0xFF34 */
   "",			/* 0xFF35 */
   "",			/* 0xFF36 */
   "",			/* 0xFF37 */
   "",			/* 0xFF38 */
   "",			/* 0xFF39 */
   "",			/* 0xFF3A */
   "",			/* 0xFF3B */
   "",			/* 0xFF3C */
   "",			/* 0xFF3D */
   "",			/* 0xFF3E */
   "",			/* 0xFF3F */
   "",			/* 0xFF40 */
   "",			/* 0xFF41 */
   "",			/* 0xFF42 */
   "",			/* 0xFF43 */
   "",			/* 0xFF44 */
   "",			/* 0xFF45 */
   "",			/* 0xFF46 */
   "",			/* 0xFF47 */
   "",			/* 0xFF48 */
   "",			/* 0xFF49 */
   "",			/* 0xFF4A */
   "",			/* 0xFF4B */
   "",			/* 0xFF4C */
   "",			/* 0xFF4D */
   "",			/* 0xFF4E */
   "",			/* 0xFF4F */
   "\003\033[H",	/* 0xFF50 	 XK_Home */
#if	DARWIN
//   "\002\033P",	/* 0xFF55 	 XK_Prior */
   "\001\001",		/* 0xFF50 	 XK_Home */
   "\004\033[5~",	/* page up  */
   "\001\005",		/* 0xFF57 	 XK_End */
//   "\002\033N",	/* 0xFF53 	 XK_Right */
   "\004\033[6~",	/* page down  */
#else
   "\003\033[d",	/* 0xFF51 	 XK_Left */
   "\003\033[a",	/* 0xFF52 	 XK_Up */
   "\003\033[c",	/* 0xFF53 	 XK_Right */
   "\003\033[b",	/* 0xFF54 	 XK_Down */
#endif
   "\004\033[5$",	/* 0xFF55 	 XK_Prior */
   "\004\033[6$",	/* 0xFF56 	 XK_Next */
   "\003\033[[",	/* 0xFF57 	 XK_End */
   "\001\001",		/* 0xFF58 	 XK_Begin */
   "",			/* 0xFF59 */
   "",			/* 0xFF5A */
   "",			/* 0xFF5B */
   "",			/* 0xFF5C */
   "",			/* 0xFF5D */
   "",			/* 0xFF5E */
   "",			/* 0xFF5F */
   "\004\033[4$",	/* 0xFF60 	 XK_Select */
   "",			/* 0xFF61 	 XK_Print */
   "",			/* 0xFF62 	 XK_Execute */
//   "\004\033[2$",	/* 0xFF63 	 XK_Insert */
   "\002\033\031",	/* 0xFF63 	 XK_Insert */
   "\001T",			/* 0xFF64 */
   "\001\037",		/* 0xFF65 	 XK_Undo 	 ^_ */
   "",			/* 0xFF66 	 XK_Redo */
   "\005\033[29$",	/* 0xFF67 	 XK_Menu         Do */
   "\004\033[1$",	/* 0xFF68 	 XK_Find */
   "\001\007",		/* 0xFF69 	 XK_Cancel */
   "\005\033OP",	/* 0xFF6A 	 XK_Help */
   "",			/* 0xFF6B 	 XK_Break */
   "",			/* 0xFF6C */
   "",			/* 0xFF6D */
   "",			/* 0xFF6E */
   "",			/* 0xFF6F */
   "",			/* 0xFF70 */
   "",			/* 0xFF71 */
   "",			/* 0xFF72 */
   "",			/* 0xFF73 */
   "",			/* 0xFF74 */
   "",			/* 0xFF75 */
   "",			/* 0xFF76 */
   "",			/* 0xFF77 */
   "",			/* 0xFF78 */
   "",			/* 0xFF79 */
   "",			/* 0xFF7A */
   "",			/* 0xFF7B */
   "",			/* 0xFF7C */
   "",			/* 0xFF7D */
   "\003\033[l",			/* 0xFF7E */
   "",			/* 0xFF7F 	 XK_Num_Lock */
   "",			/* 0xFF80 	 XK_KP_Space */
   "",			/* 0xFF81 */
   "",			/* 0xFF82 */
   "",			/* 0xFF83 */
   "",			/* 0xFF84 */
   "",			/* 0xFF85 */
   "",			/* 0xFF86 */
   "",			/* 0xFF87 */
   "",			/* 0xFF88 */
   "",			/* 0xFF89 	 XK_KP_Tab */
   "",			/* 0xFF8A */
   "",			/* 0xFF8B */
   "",			/* 0xFF8C */
   "\001\015",		/* 0xFF8D 	 XK_KP_Enter */
   "",			/* 0xFF8E */
   "",			/* 0xFF8F */
   "",			/* 0xFF90 */
   "\003\033OP",	/* 0xFF91 	 XK_KP_F1        PF1 */
   "\003\033OQ",	/* 0xFF92 	 XK_KP_F2        PF2 */
   "\003\033OR",	/* 0xFF93 	 XK_KP_F3        PF3 */
   "\003\033OS",	/* 0xFF94 	 XK_KP_F4        PF4 */
   "\0017",			/* 0xFF95 */
   "\0014",			/* 0xFF96 */			/* Shift 150 */
   "\0018",			/* 0xFF97 */
   "\0016",			/* 0xFF98 */
   "\0012",			/* 0xFF99 */
   "\0019",			/* 0xFF9A */
   "\0013",			/* 0xFF9B */
   "\0011",			/* 0xFF9C */
   "\0015",			/* 0xFF9D */
   "\0010",			/* 0xFF9E */
   "\001.",			/* 0xFF9F */
   "",			/* 0xFFA0 */				/* shift 160 */
   "",			/* 0xFFA1 */
   "",			/* 0xFFA2 */
   "",			/* 0xFFA3 */
   "",			/* 0xFFA4 */
   "",			/* 0xFFA5 */
   "",			/* 0xFFA6 */
   "",			/* 0xFFA7 */
   "",			/* 0xFFA8 */
   "",			/* 0xFFA9 */				
   "",			/* 0xFFAA 	 XK_KP_Multiply */	/* shift 170 */
   "",			/* 0xFFAB 	 XK_KP_Add */
   "\001,",	/* 0xFFAC 	 XK_KP_Separator KP , */
   "\001-",	/* 0xFFAD 	 XK_KP_Subtract  KP - */
   "\001.",	/* 0xFFAE 	 XK_KP_Decimal   KP . */
   "\001/",			/* 0xFFAF 	 XK_KP_Divide */
   "\0010",	/* 0xFFB0 	 XK_KP_0 */
   "\0011",	/* 0xFFB1 	 XK_KP_1 */
   "\0012",	/* 0xFFB2 	 XK_KP_2 */
   "\0013",	/* 0xFFB3 	 XK_KP_3 */
   "\0014",	/* 0xFFB4 	 XK_KP_4 */		/* shift 180 */
   "\0015",	/* 0xFFB5 	 XK_KP_5 */
   "\0016",	/* 0xFFB6 	 XK_KP_6 */
   "\0017",	/* 0xFFB7 	 XK_KP_7 */
   "\0018",	/* 0xFFB8 	 XK_KP_8 */
   "\0019",	/* 0xFFB9 	 XK_KP_9 */
   "",			/* 0xFFBA */
   "",			/* 0xFFBB */
   "",			/* 0xFFBC */
   "\001e",			/* 0xFFBD 	 XK_KP_Equal */
   "\006\033[1;2P",	/* 0xFFBE 	 XK_F1 */
   "\006\033[1;2Q",	/* 0xFFBF 	 XK_F2 */
   "\006\033[1;2R",	/* 0xFFC0 	 XK_F3 */
   "\006\033[1;2S",	/* 182 0xFFC1 	 XK_F4 */
   "\007\033[15;2~",/* 57  0xFFC2 	 XK_F5 */
   "\007\033[17;2~",/* 1 0xFFC3 	 XK_F6 */
   "\007\033[18;2~",/* 101 0xFFC4 	 XK_F7 */
   "\007\033[19;2~",/* 201 0xFFC5 	 XK_F8 */
   "\007\033[20;2~",	/* 0xFFC6 	 XK_F9 */
   "\007\033[21;2~",	/* 0xFFC7 	 XK_F10 */
   "\005\033[23$",	/* 0xFFC8 	 XK_F11 	 XK_L1 */
   "\005\033[24$",	/* 0xFFC9 	 XK_F12 	 XK_L2 */
   "\005\033[25$",	/* 0xFFCA 	 XK_F13 	 XK_L3 */
   "\005\033[26$",	/* 0xFFCB 	 XK_F14 	 XK_L4 */
   "\005\033[28$",	/* 0xFFCC 	 XK_F15 	 XK_L5 */
   "\005\033[29$",	/* 0xFFCD 	 XK_F16 	 XK_L6 */
   "\005\033[31$",	/* 0xFFCE 	 XK_F17 	 XK_L7 */
   "\005\033[32$",	/* 0xFFCF 	 XK_F18 	 XK_L8 */
   "\005\033[33$",	/* 0xFFD0 	 XK_F19 	 XK_L9 */
   "\005\033[34$",	/* 0xFFD1 	 XK_F20 	 XK_L10 */
   "\005\033[35$",	/* 0xFFD2 	 XK_F21 	 XK_R1 */
   "\005\033[36$",	/* 0xFFD3 	 XK_F22 	 XK_R2 */
   "\005\033[37$",	/* 0xFFD4 	 XK_F23 	 XK_R3 */
   "\005\033[38$",	/* 0xFFD5 	 XK_F24 	 XK_R4 */
   "\005\033[39$",	/* 0xFFD6 	 XK_F25 	 XK_R5 */
   "\005\033[40$",	/* 0xFFD7 	 XK_F26 	 XK_R6 */
   "\005\033[41$",	/* 0xFFD8 	 XK_F27 	 XK_R7 */
   "\005\033[42$",	/* 0xFFD9 	 XK_F28 	 XK_R8 */
   "\005\033[43$",	/* 0xFFDA 	 XK_F29 	 XK_R9 */
   "\005\033[44$",	/* 0xFFDB 	 XK_F30 	 XK_R10 */
   "\005\033[45$",	/* 0xFFDC 	 XK_F31 	 XK_R11 */
   "\005\033[46$",	/* 0xFFDD 	 XK_F32 	 XK_R12 */
   "\005\033[47$",	/* 0xFFDE 	 XK_R13 	 XK_F33 */
   "\005\033[48$",	/* 0xFFDF 	 XK_F34 	 XK_R14 */
   "\005\033[49$",	/* 0xFFE0 	 XK_F35 	 XK_R15 */
   "",			/* 0xFFE1 	 XK_Shift_L 	 XK_Shift_L */
   "",			/* 0xFFE2 	 XK_Shift_R */
   "",			/* 0xFFE3 	 XK_Control_L */
   "",			/* 0xFFE4 	 XK_Control_R */
   "",			/* 0xFFE5 	 XK_Caps_Lock */
   "",			/* 0xFFE6 	 XK_Shift_Lock */
   "",			/* 0xFFE7 	 XK_Meta_L */
   "",			/* 0xFFE8 	 XK_Meta_R */
   "",			/* 0xFFE9 	 XK_Alt_L */
   "",			/* 0xFFEA 	 XK_Alt_R */
   "",			/* 0xFFEB 	 XK_Super_L */
   "",			/* 0xFFEC 	 XK_Super_R */
   "",			/* 0xFFED 	 XK_Hyper_L */
   "",			/* 0xFFEE 	 XK_Hyper_R */
   "",			/* 0xFFEF */
   "",			/* 0xFFF0 */
   "",			/* 0xFFF1 */
   "",			/* 0xFFF2 */
   "",			/* 0xFFF3 */
   "",			/* 0xFFF4 */
   "",			/* 0xFFF5 */
   "",			/* 0xFFF6 */
   "",			/* 0xFFF7 */
   "",			/* 0xFFF8 */
   "",			/* 0xFFF9 */
   "",			/* 0xFFFA */
   "",			/* 0xFFFB */
   "",			/* 0xFFFC */
   "",			/* 0xFFFD */
   "",			/* 0xFFFE */
   "\001\177"		/* 0xFFFF 	 XK_Delete */
};

 /* control */
static KeySym_Mapping_Type Control_KeySym_Mapping[256] =
{
   "\004\033[3^",	/* 0xFF00                        Remove */
   "",			/* 0xFF01 */
   "",			/* 0xFF02 */
   "",			/* 0xFF03 */
   "",			/* 0xFF04 */
   "",			/* 0xFF05 */
   "",			/* 0xFF06 */
   "",			/* 0xFF07 */
   "\002\033\010",	/* 0xFF08 	 XK_BackSpace */
   "\003\033[w",	/* 0xFF09 	 XK_Tab */
   "\001\n",		/* 0xFF0A 	 XK_Linefeed */
   "",			/* 0xFF0B 	 XK_Clear */
   "",			/* 0xFF0C */
   "\001\r",		/* 0xFF0D 	 XK_Return */
   "",			/* 0xFF0E */
   "",			/* 0xFF0F */
   "",			/* 0xFF10 */
   "",			/* 0xFF11 */
   "",			/* 0xFF12 */
   "",			/* 0xFF13 	 XK_Pause */
   "",			/* 0xFF14 	 XK_Scroll_Lock */
   "",			/* 0xFF15 */
   "",			/* 0xFF16 */
   "",			/* 0xFF17 */
   "",			/* 0xFF18 */
   "",			/* 0xFF19 */
   "",			/* 0xFF1A */
   "\001\033",		/* 0xFF1B 	 XK_Escape */
   "",			/* 0xFF1C */
   "\001\036",			/* 0xFF1D */
   "\001\037",			/* 0xFF1E */
   "",			/* 0xFF1F */
   "",			/* 0xFF20 	 XK_Multi_key */
   "",			/* 0xFF21 	 XK_Kanji */
   "",			/* 0xFF22 	 XK_Muhenkan */
   "",			/* 0xFF23 	 XK_Henkan_Mode 	 XK_Henkan */
   "",			/* 0xFF24 	 XK_Romaji */
   "",			/* 0xFF25 	 XK_Hiragana */
   "\003\033[w",			/* 0xFF26 	 XK_Katakana */
   "\003\033[z",			/* 0xFF27 	 XK_Hiragana_Katakana */
   "",			/* 0xFF28 	 XK_Zenkaku */
   "",			/* 0xFF29 	 XK_Hankaku */
   "",			/* 0xFF2A 	 XK_Zenkaku_Hankaku */
   "",			/* 0xFF2B 	 XK_Touroku */
   "",			/* 0xFF2C 	 XK_Massyo */
   "",			/* 0xFF2D 	 XK_Kana_Lock */
   "",			/* 0xFF2E 	 XK_Kana_Shift */
   "",			/* 0xFF2F 	 XK_Eisu_Shift */
   "",			/* 0xFF30 	 XK_Eisu_toggle */
   "",			/* 0xFF31 */
   "",			/* 0xFF32 */
   "",			/* 0xFF33 */
   "",			/* 0xFF34 */
   "",			/* 0xFF35 */
   "",			/* 0xFF36 */
   "",			/* 0xFF37 */
   "",			/* 0xFF38 */
   "",			/* 0xFF39 */
   "",			/* 0xFF3A */
   "",			/* 0xFF3B */
   "",			/* 0xFF3C */
   "",			/* 0xFF3D */
   "",			/* 0xFF3E */
   "",			/* 0xFF3F */
   "",			/* 0xFF40 */
   "",			/* 0xFF41 */
   "",			/* 0xFF42 */
   "",			/* 0xFF43 */
   "",			/* 0xFF44 */
   "",			/* 0xFF45 */
   "",			/* 0xFF46 */
   "",			/* 0xFF47 */
   "",			/* 0xFF48 */
   "",			/* 0xFF49 */
   "",			/* 0xFF4A */
   "",			/* 0xFF4B */
   "",			/* 0xFF4C */
   "",			/* 0xFF4D */
   "",			/* 0xFF4E */
   "",			/* 0xFF4F */
   "\002\033<",		/* 0xFF50 	 XK_Home */
   "\003\033[P",	/* 0xFF51 	 XK_Left */
   "\005\033[17~",	/* 0xFF52 	 XK_Up */
   "\003\033[N",	/* 0xFF53 	 XK_Right */
   "\005\033[15~",	/* 0xFF54 	 XK_Down */
   "\003\033[w",	/* 0xFF55 	 XK_Prior */
   "\003\033[z",	/* 0xFF56 	 XK_Next */
   "\002\033>",		/* 0xFF57 	 XK_End */
   "\001\001",		/* 0xFF58 	 XK_Begin */
   "",			/* 0xFF59 */
   "",			/* 0xFF5A */
   "",			/* 0xFF5B */
   "",			/* 0xFF5C */
   "",			/* 0xFF5D */
   "",			/* 0xFF5E */
   "",			/* 0xFF5F */
   "\004\033[4^",	/* 0xFF60 	 XK_Select */
   "",			/* 0xFF61 	 XK_Print */
   "",			/* 0xFF62 	 XK_Execute */
   "\004\033[2^",	/* 0xFF63 	 XK_Insert */
   "",			/* 0xFF64 */
   "\001\037",		/* 0xFF65 	 XK_Undo 	 ^_ */
   "",			/* 0xFF66 	 XK_Redo */
   "\005\033[29^",	/* 0xFF67 	 XK_Menu         Do */
   "\004\033[1^",	/* 0xFF68 	 XK_Find */
   "\001\007",		/* 0xFF69 	 XK_Cancel */
   "\005\033[28^",	/* 0xFF6A 	 XK_Help */
   "",			/* 0xFF6B 	 XK_Break */
   "",			/* 0xFF6C */
   "",			/* 0xFF6D */
   "",			/* 0xFF6E */
   "",			/* 0xFF6F */
   "",			/* 0xFF70 */
   "",			/* 0xFF71 */
   "",			/* 0xFF72 */
   "",			/* 0xFF73 */
   "",			/* 0xFF74 */
   "",			/* 0xFF75 */
   "",			/* 0xFF76 */
   "",			/* 0xFF77 */
   "",			/* 0xFF78 */
   "",			/* 0xFF79 */
   "",			/* 0xFF7A */
   "",			/* 0xFF7B */
   "",			/* 0xFF7C */
   "",			/* 0xFF7D */
   "\003\033[f",			/* 0xFF7E */
   "",			/* 0xFF7F 	 XK_Num_Lock */
   "",			/* 0xFF80 	 XK_KP_Space */
   "",			/* 0xFF81 */
   "",			/* 0xFF82 */
   "",			/* 0xFF83 */
   "",			/* 0xFF84 */
   "",			/* 0xFF85 */
   "",			/* 0xFF86 */
   "",			/* 0xFF87 */
   "",			/* 0xFF88 */
   "",			/* 0xFF89 	 XK_KP_Tab */
   "",			/* 0xFF8A */
   "",			/* 0xFF8B */
   "",			/* 0xFF8C */
   "\001\015",		/* 0xFF8D 	 XK_KP_Enter */
   "",			/* 0xFF8E */
   "",			/* 0xFF8F */
   "",			/* 0xFF90 */
   "\003\033OP",	/* 0xFF91 	 XK_KP_F1        PF1 */
   "\003\033OQ",	/* 0xFF92 	 XK_KP_F2        PF2 */
   "\003\033OR",	/* 0xFF93 	 XK_KP_F3        PF3 */
   "\003\033OS",	/* 0xFF94 	 XK_KP_F4        PF4 */
   "\002\033<",			/* 0xFF95 */			
   "\003\033[P",			/* 0xFF96 */			/* control 150 */
   "\005\033[17~",			/* 0xFF97 ctrl up */
   "\003\033[N",			/* 0xFF98 */
   "\005\033[15~",			/* 0xFF99 ctrl down */
   "\002\030P",			/* 0xFF9A ctrl pgup*/
   "\003\033[z",			/* 0xFF9B ctrl pgdown */
   "\002\033>",			/* 0xFF9C ctrl end*/
   "\002\030P",			/* 0xFF9D crtl key center */
   "\003\033[I",			/* 0xFF9E ctrl insert */
   "\003\033[W",			/* 0xFF9F ctrl delete */
   "",			/* 0xFFA0 */
   "",			/* 0xFFA1 */
   "",			/* 0xFFA2 */
   "",			/* 0xFFA3 */
   "",			/* 0xFFA4 */
   "",			/* 0xFFA5 */
   "",			/* 0xFFA6 */
   "",			/* 0xFFA7 */
   "",			/* 0xFFA8 */
   "",			/* 0xFFA9 */
   "",			/* 0xFFAA 	 XK_KP_Multiply */
   "",			/* 0xFFAB 	 XK_KP_Add */
   "\003\033Ol",	/* 0xFFAC 	 XK_KP_Separator KP , */
   "\003\033Om",	/* 0xFFAD 	 XK_KP_Subtract  KP - */
   "\003\033On",	/* 0xFFAE 	 XK_KP_Decimal   KP . */
   "",			/* 0xFFAF 	 XK_KP_Divide */
   "\0010",	/* 0xFFB0 	 XK_KP_0 */
   "\0011",	/* 0xFFB1 	 XK_KP_1 */
   "\0012",	/* 0xFFB2 	 XK_KP_2 */
   "\0013",	/* 0xFFB3 	 XK_KP_3 */		/* 180 */
   "\0014",	/* 0xFFB4 	 XK_KP_4 */
   "\002\030P",	/* 0xFFB5 	 XK_KP_5 */
   "\0015",	/* 0xFFB6 	 XK_KP_6 */
   "\0017",	/* 0xFFB7 	 XK_KP_7 */
   "\0018",	/* 0xFFB8 	 XK_KP_8 */
   "\0019",	/* 0xFFB9 	 XK_KP_9 */
   "",			/* 0xFFBA */
   "",			/* 0xFFBB */
   "",			/* 0xFFBC */
   "",			/* 0xFFBD 	 XK_KP_Equal */
   "\005\033[11^",	/* 0xFFBE 	 XK_F1 */
   "\005\033[12^",	/* 0xFFBF 	 XK_F2 */
   "\005\033[13^",	/* 0xFFC0 	 XK_F3 */
   "\005\033[14^",	/* 0xFFC1 	 XK_F4 */
   "\005\033[15^",	/* 0xFFC2 	 XK_F5 */
   "\005\033[17^",	/* 0xFFC3 	 XK_F6 */
   "\005\033[18^",	/* 0xFFC4 	 XK_F7 */
   "\005\033[19^",	/* 0xFFC5 	 XK_F8 */
   "\005\033[20^",	/* 0xFFC6 	 XK_F9 */
   "\005\033[21^",	/* 0xFFC7 	 XK_F10 */
   "\005\033[23^",	/* 0xFFC8 	 XK_F11 	 XK_L1 */
   "\005\033[24^",	/* 0xFFC9 	 XK_F12 	 XK_L2 */
   "\005\033[25^",	/* 0xFFCA 	 XK_F13 	 XK_L3 */
   "\005\033[26^",	/* 0xFFCB 	 XK_F14 	 XK_L4 */
   "\005\033[28^",	/* 0xFFCC 	 XK_F15 	 XK_L5 */
   "\005\033[29^",	/* 0xFFCD 	 XK_F16 	 XK_L6 */
   "\005\033[31^",	/* 0xFFCE 	 XK_F17 	 XK_L7 */
   "\005\033[32^",	/* 0xFFCF 	 XK_F18 	 XK_L8 */
   "\005\033[33^",	/* 0xFFD0 	 XK_F19 	 XK_L9 */
   "\005\033[34^",	/* 0xFFD1 	 XK_F20 	 XK_L10 */
   "\005\033[35^",	/* 0xFFD2 	 XK_F21 	 XK_R1 */
   "\005\033[36^",	/* 0xFFD3 	 XK_F22 	 XK_R2 */
   "\005\033[37^",	/* 0xFFD4 	 XK_F23 	 XK_R3 */
   "\005\033[38^",	/* 0xFFD5 	 XK_F24 	 XK_R4 */
   "\005\033[39^",	/* 0xFFD6 	 XK_F25 	 XK_R5 */
   "\005\033[40^",	/* 0xFFD7 	 XK_F26 	 XK_R6 */
   "\005\033[41^",	/* 0xFFD8 	 XK_F27 	 XK_R7 */
   "\005\033[42^",	/* 0xFFD9 	 XK_F28 	 XK_R8 */
   "\005\033[43^",	/* 0xFFDA 	 XK_F29 	 XK_R9 */
   "\005\033[44^",	/* 0xFFDB 	 XK_F30 	 XK_R10 */
   "\005\033[45^",	/* 0xFFDC 	 XK_F31 	 XK_R11 */
   "\005\033[46^",	/* 0xFFDD 	 XK_F32 	 XK_R12 */
   "\005\033[47^",	/* 0xFFDE 	 XK_R13 	 XK_F33 */
   "\005\033[48^",	/* 0xFFDF 	 XK_F34 	 XK_R14 */
   "\005\033[49^",	/* 0xFFE0 	 XK_F35 	 XK_R15 */
   "",			/* 0xFFE1 	 XK_Shift_L 	 XK_Shift_L */
   "",			/* 0xFFE2 	 XK_Shift_R */
   "",			/* 0xFFE3 	 XK_Control_L */
   "",			/* 0xFFE4 	 XK_Control_R */
   "",			/* 0xFFE5 	 XK_Caps_Lock */
   "",			/* 0xFFE6 	 XK_Shift_Lock */
   "",			/* 0xFFE7 	 XK_Meta_L */
   "",			/* 0xFFE8 	 XK_Meta_R */
   "",			/* 0xFFE9 	 XK_Alt_L */
   "",			/* 0xFFEA 	 XK_Alt_R */
   "",			/* 0xFFEB 	 XK_Super_L */
   "",			/* 0xFFEC 	 XK_Super_R */
   "",			/* 0xFFED 	 XK_Hyper_L */
   "",			/* 0xFFEE 	 XK_Hyper_R */
   "",			/* 0xFFEF */
   "",			/* 0xFFF0 */
   "",			/* 0xFFF1 */
   "",			/* 0xFFF2 */
   "",			/* 0xFFF3 */
   "",			/* 0xFFF4 */
   "",			/* 0xFFF5 */
   "",			/* 0xFFF6 */
   "",			/* 0xFFF7 */
   "",			/* 0xFFF8 */
   "",			/* 0xFFF9 */
   "",			/* 0xFFFA */
   "",			/* 0xFFFB */
   "",			/* 0xFFFC */
   "",			/* 0xFFFD */
   "",			/* 0xFFFE */
   "\002\033\177"	/* 0xFFFF 	 XK_Delete */
};


static KeySym_Mapping_Type Alt_KeySym_Mapping[256] =
{
   "\004\033[3~",	/* 0xFF00                        Remove */
   "",			/* 0xFF01 */
   "",			/* 0xFF02 */
   "",			/* 0xFF03 */
   "",			/* 0xFF04 */
   "",			/* 0xFF05 */
   "",			/* 0xFF06 */
   "\001w",			/* 0xFF07 */
   "\001\010",		/* 0xFF08 	 XK_BackSpace */
   "\001\t",		/* 0xFF09 	 XK_Tab */
   "\001\n",		/* 0xFF0A 	 XK_Linefeed */
   "",			/* 0xFF0B 	 XK_Clear */
   "",			/* 0xFF0C */
   "\001\r",		/* 0xFF0D 	 XK_Return */
   "",			/* 0xFF0E */
   "",			/* 0xFF0F */
   "",			/* 0xFF10 */
   "",			/* 0xFF11 */
   "",			/* 0xFF12 */
   "",			/* 0xFF13 	 XK_Pause */
   "",			/* 0xFF14 	 XK_Scroll_Lock */
   "",			/* 0xFF15 */
   "",			/* 0xFF16 */
   "",			/* 0xFF17 */
   "",			/* 0xFF18 */
   "",			/* 0xFF19 */
   "",			/* 0xFF1A */
   "\001\033",		/* 0xFF1B 	 XK_Escape */
   "",			/* 0xFF1C */
   "",			/* 0xFF1D */
   "",			/* 0xFF1E */
   "",			/* 0xFF1F */
   "",			/* 0xFF20 	 XK_Multi_key */
   "",			/* 0xFF21 	 XK_Kanji */
   "",			/* 0xFF22 	 XK_Muhenkan */
   "",			/* 0xFF23 	 XK_Henkan_Mode 	 XK_Henkan */
   "",			/* 0xFF24 	 XK_Romaji */
   "",			/* 0xFF25 	 XK_Hiragana */
   "\004\033[5~",			/* 0xFF26 	 XK_Katakana */
   "\004\033[6~",			/* 0xFF27 	 XK_Hiragana_Katakana */
   "",			/* 0xFF28 	 XK_Zenkaku */
   "",			/* 0xFF29 	 XK_Hankaku */
   "",			/* 0xFF2A 	 XK_Zenkaku_Hankaku */
   "",			/* 0xFF2B 	 XK_Touroku */
   "",			/* 0xFF2C 	 XK_Massyo */
   "",			/* 0xFF2D 	 XK_Kana_Lock */
   "",			/* 0xFF2E 	 XK_Kana_Shift */
   "",			/* 0xFF2F 	 XK_Eisu_Shift */
   "",			/* 0xFF30 	 XK_Eisu_toggle */
   "",			/* 0xFF31 */
   "",			/* 0xFF32 */
   "",			/* 0xFF33 */
   "",			/* 0xFF34 */
   "",			/* 0xFF35 */
   "",			/* 0xFF36 */
   "",			/* 0xFF37 */
   "",			/* 0xFF38 */
   "",			/* 0xFF39 */
   "",			/* 0xFF3A */
   "",			/* 0xFF3B */
   "",			/* 0xFF3C */
   "",			/* 0xFF3D */
   "",			/* 0xFF3E */
   "",			/* 0xFF3F */
   "",			/* 0xFF40 */
   "",			/* 0xFF41 */
   "",			/* 0xFF42 */
   "",			/* 0xFF43 */
   "",			/* 0xFF44 */
   "",			/* 0xFF45 */
   "",			/* 0xFF46 */
   "",			/* 0xFF47 */
   "",			/* 0xFF48 */
   "",			/* 0xFF49 */
   "",			/* 0xFF4A */
   "",			/* 0xFF4B */
   "",			/* 0xFF4C */
   "",			/* 0xFF4D */
   "",			/* 0xFF4E */
   "",			/* 0xFF4F */
   "\001\001",		/* 0xFF50 	 XK_Home */
#if	DARWIN
   "\002\033P",	/* 0xFF55 	 XK_Prior */
   "\003\033[w",	/* 0xFF52 	 next window */
   "\002\033N",	/* 0xFF53 	 XK_Right */
   "\003\033[z",	/* 0xFF54 	 next file */
#else
   "\003\033[p",	/* 0xFF51 	 XK_Left */
   "\003\033[w",	/* 0xFF52 	 next window */
   "\003\033[n",	/* 0xFF53 	 XK_Right */
   "\003\033[z",	/* 0xFF54 	 next file */
#endif
   "\004\033[5~",	/* 0xFF55 	 XK_Prior */
   "\004\033[6~",	/* 0xFF56 	 XK_Next */
   "\001\005",		/* 0xFF57 	 XK_End */
   "\001\001",		/* 0xFF58 	 XK_Begin */
   "\001\002",			/* 0xFF59 */
   "\001\003",			/* 0xFF5A */
   "\001\004",			/* 0xFF5B */
   "\001\005",			/* 0xFF5C */
   "\001\006",			/* 0xFF5D */
   "\001\007",			/* 0xFF5E */
   "\001\008",			/* 0xFF5F */
   "\004\033[4~",	/* 0xFF60 	 XK_Select */
   "",			/* 0xFF61 	 XK_Print */
   "",			/* 0xFF62 	 XK_Execute */
   "\004\033[2~",	/* 0xFF63 	 XK_Insert */
   "\001\011",			/* 0xFF64 */
   "\001\037",		/* 0xFF65 	 XK_Undo 	 ^_ */
   "\004redo",			/* 0xFF66 	 XK_Redo */
   "\005\033[29~",	/* 0xFF67 	 XK_Menu        Do */
   "\004\033[1~",	/* 0xFF68 	 XK_Find */
   "\001\007",		/* 0xFF69 	 XK_Cancel */
   "\003\033OP",	/* 0xFF6A 	 XK_Help */
   "",			/* 0xFF6B 	 XK_Break */
   "",			/* 0xFF6C */
   "",			/* 0xFF6D */
   "",			/* 0xFF6E */
   "",			/* 0xFF6F */
   "",			/* 0xFF70 */
   "",			/* 0xFF71 */
   "",			/* 0xFF72 */
   "",			/* 0xFF73 */
   "",			/* 0xFF74 */
   "",			/* 0xFF75 */
   "",			/* 0xFF76 */
   "",			/* 0xFF77 */
   "",			/* 0xFF78 */
   "",			/* 0xFF79 */
   "",			/* 0xFF7A */
   "",			/* 0xFF7B */
   "",			/* 0xFF7C */
   "",			/* 0xFF7D */
   "\003\033[s",	/* 0xFF7E 	right alt */ 
   "",			/* 0xFF7F 	 XK_Num_Lock */
   "",			/* 0xFF80 	 XK_KP_Space */
   "",			/* 0xFF81 */
   "",			/* 0xFF82 */
   "",			/* 0xFF83 */
   "",			/* 0xFF84 */
   "",			/* 0xFF85 */
   "",			/* 0xFF86 */
   "",			/* 0xFF87 */
   "",			/* 0xFF88 */
   "",			/* 0xFF89 	 XK_KP_Tab */
   "",			/* 0xFF8A */
   "",			/* 0xFF8B */
   "",			/* 0xFF8C */
   "\001\015",		/* 0xFF8D 	 XK_KP_Enter */
   "",			/* 0xFF8E */
   "",			/* 0xFF8F */
   "",			/* 0xFF90 */
   "\003\033OP",	/* 0xFF91 	 XK_KP_F1        PF1 */
   "\003\033OQ",	/* 0xFF92 	 XK_KP_F2        PF2 */
   "\003\033OR",	/* 0xFF93 	 XK_KP_F3        PF3 */
   "\003\033OS",	/* 0xFF94 	 XK_KP_F4        PF4 */
   "\001\001",		/* 0xFF95 	HOME */
   "\003\033[D",			/* 0xFF96 */		/* 150 */
   "\003\033[A",			/* 0xFF97 */
   "\003\033[C",			/* 0xFF98 */
   "\003\033[B",			/* 0xFF99 */
   "\004\033[5~",	/* 0xFF9A */
   "\004\033[6~",	/* 0xFF9B */
   "\001\005",	/* 0xFF9C */
   "\005\033[15~",	/* 0xFF9D central key */
   "\001\031",		/* 0xFF9E INS*/
   "\001\177",		/* 0xFF9F Numeric . */	
   "",			/* 0xFFA0 */				/* 160 */
   "",			/* 0xFFA1 */
   "",			/* 0xFFA2 */
   "",			/* 0xFFA3 */
   "",			/* 0xFFA4 */
   "",			/* 0xFFA5 */
   "",			/* 0xFFA6 */
   "",			/* 0xFFA7 */
   "",			/* 0xFFA8 */
   "",			/* 0xFFA9 */
   "\001*",		/* 0xFFAA 	 XK_KP_Multiply */
   "\001+",		/* 0xFFAB 	 XK_KP_Add */
   "\003\033Ol",	/* 0xFFAC 	 XK_KP_Separator KP , */
   "\001-",		/* 0xFFAD 	 XK_KP_Subtract  KP - */
   "\001.",	/* 0xFFAE 	 XK_KP_Decimal   KP . */
   "\001/",		/* 0xFFAF 	 XK_KP_Divide */
   "\0010",	/* 0xFFB0 	 XK_KP_0 */
   "\0011",	/* 0xFFB1 	 XK_KP_1 */
   "\0012",	/* 0xFFB2 	 XK_KP_2 */
   "\0013",	/* 0xFFB3 	 XK_KP_3 */
   "\0014",	/* 0xFFB4 	 XK_KP_4 */
   "\005\033[15~",	/* 0xFFB5	 XK_KP_5 */
   "\0016",	/* 0xFFB6 	 XK_KP_6 */
   "\0017",	/* 0xFFB7 	 XK_KP_7 */
   "\0018",	/* 0xFFB8 	 XK_KP_8 */
   "\0019",	/* 0xFFB9 	 XK_KP_9 */
   "\001a",		/* 0xFFBA */
   "\001b",		/* 0xFFBB */
   "\001c",		/* 0xFFBC */
   "\001=",		/* 0xFFBD 	 XK_KP_Equal */
   "\003\033OP",	/* 0xFFBE 	 XK_F1 */  /* xterm convention */
   "\005\033[12~",	/* 0xFFBF 	 XK_F2 */  /* xterm convention */ /* 190 */
   "\005\033[13~",	/* 0xFFC0 	 XK_F3 */  /* xterm convention */
   "\005\033[14~",	/* 0xFFC1 	 XK_F4 */  /* xterm convention */
   "\005\033[15~",	/* 0xFFC2 	 XK_F5 */  /* xterm convention */
   "\005\033[17~",	/* 0xFFC3 	 XK_F6 */
   "\005\033[18~",	/* 0xFFC4 	 XK_F7 */
   "\005\033[19~",	/* 0xFFC5 	 XK_F8 */
   "\005\033[20~",	/* 0xFFC6 	 XK_F9 */
   "\005\033[21~",	/* 0xFFC7 	 XK_F10 */
   "\005\033[23~",	/* 0xFFC8 	 XK_F11 	 XK_L1 */
   "\003\033Oa",	/* 0xFFC9 	 XK_F12 	 XK_L2 */	/* 200 */
   "\005\033[25~",	/* 0xFFCA 	 XK_F13 	 XK_L3 */
   "\005\033[26~",	/* 0xFFCB 	 XK_F14 	 XK_L4 */
   "\003\033OP",	/* 0xFFCC 	 XK_F15 	 XK_L5 */
   "\005\033[29~",	/* 0xFFCD 	 XK_F16 	 XK_L6 */
   "\005\033[31~",	/* 0xFFCE 	 XK_F17 	 XK_L7 */
   "\005\033[32~",	/* 0xFFCF 	 XK_F18 	 XK_L8 */
   "\005\033[33~",	/* 0xFFD0 	 XK_F19 	 XK_L9 */
   "\005\033[34~",	/* 0xFFD1 	 XK_F20 	 XK_L10 */
   "\005\033[35~",	/* 0xFFD2 	 XK_F21 	 XK_R1 */
   "\005\033[36~",	/* 0xFFD3 	 XK_F22 	 XK_R2 */	/* 210 */
   "\005\033[37~",	/* 0xFFD4 	 XK_F23 	 XK_R3 */
   "\005\033[38~",	/* 0xFFD5 	 XK_F24 	 XK_R4 */
   "\005\033[39~",	/* 0xFFD6 	 XK_F25 	 XK_R5 */
   "\005\033[40~",	/* 0xFFD7 	 XK_F26 	 XK_R6 */
   "\005\033[41~",	/* 0xFFD8 	 XK_F27 	 XK_R7 */
   "\005\033[42~",	/* 0xFFD9 	 XK_F28 	 XK_R8 */
   "\005\033[43~",	/* 0xFFDA 	 XK_F29 	 XK_R9 */
   "\005\033[44~",	/* 0xFFDB 	 XK_F30 	 XK_R10 */
   "\005\033[45~",	/* 0xFFDC 	 XK_F31 	 XK_R11 */
   "\005\033[46~",	/* 0xFFDD 	 XK_F32 	 XK_R12 */	/* 220 */
   "\005\033[47~",	/* 0xFFDE 	 XK_R13 	 XK_F33 */
   "\005\033[48~",	/* 0xFFDF 	 XK_F34 	 XK_R14 */
   "\005\033[49~",	/* 0xFFE0 	 XK_F35 	 XK_R15 */
   "",			/* 0xFFE1 	 XK_Shift_L 	 XK_Shift_L */
   "",			/* 0xFFE2 	 XK_Shift_R */
   "",			/* 0xFFE3 	 XK_Control_L */
   "",			/* 0xFFE4 	 XK_Control_R */
   "",			/* 0xFFE5 	 XK_Caps_Lock */
   "",			/* 0xFFE6 	 XK_Shift_Lock */
   "",			/* 0xFFE7 	 XK_Meta_L */
   "",			/* 0xFFE8 	 XK_Meta_R */
   "",			/* 0xFFE9 	 XK_Alt_L */
   "",			/* 0xFFEA 	 XK_Alt_R */
   "",			/* 0xFFEB 	 XK_Super_L */
   "",			/* 0xFFEC 	 XK_Super_R */
   "",			/* 0xFFED 	 XK_Hyper_L */
   "",			/* 0xFFEE 	 XK_Hyper_R */
   "",			/* 0xFFEF */
   "",			/* 0xFFF0 */
   "",			/* 0xFFF1 */
   "",			/* 0xFFF2 */
   "",			/* 0xFFF3 */
   "",			/* 0xFFF4 */
   "",			/* 0xFFF5 */
   "",			/* 0xFFF6 */
   "",			/* 0xFFF7 */
   "",			/* 0xFFF8 */
   "",			/* 0xFFF9 */
   "",			/* 0xFFFA */
   "",			/* 0xFFFB */
   "",			/* 0xFFFC */
   "",			/* 0xFFFD */
   "",			/* 0xFFFE */
   "\001\177"		/* 0xFFFF 	 XK_Delete */
};

