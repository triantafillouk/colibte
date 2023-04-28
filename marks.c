/*
	Curses,gtk editor,notes,directory browser
	Copyright Kostas Triantafillou
	GNU LESSER GENERAL PUBLIC LICENSE version 2 
	(or at your option) any later version.

*/
/* bookmarks, history marks support */

#include	"xe.h"

 BMARK bmark[BKMAX];	/* bookmarks are window independent  */
 BMARK hmark[BKMAX];	/* history marks are automatic created */

// These are bookmarks
/*
	set_bmark
	select_bmark
	next_bmark
	prev_bmark
*/
 static int bknum=0;
 static int ctag=0;
 static int ntag=0;
 static int ptag=0;
/* history variables */
 int hmnum=-1;	/* number of history marks */
 int hmstart=0;	/* history marks start mark in the circular array */
 int hmcurrent=-1; /* history mark current */

 FILEBUF *get_dir_buffer(int flag,int dir_num);
 char *dir_name(int dtype);
 extern FILEBUF *cbfp;

/* set a book mark 
 Assigned to ^XT
*/
int set_bmark(int n)
{
 bmark[bknum].line=getcline()+1;
 bmark[bknum].column=getccolumn()+1;
 strlcpy(bmark[bknum].fname,cbfp->b_fname,MAXFLEN);
 strlcpy(bmark[bknum].dname,cbfp->b_dname,MAXFLEN);
 ptag=ctag;
 ctag=bknum;
 msg_line("set bookmark %d ok!",bknum);
 bknum++;
 if(bknum>BKMAX-1) bknum=1;
 return(TRUE);
}

int goto_mark(BMARK *mark)
{
 char file_name[MAXFLEN];
	if(mark->fname[0]=='/') strlcpy(file_name,mark->fname,MAXFLEN);
	else { 
		if(strncmp("[D",mark->fname,2)==0) {
			int dir_num=mark->fname[2]-'0';
			int err;
			FILEBUF *bp;
			bp=get_dir_buffer(0,dir_num);
			if(bp==NULL || bp->dir_num != dir_num) {
				bp=new_filebuf(dir_name(dir_num),FSDIRED);
				bp->dir_num=dir_num;
			};
			strlcpy(bp->b_dname,mark->dname,sizeof(bp->b_dname));
			err=insert_dir(bp,0);
			select_filebuf(bp);
			igotolinecol(mark->line,mark->column,0);
//			cwp->tp_current->col=mark->column;
			return err;
		}
	 	strlcpy(file_name,mark->dname,MAXFLEN);
		strlcat(file_name,"/",MAXFLEN);
		strlcat(file_name,mark->fname,MAXFLEN);
	};
	goto_file(file_name);
//	MESG("goto_mark: line=%ld col=%ld",mark->line,mark->column);

 	igotolinecol(mark->line,mark->column,0);
//	cwp->tp_current->col=mark->column;
	return(1);
}

int goto_bmark(int n)
{
 if(n>=0 && n<bknum) {
 	goto_mark(&bmark[n]);
	set_hmark(1,"goto_bmark");
	if(ctag!=n) {
		ptag=ctag;
		ctag=n;
	};
	return TRUE;
 } else { 
	msg_line("");
	return FALSE;
 };
}


#define BMARKLEN	80
/* select a bookmark from keyboard and goto it */
int select_bmark(int n)
{
 char **mark_list;
 int ind;
 int i;
 char value[MAXMLEN];
 mark_list=(char **)malloc(sizeof(char *)*(bknum+2));
 for(i=0;i<bknum;i++){
	snprintf(value,BMARKLEN+8,"%-*s :%5ld",BMARKLEN,get_pfname(bmark[i].dname,bmark[i].fname,BMARKLEN),bmark[i].line);
	mark_list[i]=strdup(value);
 };
 mark_list[i]=0;
 ind=selectl("select bookmark",mark_list,bknum,10,5,1,BMARKLEN+7,-1);
 sarray_clear(mark_list);
 if(ind<0) return 0;
 return goto_bmark(ind);
}


int prev_bmark(int n)
{
 int t;
 t=ptag;
 ptag=ctag;
 goto_bmark(t);

 return TRUE;
}

/* next_bmark: FIXME!! */
int next_bmark(int n)
{
 ptag=ctag;
 ctag++;
 if(ctag>=bknum) ctag=0;
  goto_bmark(ctag);
  msg_line("Bookmark %d",ctag);
 return TRUE;
}

/* clear all bookmarks */
int clear_bmarks(int n) 
{
 int i;
 for(i=0;i<BKMAX;i++) {
	bmark[i].line = 0l;
	bmark[i].column = 0l;
 };
 bknum=0;
 ctag=0;
 ntag=0;
 ptag=0;
 return TRUE;
}


/* Bookmark functions  */

/* set  history mark at current position */
int set_hmark(int flag,char *from)
{
 int current_ind;
 if(!strcmp("[tagx.tags]",cbfp->b_fname)) return(FALSE);
 if(macro_exec) return false;

 if(flag) {
 	if(hmcurrent<BKMAX-1) hmcurrent++;
	else {
		hmstart++;
		if(hmstart>BKMAX-1) hmstart=0;
	};
 };

 current_ind=(hmstart+hmcurrent)%BKMAX;
// MESG("set_hmark: hmstart=%d hmcurrent=%d current_id=%d %s",hmstart,hmcurrent,current_ind,from);
 hmark[current_ind].line=getcline()+1;
 hmark[current_ind].column=getccolumn()+1;
// MESG("set_hmark: f=%s l=%ld c=%ld",cbfp->b_fname,hmark[current_ind].line,hmark[current_ind].column);
 strlcpy(hmark[current_ind].fname,cbfp->b_fname,MAXFLEN);
 strlcpy(hmark[current_ind].dname,cbfp->b_dname,MAXFLEN);
 if(flag) {
	hmnum=hmcurrent; // forget all next marks
 };
 return TRUE;
}

void update_hmark()
{
 int current_ind;
 current_ind=(hmstart+hmcurrent)%BKMAX;
// MESG("update_hmark: [%s] l=%ld c=%ld",hmark[current_ind].fname,hmark[current_ind].line,hmark[current_ind].column);
 hmark[current_ind].line=getcline()+1;
 hmark[current_ind].column=getccolumn()+1;
// MESG("		new:          l=%ld c=%ld",hmark[current_ind].line,hmark[current_ind].column);
}


int select_hmark(int nind)
{
 char **mark_list;
 int ind;
 int rn;
 int i;
 char value[MAXSLEN];
 mark_list=(char **)malloc(sizeof(char *)*(hmnum+2));
 for(i=0;i<=hmnum;i++){
 	rn=(hmstart+i)%BKMAX;
	snprintf(value,MAXSLEN,"%2d %s %ld",i,hmark[rn].fname,hmark[rn].line);
	mark_list[i]=strdup(value);
 };
 mark_list[i]=0;
 msg_line("select buffer from history");
 ind=selectl("select buffer from history",mark_list,hmnum+1,20,40,1,25,-1);
 sarray_clear(mark_list);
 msg_line("");
 if(ind<0) return 0;
 hmcurrent=ind;
 rn=(hmstart+ind)%BKMAX;
 goto_mark(&hmark[rn]);
 return 1;
}

/* delete all history marks of a buffer. */
int delete_hmark(int flag)
{
 int i1,i0;
 char now_fname[MAXFLEN];
 int rn1,rn2;
 int dels=0; 	/* deleted before hmcurrent  */

 if(flag) {
 rn1=(hmstart+hmcurrent)%BKMAX;
 strlcpy(now_fname,hmark[rn1].fname,MAXFLEN);
 for(i0=0;i0<=hmnum;i0++) {
 	rn1=(hmstart+i0)%BKMAX;
	if(!strcmp(now_fname,hmark[rn1].fname)){
		for(i1=i0;i1<hmnum;i1++){
			rn1=(hmstart+i1)%BKMAX;
			rn2=(hmstart+i1+1)%BKMAX;
			strlcpy(hmark[rn1].fname,hmark[rn2].fname,MAXFLEN);
			hmark[rn1].line=hmark[rn2].line;
			hmark[rn1].column=hmark[rn2].column;
 		};
		if(i0<=hmcurrent) dels++;
		i0--;
 		hmnum--;
 	}
 };
 hmcurrent-=dels;
 } else { // delete current
  if(hmnum<1) return 0;
  hmcurrent--;hmnum=hmcurrent;return 1;
  for(i0=hmcurrent;i0<hmnum;i0++) {
	rn1=(hmstart+i0)%BKMAX;
	rn2=(hmstart+i0+1)%BKMAX;
	strlcpy(hmark[rn1].fname,hmark[rn2].fname,MAXFLEN);
	hmark[rn1].line=hmark[rn2].line;
	hmark[rn1].column=hmark[rn2].column;
  };
  hmnum--;
 };
 return hmnum;
}


/* goto current history mark */
int current_hmark(int n)
{
 int rn;
 if(hmstart<BKMAX && hmcurrent<hmstart) return 0;	/* No mark!  */
 rn = (hmstart+hmcurrent)%BKMAX;

 rn=(hmstart+hmcurrent)%BKMAX;
 
 goto_mark(&hmark[rn]);
 return TRUE;
}

/* goto previous history mark */
int prev_hmark(int n)
{
 int rn;
 // MESG("prev_hmark:");
 if(cwp->w_fp->b_flag & FSDIRED){ /* In dir mode exit view or go to parent dir */
 	// MESG("prev_hmark: move left!");
	dir_left(0);
	return TRUE;
 }
 if(hmcurrent<1) return(FALSE);
 
 if(cbfp->b_state & FS_VIEW) return (dir_left(0));
 rn = (hmstart+hmcurrent)%BKMAX;

 hmcurrent--;
// MESG("prev_hmark: %d -> %d",hmcurrent,rn);
 rn=(hmstart+hmcurrent)%BKMAX;
 goto_mark(&hmark[rn]);
 return TRUE;
}


/* goto next history mark */
int next_hmark(int n)
{
 int rn;
 if(hmcurrent+n >  hmnum) return FALSE;
 
 hmcurrent+=n;
 rn=(hmstart+hmcurrent)%BKMAX;
 goto_mark(&hmark[rn]);
 return TRUE;
}

/* -- */
