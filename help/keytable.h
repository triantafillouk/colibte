/* Not yet bounded keys
	^X(Q, E, Y, A, H, K, 456789, -, +)
	^X({,}, ;, :, ', ", ,, .,<,>,/, ?, \, |)


	ESC( Y, ], ;,  ", /, ?, ~) (do not allow shifted characters)
	ESC-^(Q,W,T, U, I, O, P, A, S, E, D, F, G, J, L,Z,V,B,N, M)
*/
/*
 Other keys
!!	^XG			update tagx file
!!	M-.			-> find_tag (jed, emacs M-.)
!!	M-'			-> add/delete c++ comment
!!	M-;			-> add/edit comment at end of line (jed)
!!	M-/			-> dabbrev (abbreviation jed)
!!				-> set bookmark
!!				-> goto bookmark
				-> mark function
*/

#if	COMMENT
// keymap editor compatibility !
# 		 native					brief					gedit				kate		emacs
 ALT-A = user menu file
 ALT-B = 						buffer list				??					??
 ALT-C = column mark			same					??					??
 ALT-D = delete next word		delete line				??					??
 ALT-E = edit menu				edit file				??					??
 ALT-I = insert tab				insert/overwrite mode	??					??
 ALT-K = assign key				kill to the end of line	??					??
 ALT-L = line mark 				same					??					??			region lower
 ALT-M = menu mode				mark/unmark				??					??
 ALT-G = goto line				same
 ALT-H = menu help				help					menu help			menu help
 ALT-N = next buffer			cannot  (M-N is used bu Unixware) can be done!
 ALT-P = previous hmark
 ALT-N = next mark				this is a special character on SCO
 ALT-Q = fill paragraph									quit				quit
 ALT-R = replace string
 ALT-T = tools menu				replace
 ALT-S = search menu
 ALT-- = sub test1				previous buffer
 ALT-U = word upper
 ALT-V = 
 ALT-W = window menu									quit				quit
 ALT-X = quit																			quit
 ALT-Y = 
 ALT-Z = redo
 
 F1 = show info		
 F2 = open file
 F3 = quick close
 F4 = describe key
 F5 = find next
 F6 = find previous
 F7 = macro on/off				YES
 F8 = play macro				YES
 F9 = next window
 F10 = menu
 F11 = toggle_sync_lock
 F12 = toggle move lock

 grey+ = copy to scrap			->yes? can be done!
 INS   = insert scrap				OK
 Alt-F5 = backward search		cannot 
 alt-f6 = backward replace		cannot

 ctrl-F5 = case sensitivity toggle
 ctrl-F6 = regular expression toggle

#       native                        	gedit               kate            	emacs
 ^A =   bol                            	select all          select all      	bol
 ^B =   select file                    	-                                   	back character
 ^C =   copy                           	copy                copy
 ^D =   delete line                    	delete line         -               	delete character
 ^E =   eol                            	-                   -               	eol
 ^F =   next char                      	find                find            	next char
 ^G =   abort                          	find next           goto line       	abort
S^G =									find previous
 ^H =   del_prev_char                  	replace             mark & find     	help or del_prev_char
 ^I =   tab                            	goto line           -               	tab
 ^J =   insert_ident                   	-                   -               	insert ident
 ^K =   erase_eol                      	incremental search  -               	erase_eol
 ^L =   redraw disp                    	open location       save all        	redraw disp
S^L =                                                      split vertical
 ^M =   new line                       	-                   -           
 ^N =   next line                      	new file            new file        	next line
 ^O =   open file                      	open file           open file       	open line
 ^P =   both_down                      	print               print           	previous line
 ^Q =   quote character                	quit                quit            	>>
 ^R =   search_reverse                 	-                   replace         	>>
S^R =                                  	                    close window
 ^S =   search                         	save                save            	i-search
 ^T =   twidle                         	-                   -               	twidle
S^T =                                                      split horizontal
 ^U =   both up                        	-                   -               	universal argument
 ^V =   past                           	paste               paste           	next page
 ^W =   cut_region                     	close               close           	cut_region
 ^X =   start ^X                       	cut_region          cut_region      	start ^X
 ^Y =                                  	-                   -               	yank
 ^Z =   undo                           	undo                undo				scroll up
S^Z = 									redo				redo
 ^Space	mark on/off						-					code completion		mark on/off
 ^_ = 																			undo
----------------------------------------------------------------------------------------------------
# 		native          		brief					gedit				kate		emacs
 ^X^@
 ^X^A =	save session*
 ^x^b = load session*
 ^x^c = quit																			save quit
 ^x^d = detab line*M																	dir list
 ^x^e = entab line*M																	eval-last-sexp
 ^x^f = select font*M																	find file
 ^x^g = 
 ^x^h =																					key help
 ^x^i = remove tab																		ident-rigidly
 ^x^j = 
 ^x^K = close file	*M																	kmacro-keymap
 ^x^l = region lower*M																	downcase-region
 ^X^M = toggle move lock*F12															prefix-command
 ^X^N = 																				set goal column
 ^X^O = del blank lines
 ^X^P = 																				mark page (bof?)
 ^X^Q = 																				toggle read only
 ^X^R = replace next																	find file read only
 ^X^S = save file																		save buffer
 ^X^T = trim line																		transpose lines
 ^X^U = region upper																	upcase region
 ^X^V = show session																	find alternate file
 ^X^W = save as																			write file
 ^X^X = swap mark																		swap mark
 ^X^Y = 
 ^X^Z = 																				iconify-deiconify-frame


# 		native         		brief					gedit				kate		emacs
 ^X$																					set selective display
 ^X''																					expand abbrev
 ^X(																					start macro
 ^X)																					end macro
 ^X*																					calc-dispatch
 ^X+																					balance windows
 ^X-																					shrink window
 ^X0	delete window																	delete window
 ^X1	one window																		one window
 ^X2	split window vertically															>>
 ^X3	split window horizontally														>>
 ^X;																					comment-set-column
 ^X<																					scroll left
 ^X=																					show cursor position
 ^X>																					scroll right
 ^X[																					backward page
 ^X]																					forward page
 ^X^																					enlarge window
 ^X`																					next error
 ^XA 	
 ^XB	toggle bold																		switch to buffer
 ^XC
 ^XD	 list dir																		dired
 ^XE																					kmacrot-end-and-call-macro
 ^XF																					set fill column
 ^XH																					mark-whole-buffer
 ^XG	update tags
 ^XH
 ^XI	toggle over																		insert file
 ^XK	close file																		kill buffer
 ^XL	toggle sync lock                       											count lines page
 ^XM																					compose mail
 ^XN	rename file																		rename file
 ^XO																					other window
 ^XP	previous window
 ^XQ																					kbd-macro-query
 ^XR	resize window(text mode)
 ^XS	set option																		save-some-buffers
 ^XT	set bmark
 ^XU																					advertised-undo
 ^XV
 ^XW
 ^XX
 ^XY
 ^XZ																					repeat
 ^X{																					shrink window horizontally
 ^X}																					enlarge window horizontally
 ^XDEL																					backward delete

 
#endif

/* -- */
