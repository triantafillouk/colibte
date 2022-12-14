/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/*
 embeded icons
*/

char *quit_icon[] = {
/* width height num_colors chars_per_pixel */
"14 14 15 1",
" 	c None",
".	c #FF0000",
"+	c #FF5252",
"@	c #B52323",
"#	c #FF7676",
"$	c #FF5656",
"%	c #FF3636",
"&	c #FF6666",
"*	c #FF0404",
"=	c #FF3A3A",
"-	c #FF0101",
";	c #FF5F5F",
">	c #FF0202",
",	c #FF0B0B",
"'	c #FF1C1C",
"              ",
"@..#       @..",
" @..$     @%.,",
"  @..&   @.., ",
"   @..& @.%,  ",
"    @.*...,   ",
"     @...,    ",
"    @+@..$    ",
"   @+&>@..$   ",
"  @+.,  @..$  ",
" @+.,    @..$ ",
"@+.,      @..#",
"..,        @..",
"              "
}; 
 
/* XPM */
char *fileopen_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 71 1",
"  c #000000",
". c #0b0704",
"X c #150f09",
"o c #141414",
"O c #191919",
"+ c #25170c",
"@ c #26190e",
"# c #2c1c0f",
"$ c #271c11",
"% c #352212",
"& c #3b2714",
"* c #262625",
"= c #2c2c2b",
"- c #343434",
"; c #383837",
": c #432b16",
"> c #472e19",
", c #4b3119",
"< c #7e532c",
"1 c #53504d",
"2 c #5d5d5b",
"3 c #676766",
"4 c #6d6864",
"5 c #6d6d6c",
"6 c #7d7d7c",
"7 c #80532c",
"8 c #855e38",
"9 c #a66d3a",
"0 c #aa6f3b",
"q c #b57740",
"w c #ea9e58",
"e c #eea059",
"r c #f2a55b",
"t c #e5a566",
"y c #f4ad6a",
"u c #f8ac62",
"i c #fdb36a",
"p c #f6b675",
"a c #fdb773",
"s c #feb971",
"d c #fec27f",
"f c #818180",
"g c #8c8c8b",
"h c #90908f",
"j c #949494",
"k c #9c9c9b",
"l c #a0a09e",
"z c #a5a5a4",
"x c #a9a9a7",
"c c #adadac",
"v c #b0b0af",
"b c #b4b4b3",
"n c #b8b8b7",
"m c #bcbcbb",
"M c #c0c0bf",
"N c #fec282",
"B c #fec98b",
"V c #fecd93",
"C c #fed39a",
"Z c #c4c4c2",
"A c #c8c8c7",
"S c #cdcdcb",
"D c #d0d0cf",
"F c #d4d4d3",
"G c #d8d8d6",
"H c #dcdcdb",
"J c #e0e0df",
"K c #e4e4e3",
"L c #eaeae9",
"P c #f3f3f3",
"I c None",
/* pixels */
"IIIIIIIIIIIIIIIIIIIIII",
"IIIIIIIII     IIIIIIII",
"IIIIIII         I IIII",
"IIIIII  IIIIII    IIII",
"IIIII IIIIIIIII   IIII",
"IIIIIIIIIIIIII    IIII",
"II@$$$IIIIIIIIIIIIIIII",
"I&yNNs&IIIIIIIIIIIIIII",
"&tVCVBp&XXXXXXXXIIIIII",
"&NCVNiuuuuuuuuuu&IIIII",
"&BCNe7,,,,::&%%#@+    ",
"&BVa01PPPPLKHGSAZmbvc;",
"&NNr>ALKJGDAZnvzkjgf2O",
"&Ni01PJLLKHFAZnczkg6;I",
"&aw,SGJLLKHGSZmbzkg5 I",
"&i91HFJKKHGDAMbczjf=II",
"&r>cAFHHGFDAZnbxkh2oII",
"&q1DZDFFDSAMmbczkf;III",
"&>cbmZAAAZmmbczkg3 III",
".4Acbbnmmbbcxlkh6=IIII",
"ohkkkllkkkjjggg62oIIII",
"o-==========****oIIIII"
};
/* XPM */
static char *filesave_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 35 1",
"  c #000000",
". c #01332f",
"X c #3d3d3d",
"o c #024441",
"O c #015d5b",
"+ c #026664",
"@ c #444444",
"# c #4e4e4e",
"$ c #555555",
"% c #5b5b5b",
"& c #636363",
"* c #6c6c6c",
"= c #747474",
"- c #7b7b7b",
"; c #3e8786",
": c #53adab",
"> c #838383",
", c #8b8b8b",
"< c #939393",
"1 c #9a9a9a",
"2 c #a6a6a6",
"3 c #ababab",
"4 c #b3b3b3",
"5 c #bcbcbc",
"6 c #c5c5c5",
"7 c #cecece",
"8 c #d6d6d6",
"9 c #dddddd",
"0 c #d8e6e6",
"q c #dde9e9",
"w c #e2e2e2",
"e c #ececec",
"r c #eff0f0",
"t c #ffffff",
"y c None",
/* pixels */
"yyyyy% yyyyyyyyyyyyyyy",
"yyyyy 3o.%yyyyyyyyyyyy",
"yyyy%,3+++o.%yyyyyyyyy",
"yyyy 24+t:+++o.%yyyyyy",
"yyyy 4;0tttt:+++o.%yyy",
"yyy%13+tttttttt:++= %y",
"yyy 22;ttttttttt0$=# y",
"yyy 2;ettttttttt$-=$Xy",
"yy%>1OetttttttttX-# yy",
"yy <1X*tttttttt9$=# yy",
"yy <<<<$X*ttttt$-=#Xyy",
"y%=,,<<,<,$@=twX=# yyy",
"y >>@@%,,,,>-%@=*# yyy",
"y --#e<@%->>--==*$Xyyy",
"%*=%<tee8&#$==**# yyyy",
" *=$ee9ww8@%X#%&# yyyy",
"% *@e98765$=>X%&#Xyyyy",
"yy%  2654>#--#%# yyyyy",
"yyyyy%  2#%*@$&# yyyyy",
"yyyyyyyy%  &X%&#Xyyyyy",
"yyyyyyyyyyy%  $ yyyyyy",
"yyyyyyyyyyyyyy %yyyyyy"
};

/* XPM */
static char *editcopy_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 52 1",
"  c #000000",
". c #121212",
"X c #313131",
"o c #3c3b3c",
"O c #474646",
"+ c #4d4d4d",
"@ c #564c45",
"# c #5a5552",
"$ c #5c5b5b",
"% c #786a5c",
"& c #7b6e64",
"* c #7d746e",
"= c #747474",
"- c #7f7975",
"; c #807c79",
": c #998778",
"> c #828282",
", c #898989",
"< c #9f958d",
"1 c #a4a4a4",
"2 c #c9a88c",
"3 c #cbad94",
"4 c #ccb097",
"5 c #ceb49f",
"6 c #c0b5ac",
"7 c #ceb6a0",
"8 c #cfb9a5",
"9 c #c1bab3",
"0 c #d1bdab",
"q c #c4c0bd",
"w c #d3c3b4",
"e c #d5c7ba",
"r c #d6cabf",
"t c #eed3bc",
"y c #c4c4c4",
"u c #c9c9c9",
"i c #d7ccc1",
"p c #d8d0c8",
"a c #dedede",
"s c #efd7c2",
"d c #f1dac7",
"f c #f2decc",
"g c #f3e2d3",
"h c #f5e6d9",
"j c #f6e9dd",
"k c #f4ebe4",
"l c #f8eee5",
"z c #faf2ec",
"x c #fbf6f2",
"c c #fcf8f5",
"v c #ffffff",
"b c None",
/* pixels */
"bbbbbbbbbbbbbbbbbbbbbb",
"b.........bbbbbbbbbbbb",
"b.vvvvvvX,.bbbbbbbbbbb",
"b.vvvvvvovu.bbbbbbbbbb",
"b.v>u>yvOuv,.bbbbbbbbb",
"b.vvvvvvOOoX.bbbbbbbbb",
"b.v>$u>uvy1=......bbbb",
"b.vvvvvcczk9.yvvX,.bbb",
"b.v$>u>#-<*i.yvvovu.bb",
"b.vvvczzljhr.>yvOuv,.b",
"b.v$q;#6:&&w.yvvOOoX.b",
"b.vxzlkhggf0.+>uvy1>.b",
"b.v-#6:&@:%7.yvvvvvy.b",
"b.vljhgfddt3.1>$>1>a.b",
"b.piew085432.yvvvvva.b",
"b..........  >$y1>>a.b",
"bbbbbbbbb.yyyyvvvvva.b",
"bbbbbbbbb.v>$y1>$1>a.b",
"bbbbbbbbb.vvvvvvvvva.b",
"bbbbbbbbb.aaaaaaaaaa.b",
"bbbbbbbbb............b",
"bbbbbbbbbbbbbbbbbbbbbb"
};
/* XPM */
static char *editpaste_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 102 2",
"   c #010101",
".  c #060a06",
"X  c #120e0b",
"o  c #15100c",
"O  c #19130e",
"+  c #121212",
"@  c #1c1c1c",
"#  c #22170f",
"$  c #2d1e11",
"%  c #262206",
"&  c #222216",
"*  c #24241a",
"=  c #342213",
"-  c #362e16",
";  c #3b2614",
":  c #3e3811",
">  c #242424",
",  c #2e2e26",
"<  c #2e2e2e",
"1  c #313131",
"2  c #3a3636",
"3  c #3e3a32",
"4  c #3b3a3b",
"5  c #462c15",
"6  c #5d5100",
"7  c #4d472a",
"8  c #675800",
"9  c #776d14",
"0  c #464545",
"q  c #4c4c4c",
"w  c #564b43",
"e  c #565656",
"r  c #5a5451",
"t  c #5c5b5b",
"y  c #7a6959",
"u  c #656565",
"i  c #6a6666",
"p  c #6d6d6d",
"a  c #7c6d61",
"s  c #7d736c",
"d  c #727272",
"f  c #7e7771",
"g  c #7e7975",
"h  c #7c7c7c",
"j  c #897912",
"k  c #897b1e",
"l  c #9b8574",
"z  c #a79c40",
"x  c #ada35a",
"c  c #b3ad50",
"v  c #beb961",
"b  c #d2cd7c",
"n  c #838383",
"m  c #888885",
"M  c #8c8c8c",
"N  c #969696",
"B  c #9b9c9c",
"V  c #9f9fa3",
"C  c #a0948b",
"Z  c #a2a29e",
"A  c #a5a5a5",
"S  c #ababab",
"D  c #b4b6b4",
"F  c #bebebe",
"G  c #cea685",
"H  c #cfa98a",
"J  c #d0ae91",
"K  c #d2b39a",
"L  c #c1b4aa",
"P  c #c2b9b1",
"I  c #c3bfbc",
"U  c #d3b9a3",
"Y  c #d3bca8",
"T  c #d4ce8f",
"R  c #dcd697",
"E  c #d6c3b2",
"W  c #d7c9bc",
"Q  c #d8cbbf",
"!  c #f2d1b7",
"~  c #f2d5bd",
"^  c #c6c6c6",
"/  c #cacaca",
"(  c #d9cfc6",
")  c #d3d3d3",
"_  c #dbdbdb",
"`  c #f5dac5",
"'  c #f5ddca",
"]  c #f5e0ce",
"[  c #f7e4d4",
"{  c #f7e8da",
"}  c #f8e7d9",
"|  c #f8eade",
" . c #f1f1c6",
".. c #f8f8db",
"X. c #e5e5e5",
"o. c #ededed",
"O. c #f0e9e5",
"+. c #f9ede2",
"@. c #faf2eb",
"#. c #fcf6f1",
"$. c #ffffff",
"%. c None",
/* pixels */
"%.%.%.%.%.%.%.+ 5 9 8 6 %.%.%.%.%.%.%.%.%.%.",
"%.          . % k $.b x -             %.%.%.",
"  o.$.o.o.o.o.6 $... .T z 7 ^ / / ) _ 2 %.%.",
"  $.A A A N k $.R b v c z j 7 n n n M > %.%.",
"  o.A A D A 3 , * * & * & & + u d p g > %.%.",
"  o.B B S F S B n p t u t u t t u p B < %.%.",
"  o.D S S B B N n 5 5 5 ; $ O O @ @ p @ %.%.",
"  o.N B A Z B N h 5 $.$.$.$.$.$.1 m + @ %.%.",
"  X.D A V N m h i 5 $.$.$.$.$.$.4 $./ + %.%.",
"  _ N V V S S N n 5 $.n / n ^ $.0 / $.m + %.",
"  _ B A V M n n p 5 $.$.$.$.$.$.0 0 4 1 + %.",
"  _ M N B A A m d 5 $.n t / n ^ $.^ A d + %.",
"  ) A S A n m m d 5 $.$.$.$.$.#.#.@.O.P + %.",
"  ) h m M B N h i ; $.t n ^ n r f C s ( + %.",
"  / B N m h g d u = $.$.$.#.@.@.+.| [ W + %.",
"  / m M M m n g u $ $.t I g r D l a y E + %.",
"  / h n h h d i t # $.@.@.+.| } [ ' ` U + %.",
"  ^ n m m n n t q O $.g r D l a w l y K + %.",
"  S p d p d u e q X $.| } [ ] ` ` ~ ! J + %.",
"%.              + @ ( Q E E Y U K J H G + %.",
"%.%.%.%.%.%.%.%.%.+ + + + + + + + + +     %.",
"%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%.%."
};

/* XPM */
static char *queue_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 5 1",
"  c #000000",
". c #a0a0a0",
"X c #c0c0ff",
"o c #ffffff",
"O c None",
/* pixels */
"OOOOOOOOOOOOOOOOOOOOOO",
"OOOOOOOOOOOOOOOOOOOOOO",
"OOOOOOOOOOO  OOOOOOOOO",
"OOOOOOOOOO ..  OOOOOOO",
"OOOOOOOO   ....  OOOOO",
"OOOOOOO XX  .....  OOO",
"OOOOO   XXXX  .....  O",
"OOOO oo  XXXXX  ..... ",
"OOOO oooo  XXXXX  ... ",
"OOO ooooooo  XXXXX . O",
"OOO ooooooooo  XXX . O",
"OO oooooooooooo X . OO",
"OO oooooooooooo X . OO",
"O oooooooooooo X . OOO",
"O oooooooooooo X . OOO",
" oooooooooooo X . OOOO",
" oooooooooooo X . OOOO",
"O  ooooooooo X   OOOOO",
"OOO  ooooooo X OOOOOOO",
"OOOOO  oooo   OOOOOOOO",
"OOOOOOO  oo OOOOOOOOOO",
"OOOOOOOOO  OOOOOOOOOOO"
};

/* XPM */
static char *undo_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 55 1",
"  c #000000",
". c #060b03",
"X c #071003",
"o c #081204",
"O c #0b1b05",
"+ c #0f1c09",
"@ c #0e2005",
"# c #13240b",
"$ c #112a06",
"% c #173807",
"& c #232d1d",
"* c #1c4408",
"= c #234413",
"- c #275f0b",
"; c #2b6a0e",
": c #35691b",
"> c #3a8816",
", c #3f8321",
"< c #34a814",
"1 c #3fa21c",
"2 c #38aa18",
"3 c #41821f",
"4 c #478824",
"5 c #469d24",
"6 c #529033",
"7 c #5b923c",
"8 c #40a513",
"9 c #42a01e",
"0 c #47ab29",
"q c #4aa721",
"w c #4ab427",
"e c #53b529",
"r c #51b929",
"t c #79b658",
"y c #79be53",
"u c #5bc930",
"i c #6fd63d",
"p c #72c24f",
"a c #79d54b",
"s c #7fcc67",
"d c #88be71",
"f c #87c86a",
"g c #8ac963",
"h c #91d56b",
"j c #9ac589",
"k c #a7c49a",
"l c #c7e0ba",
"z c #c8e1be",
"x c #cde3c1",
"c c #d4e4cb",
"v c #e5eedf",
"b c #ecf3e8",
"n c #f5f9f4",
"m c #ffffff",
"M c None",
/* pixels */
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMM-----MMMMMM",
"MMMMMMMMM--yyyyy--MMMM",
"MMMMMMM--yyyyyyyyy-MMM",
"M-MMMM-yyyqq1111yyy MM",
"M--MM-yyy1uiiiiuyyyy M",
"M-y--yy1uie    ;:yyy M",
"M-yyyy9ie $MMMM.@;yyy*",
"M-yyyyie MMMMMMMMO*yy*",
"M-yyyy5 MMMMMMMMMM$>y*",
"M-yyyya*MMMMMMMMMMo>*M",
"M-yyyy46*MMMMMMMMM*MMM",
"M-        MMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM",
"MMMMMMMMMMMMMMMMMMMMMM"
};

/* XPM */
static char *redo_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 56 1",
"  c #000000",
". c #060b03",
"X c #071003",
"o c #081204",
"O c #0b1b05",
"+ c #0f1c09",
"@ c #0e2005",
"# c #13240b",
"$ c #112a06",
"% c #173807",
"& c #232d1d",
"* c #1c4408",
"= c #234413",
"- c #275f0b",
"; c #2b6a0e",
": c #35691b",
"> c #3a8816",
", c #3f9524",
"< c #3fa21c",
"1 c #38aa18",
"2 c #41821f",
"3 c #428626",
"4 c #4a8d2d",
"5 c #469d24",
"6 c #50932f",
"7 c #5b973a",
"8 c #40a513",
"9 c #42a01e",
"0 c #41ae20",
"q c #47ab29",
"w c #4aa721",
"e c #48b227",
"r c #53a82f",
"t c #53b529",
"y c #51b929",
"u c #67a050",
"i c #79b658",
"p c #79be53",
"a c #7faa67",
"s c #5bc930",
"d c #63c53e",
"f c #6fd63d",
"g c #72c24f",
"h c #7fcc67",
"j c #87c86a",
"k c #8ac963",
"l c #91d56b",
"z c #9ac589",
"x c #a7c49a",
"c c #c7e0ba",
"v c #c8e1be",
"b c #cde2c1",
"n c #e4eddf",
"m c #f5f9f4",
"M c #ffffff",
"N c None",
/* pixels */
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNN-----NNNNNNNNNNN",
"NNNN--ppppp--NNNNNNNNN",
"NNN-ppppppppp**NNNNNNN",
"NN-ppp<<<<wpppp NNNN N",
"N-zl<tsffffs<ppp NN* N",
"N-p<t:;    tfsppp *p N",
"*pi3;@.NNNN$ tf9kppp N",
"*p>*ONNNNNNNN tf9ppp N",
"*p-$NNNNNNNNNN pyppp N",
"N*<oNNNNNNNNNN*pdppp N",
"NNNoNNNNNNNNN*p7pppp N",
"NNNNNNNNNNNN         N",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN",
"NNNNNNNNNNNNNNNNNNNNNN"
};



/* XPM */
static char * left_20_xpm[] = {
"20 20 31 1",
" 	c None",
".	c #4A8423",
"+	c #9FC487",
"@	c #419907",
"#	c #53B257",
"$	c #B1F981",
"%	c #EAF9E0",
"&	c #B4FF83",
"*	c #36963E",
"=	c #278733",
"-	c #BDEA9F",
";	c #A4FA6A",
">	c #106D20",
",	c #4E991D",
"'	c #91DB60",
")	c #9FE570",
"!	c #97EF5D",
"~	c #93F453",
"{	c #9CF263",
"]	c #7CE537",
"^	c #1A7A29",
"/	c #41C229",
"(	c #6DAD43",
"_	c #76B54C",
":	c #65BA2C",
"<	c #B6E298",
"[	c #83AE7B",
"}	c #57AA20",
"|	c #6BC62D",
"1	c #5EB227",
"2	c #3B7A11",
"                    ",
"                    ",
"                .   ",
"              +@#   ",
"            +@@$#+  ",
"          +@@%$&*+  ",
"        +@@%$&&&*+  ",
"      +@@%$&&$&$=+  ",
"    +@@%$&&&$-;;=+  ",
"   >,')!)~{;);]!^+  ",
"   >>^=*/#/(__::^+  ",
"    <>>^=*#(:[(}^+  ",
"      <>>^=*#(|1^+  ",
"        <>>^=*##>+  ",
"          <>>^=#>+  ",
"            <>>^>+  ",
"              <>>+  ",
"               +2+  ",
"                    ",
"                    "};


/* XPM */
static char * right_20_xpm[] = {
"20 20 26 1",
" 	c None",
".	c #53B257",
"+	c #419907",
"@	c #9FC487",
"#	c #CFF7B4",
"$	c #36963E",
"%	c #B4FF83",
"&	c #EAF9E0",
"*	c #278733",
"=	c #97EF5D",
"-	c #D1EFBD",
";	c #A4FA6A",
">	c #BDEA9F",
",	c #1A7A29",
"'	c #93F453",
")	c #4E991D",
"!	c #106D20",
"~	c #92C172",
"{	c #83AA7B",
"]	c #41C229",
"^	c #57AA20",
"/	c #83AE7B",
"(	c #83A57B",
"_	c #B6E298",
":	c #5EB227",
"<	c #83A573",
"                    ",
"                    ",
"                    ",
"   .+@              ",
"  @.#++@            ",
"  @$%#&++@          ",
"  @$%%%#&++@        ",
"  @*=%-%%#&++@      ",
"  @*;;>-%%%#&++@    ",
"  @,==;;;-''=#&)!@  ",
"  @,)~{~{].]$*,!!   ",
"  @,^{/](.$*,!!_    ",
"  @,:]<.$*,!!_      ",
"  @!..$*,!!_        ",
"  @!.*,!!_          ",
"  @!,!!_            ",
"  @!!_              ",
"  @@@               ",
"                    ",
"                    "};

/* XPM */
static char * up_xpm[] = {
"20 20 29 1",
" 	c None",
".	c #106D20",
"+	c #B6E298",
"@	c #4E991D",
"#	c #9FC487",
"$	c #1A7A29",
"%	c #91DB60",
"&	c #419907",
"*	c #278733",
"=	c #9FE570",
"-	c #36963E",
";	c #97EF5D",
">	c #EAF9E0",
",	c #41C229",
"'	c #B1F981",
")	c #53B257",
"!	c #93F453",
"~	c #B4FF83",
"{	c #9CF263",
"]	c #6DAD43",
"^	c #A4FA6A",
"/	c #65BA2C",
"(	c #76B54C",
"_	c #83AE7B",
":	c #BDEA9F",
"<	c #6BC62D",
"[	c #7CE537",
"}	c #5EB227",
"|	c #57AA20",
"                    ",
"                    ",
"                    ",
"          ..        ",
"         +.@#       ",
"         .$%&       ",
"        +.*=&#      ",
"        .$-;>&      ",
"       +.*,='&#     ",
"       .$-)!~>&     ",
"      +.*),{~'&#    ",
"      .$-]]^~~>&    ",
"     +.*)/(='~'&#   ",
"     .$-]_(^:'~>&   ",
"    +.*)<]/[^~~'&#  ",
"   #.$))}|/;^'~~'&  ",
"   ##############   ",
"                    ",
"                    ",
"                    "};

/* XPM */
static char * down_xpm[] = {
"20 20 30 1",
" 	c None",
".	c #9FC487",
"+	c #4A8423",
"@	c #53B257",
"#	c #36963E",
"$	c #278733",
"%	c #1A7A29",
"&	c #106D20",
"*	c #3B7A11",
"=	c #419907",
"-	c #EAF9E0",
";	c #B1F981",
">	c #B4FF83",
",	c #97EF5D",
"'	c #65BA2C",
")	c #57AA20",
"!	c #5EB227",
"~	c #7CE537",
"{	c #6DAD43",
"]	c #6BC62D",
"^	c #B6E298",
"/	c #A4FA6A",
"(	c #76B54C",
"_	c #83AE7B",
":	c #9FE570",
"<	c #9CF263",
"[	c #41C229",
"}	c #93F453",
"|	c #91DB60",
"1	c #4E991D",
"                    ",
"                    ",
"                    ",
"  ................  ",
"   +@@##$%%%%&&&&*  ",
"   =-;>>;,')!@@%&.  ",
"   .=>;>>~'{]@$&^   ",
"    =->>;/(_{#%&    ",
"    .=>;>:('@$&^    ",
"     =->>/{{#%&     ",
"     .=>;<[@$&^     ",
"      =->}@#%&      ",
"      .=;:[$&^      ",
"       =-,#%&       ",
"       .=:$&^       ",
"        =|%&        ",
"        .1&^        ",
"         &&         ",
"                    ",
"                    "};

static char * ok2_xpm[] = {
"14 14 11 1",
" 	c None",
".	c #008400",
"+	c #008200",
"@	c #008D00",
"#	c #67C467",
"$	c #008C00",
"%	c #93E693",
"&	c #008500",
"*	c #008600",
"=	c #008700",
"-	c #008300",
"                ",
" +$%      ++%   ",
" ++$%     ++$%  ",
"  +$%    ++$#%  ",
"  &$%   ++$#%   ",
"  .$%  ++$#%    ",
"  *$% $++#%     ",
"  +$%$++#%      ",
"  +$=++#%       ",
"  +-*$#%        ",
"  +++#%         ",
"  ++#%          ",
"  +#%           ",
"                "
};


/* XPM */
static char * exit_xpm[] = {
"14 14 15 1",
" 	c None",
".	c #FF0000",
"+	c #FF5252",
"@	c #B52323",
"#	c #FF7676",
"$	c #FF5656",
"%	c #FF3636",
"&	c #FF6666",
"*	c #FF0404",
"=	c #FF3A3A",
"-	c #FF0101",
";	c #FF5F5F",
">	c #FF0202",
",	c #FF0B0B",
"'	c #FF1C1C",
"              ",
"@..#       @..",
" @..$     @%.&",
"  @..&   @..& ",
"   @..& @.%+  ",
"    @.*...+   ",
"     @...+    ",
"     +@.@     ",
"    +&>..@    ",
"   +.,@+..@   ",
"  +.,@  +.*@  ",
" +.,@    +'.@ ",
"+.,@      +..@",
"              "};

/* XPM */
char *rec_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 21 1",
"  c #394248",
". c gray29",
"X c gray44",
"o c #1c60ab",
"O c #929292",
"+ c #9b9b9b",
"@ c #a3a3a3",
"# c #b1b0b0",
"$ c #bababa",
"% c #c0c0c0",
"& c #cbcbcb",
"* c #d0cfd0",
"= c #d3d4d3",
"- c #dbdbdb",
"; c #e2e2e2",
": c #e7e7e8",
"> c #ebebeb",
", c #f2f3f3",
"< c #f8f7f8",
"1 c #fdfdfd",
"2 c None",
/* pixels */
"2222222222222222222222",
"2222222222222222222222",
"2222222222222222222222",
"222                222",
"222 oooooooooooooo 222",
"222 oooooooooooooo 222",
"222                222",
"222 $$$$$$$%%%&*=- 222",
"222 $$$$$$%%&*=--; 222",
"222 $$$$%%&&==--;> 222",
"222 $$%#O+O%--;>>, 222",
"222 $%%O+XX+;:>,<< 222",
"222 %&*+X1X.*@+@;1 222",
"222 *==+XX.@@+XX@1 222",
"222 =--&+.@=+X1X.1 222",
"222 -;:>,,<1@XX.@1 222",
"222 ;>,,<111;@.@;1 222",
"222 >,<11111111111 222",
"222                222",
"2222222222222222222222",
"2222222222222222222222",
"2222222222222222222222"
};

/* XPM */
char *run_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 35 1",
"  c #000001",
". c #070709",
"X c #090909",
"o c #101012",
"O c #1b1b1c",
"+ c #222224",
"@ c #2b2b2c",
"# c #393939",
"$ c #5a5a5a",
"% c #656566",
"& c #686869",
"* c #747474",
"= c #7c7c7c",
"- c #858585",
"; c #878788",
": c #898987",
"> c #8d8d8e",
", c #959594",
"< c #9c9c9c",
"1 c #a5a5a5",
"2 c #adadac",
"3 c #b3b3b3",
"4 c #b8b8b7",
"5 c #bdbdbc",
"6 c #c5c5c4",
"7 c #cbcbcc",
"8 c #d1d1cf",
"9 c #d4d4d4",
"0 c #dbdbda",
"q c #dfdfe0",
"w c #e3e3e3",
"e c #e9e9e7",
"r c #f0f0ee",
"t c #f2f2f0",
"y c None",
/* pixels */
"yyyyyyyyyyyyyyyyyyyyyy",
"yyyyyyyyyy  yyyyyyyyyy",
"yyyyyyyyy r7 yyyyyyyyy",
"yyyy  yyy t3.yyy. yyyy",
"yyy 0t y  r>  y w6 yyy",
"yyy tqe tr00w6 e6: yyy",
"yyyy eqqw0999807$.yyyy",
"yyyyy qq00987765 yyyyy",
"yyyy tq09,&&3665: yyyy",
"yy   t09,# X>7651   yy",
"y rrt099% yyXw553<33 y",
"y 73>099% yyor542>=* y",
"yy   w972>Xor043>  .yy",
"yyyy 69767wr0332$ yyyy",
"yyyyy 966655433< yyyyy",
"yyyy w66555432<3, yyyy",
"yyy e6$ :132>$ <2= yyy",
"yyy 6: y  &>O y =* yyy",
"yyyy. yyy+2- yyy  yyyy",
"yyyyyyyyy@6: yyyyyyyyy",
"yyyyyyyyyy +yyyyyyyyyy",
"yyyyyyyyyyyyyyyyyyyyyy"
};
#if	GTK3
static char * exact_xpm[] = {
"18 18 29 1",
" 	c None",
".	c #106D20",
"+	c #B6E298",
"@	c #4E991D",
"#	c #9FC487",
"$	c #1A7A29",
"%	c #91DB60",
"&	c #419907",
"*	c #278733",
"=	c #9FE570",
"-	c #36963E",
";	c #97EF5D",
">	c #EAF9E0",
",	c #41C229",
"'	c #B1F981",
")	c #53B257",
"!	c #93F453",
"~	c #B4FF83",
"{	c #9CF263",
"]	c #6DAD43",
"^	c #A4FA6A",
"/	c #65BA2C",
"(	c #76B54C",
"_	c #83AE7B",
":	c #BDEA9F",
"<	c #6BC62D",
"[	c #7CE537",
"}	c #5EB227",
"|	c #57AA20",
"                  ",
"                  ",
"                  ",
"                  ",
"            ###   ",
"           #   #  ",
"          #     # ",
"          #     # ",
"          #     # ",
"  #####   #     # ",
" #[   ]#  #     # ",
" #     #  ####### ",
" #     #  #     # ",
" #     #  #     # ",
" #    ##  #     # ",
"  #### ## #     # ",
"                  ",
"                  "};

static char * regexp_xpm[] = {
"18 18 29 1",
" 	c None",
".	c #106D20",
"+	c #B6E298",
"@	c #4E991D",
"#	c #9FC487",
"$	c #1A7A29",
"%	c #91DB60",
"&	c #419907",
"*	c #278733",
"=	c #9FE570",
"-	c #36963E",
";	c #97EF5D",
">	c #EAF9E0",
",	c #41C229",
"'	c #B1F981",
")	c #53B257",
"!	c #93F453",
"~	c #B4FF83",
"{	c #9CF263",
"]	c #6DAD43",
"^	c #A4FA6A",
"/	c #65BA2C",
"(	c #76B54C",
"_	c #83AE7B",
":	c #BDEA9F",
"<	c #6BC62D",
"[	c #7CE537",
"}	c #5EB227",
"|	c #57AA20",
"                  ",
"                  ",
"                  ",
"                  ",
"              #   ",
"              #   ",
"              #   ",
"              #   ",
"              #   ",
"     #        #   ",
"   # # #      #   ",
"    ###       #   ",
"  #######     #   ",
"    ###       #   ",
"   # # #  ##      ",
"     #    ##  #   ",
"                  ",
"                  "};
#endif

#if	NUSE
static char *editcut_xpm[] = {
/* columns rows colors chars-per-pixel */
"22 22 10 1",
"  c #000000",
". c #080203",
"X c #303030",
"o c #6a6a6a",
"O c #9b9b9b",
"+ c #a0a0a0",
"@ c #e2e2e2",
"# c #e8e8e8",
"$ c #ffffff",
"% c None",
/* pixels */
"%%%%%%%%%%%%%%%%%%%%%%",
"%%%%%%%%%%%%%%%%%%%%%%",
"%%  . %%%%%%%%%%%%%%%%",
"%   %  %%%%%%%%%%   %%",
"%  %%%  %%%%%%%% $#O %",
"%  %%%%  %%%%%% $#O %%",
"%%  %%%% %%%%% $#O %%%",
"%%%   %  %%%% $#O %%%%",
"%%%%%     %% $#O %%%%%",
"%%%%%%%     $#O %%%%%%",
"%%%%%%%%%+ $#O %%%%%%%",
"%%%%%%%%% $XO %%%%%%%%",
"%%%%%%%    $ o %%%%%%%",
"%%%%%       $@O %%%%%%",
"%%%   %  %%% $@O %%%%%",
"%%  %%%% %%%% $@O %%%%",
"%  %%%%  %%%%% $@O %%%",
"%  %%%  %%%%%%% $@O %%",
"%  %%  %%%%%%%%% $@O %",
"%%  . %%%%%%%%%%%   %%",
"%%%%%%%%%%%%%%%%%%%%%%",
"%%%%%%%%%%%%%%%%%%%%%%"
};
#endif

#if	NUSE
/* XPM */
static char * cancel2_xpm[] = {
"18 18 15 1",
" 	c None",
".	c #FF0000",
"+	c #FF5252",
"@	c #B52323",
"#	c #FF7676",
"$	c #FF5656",
"%	c #FF3636",
"&	c #FF6666",
"*	c #FF0404",
"=	c #FF3A3A",
"-	c #FF0101",
";	c #FF5F5F",
">	c #FF0202",
",	c #FF0B0B",
"'	c #FF1C1C",
"                  ",
"  ..+          @. ",
"  @..#        @.. ",
"   @..$      @%.& ",
"    @..&    @..&  ",
"     @..&  @.%+   ",
"      @.*#...+    ",
"      @.....+     ",
"      =....+      ",
"      -...;       ",
"    &..&>.@       ",
"   &,@  +..@      ",
"  &.@    +.*@     ",
" &.@      +'.@    ",
" ,.@       +..@@  ",
" ,@         ++..  ",
"                  ",
"                  "};



#endif
