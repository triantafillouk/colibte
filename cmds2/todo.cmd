# Test internal functions

#bind_function ("c_comment", "M-;")
#bind_function ("w_abbrev", "M-/")

function w_abbrev1()
{
	prev_word(1)
	pw1 = _next_word
	message(pw1)
}

function w_abb1()
{
 set_mark()
 prev_word(1)
#get the current word
 cw1 = _next_word
 cw1	:
 i = 0
 while (i<20) {
 s=prev_word(1)
 s:
 if(s==0){
	break
	swap_mark()
	set_mark()
	break
 }

 p1 = _next_word
 p1	:
 j=s_index(p1,cw1)
 if(j==1){
	swap_mark()
 	set_mark()
 	del_prev_word(1)
	insert (p1)
	i=20
	break
 }
 i = i+1
 }
}

# comment / uncomment (C++ like)
function c_comment1()
{
 bol()
 t = _text
 t1 = s_index(t,"//")
 if(t1==0) {
	bol() 
	insert("//")
 }
 if(t1==1) {
		bol()
		del_next_char(2)
 }
 if(t1>1) {
	bol()
	next_character(t1-1)
	del_next_char(2)
 }
}

print("functions added!")