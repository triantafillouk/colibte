# Initialize editor commands
assign_sub ("c_comment", "M-;")
assign_sub ("cp_comment", "M-'")
assign_sub ("cp_comment", "M-`")
assign_sub ("show_out","M-,")
assign_sub ("w_abbrev", "M-/")
assign_sub ("test1", "^XX")
assign_sub ("clear_results", "^XC")

color_scheme_read(2)
color_scheme(2)
#select_font("5x7")
#set_fontindex(2)
#font="misc fixed"
#font="monospace"
font="andale mono 14"
# font="9x16"
#_backup_extention=".bak"
# print("font set ok!")
#
set_locale(7)

function p_word()
{
	prev_word(1)
	pw1 = _next_word
#	message(pw1)
}

function show_out()
{
 if(!in_view("[out]")) {
	vsplit_window()
	next_window()
	select_file("[out]")
	prev_window()
 }
}

function w_abbrev()
{
# goto end of the word and set cposition
 goto_eow(0)
 cposition = _position
 prev_word(1)
#get the current word
 cw1 = _next_word
# print("w_abbrev: cw1="+cw1)
 i = 0
 pind=0
# check the previous 100 words
 while(i<1000)
 {
 s=prev_word(1)
# print("s = "+s)
 if(s < 1 )
 {
	break
 }

 p1 = _next_word
# print("previous p1="+p1)
 j=s_index(p1,cw1)
 if(j==1)
 {
#	print("old position is")
#	print(cposition)
#	print("found position is:")
#	print(_position)
	_position = cposition
#	print("new/old position is:")
#	print(_position)
 	del_prev_word(1)
	insert (p1)
	i=1000
	break
 }
 i = i+1
 pind=pind+1
 }

 if(j==0)
 {
	p1="no word found starting with '"+cw1+"'"
	_position = cposition
 }
 p1=p1+" "+pind+" words back"
 message(p1)
}

#
# make it a comment or make it real
function cp_comment()
{
# print("cp_comment")
 bol()
 t = _text
# print(t)
 t1 = s_index(t,"//")
 if(t1==0) 
 {
	bol() 
	insert("//")
 }
 if(t1==1)
 {
		bol()
#	2 @	del_char
#	print("del_char 2")
	del_char(2)
#	print("ok!")
 }
 if(t1>1)
 {
	bol()
	next_character(t1-1)
	del_char(2)
 }
}

# make it a comment or make it real
function c_comment()
{
 bol()
 t = _text
 t1 = s_index(t,"/*")
 if(t1==0) 
 {
	eol ()
	insert("	/*   */")
 }
 bol()
 t = _text
 t1 = s_index(t,"/*")
# _column = t1+2
 next_character(t1+2)
}

function test1()
{
 print("test1")
 print("Version:"+_version)
# print(_version)
 print("file name:"+_file_name)
 print("current_char is: ["+_char+"]")
 print("Search pattern:"+_search_pattern)
 print("Replace pattern:"+_replace_pattern)
 print("Date:"+_time)
 print("Match:"+_regexp_match)
 print("column = "+_column)
 print("line = "+_line)
 print("position ="+_position)
 print("base name is "+_base_name)
 print("Key:"+_key)
 print("text:"+_text)
 print("window_rows:"+_window_rows)
 print("window cols:"+_window_cols)
 print("found:"+_found)
 print("next word:"+_next_word)
}

#clear results
function clear_results()
{
line1=_line+1
bof
test=1
found=0
while(test)
{
 test = search_forward(":")
 if(test)
 {
#	print(_text)	
	erase_eol()
	found=found+1
 }
 else
 {
	message ("cleared "+found+" results!)")
	goto_line(line1)
	unmark_buffer()
	break
 }
}
bof
}
