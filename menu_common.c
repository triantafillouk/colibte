/*
	common functions for menu support
*/
extern MENUS *start_menu;

int work_menu(int n)
{
 FILEBUF *fp=cbfp;
	if(fp->b_flag & FSDIRED && fp->b_flag & FSNLIST) {
		if(fp->b_flag & FSNLIST) {
			start_menu = &m_sort;	/* or a dir menu !!!!  */
		} else {
			if (cwp->selection) start_menu = &m_select_on;
			else start_menu = &m_select_off;
		};
	}
#if	TNOTES
	else if(fp->b_flag & FSNOTES) {
	 	start_menu=&m_notes_tag;
	} else if(fp->b_flag & FSNLIST) {
	 	start_menu=&m_notes_tag;
	} 
#endif
	else {	/* normal file  */
		if (cwp->selection) start_menu = &m_select_on;
		else start_menu = &m_select_off;
	};

 return execute_menu(-1);
}
