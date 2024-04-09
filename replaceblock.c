#if	NUSE
int ReplaceBlock(char *string_to_insert,offs size_to_delete)
{
 int insert_size = strlen(string_to_insert);
 FILEBUF *fp = cbfp;
 
 {	// first delete the block 
   offs  base;	// start of delete
   num   i;

   num   newline;
   num   deleted_lines;
   int   join_at;
   int   break_at;
   offs size;
   if(size_to_delete<1)  return(true);
   if(fp->b_flag & FSMMAP)  return false;

   if(size_to_delete>FSize(fp)-FOffset(fp))      size_to_delete=FSize(fp)-FOffset(fp);

   PreModify(fp);
	
   if(fp->oldptr2>fp->ptr2 && size_to_delete>0)      fp->oldptr2=fp->ptr2;

   if(undo_enabled(fp)) {
		undo_AddChange(fp,undo_new_Change(DELETE,fp->buffer+fp->ptr1,0,fp->buffer+fp->ptr2,size_to_delete));
		undo_AddChange(fp,undo_new_Change(INSERT,string_to_insert,insert_size,NULL,0));
	};

   size=size_to_delete;
   base=FOffset(fp); // positive!

   // TextPoint *base_point=textpoint_o(fp,base,TP_BP_DELETE_BLOCK,0);
   newline=tp_line(fp->tp_current);
//    MESG("replace: l=%ld size=%ld [%s]",newline,size,string_to_insert);
	TextPoint *t=textpoint_o(fp,base+size,TP_DELETE_BLOCK,0);

	deleted_lines=tp_line(t)-newline;	// lines deleted
	textpoint_delete(t);

   break_at=-1;
   for(i=1; i<fp->EolSize; i++) {
      if(FBolAt(fp,base+size+i)) {
        break_at=i;
		MESG("break_at=%d",i);
        break;
      }
   }

   fp->ptr2+=size_to_delete;
   fp->GapSize+=size_to_delete;

   join_at=-1;
   for(i=1; i<fp->EolSize; i++) {
     if(FBolAt(fp,base+i)) {
       join_at=i;
       break;
     }
   }

	TextPoint *scan;
	for(scan=fp->tp_last; scan; scan=scan->t_next){
		if(scan->offset > base) { 
			scan->flags = LINECOLUNDEFINED;
		}
	}

   for(scan=fp->tp_last; scan; scan=scan->t_next) {
//	  MESG("check text point num=%d type %d= [%s] o=%ld bs=%ld o=%ld l=%ld",scan->num,scan->tp_type,tp_name[scan->tp_type],scan->offset,base+size,tp_offset(scan),tp_line(scan));
      if(scan->offset >= base+size) 
	  {
        scan->offset-=size;	// this change the column and possibly the line!!!!!!
//          if((scan->flags&LINEUNDEFINED)) 
		 {

            scan->line-=deleted_lines;
            if(break_at>0 && scan->offset>=base+break_at)  scan->line--;
            if(join_at>0  && scan->offset>=base+join_at)  scan->line++;

            if(scan->line==newline)  {
				scan->flags|=COLUNDEFINED;
				FindLineCol(scan);
			} else {
				scan->flags=0;
			};
         };
      } else if(scan->offset>base) {
	 	tp_copy(scan,fp->tp_current);
      }
//	  MESG("	new o=%ld l=%ld",tp_offset(scan),tp_line(scan));
   }
	update_lines(fp);
 };
 { 	// Insert the new value
   num   i;
   offs  oldoffset;
   num   num_of_lines,num_of_columns,oldline;
   int   break_at;
   offs	 size;
   if(fp->b_flag & FSMMAP) return false;
   size=insert_size;
   if(size==0) return(true);
//	MESG("InsertBlock: pos=%ld l=%ld r=%ld",FOffset(fp),insert_size,size_size_to_delete);
   PreModify(fp);

   if(insert_size>0) {
      if(fp->oldptr1 > fp->ptr1) fp->oldptr1=fp->ptr1;
   }
	
   if(GetSpace(fp,insert_size)!=true) return(false);

   memmove(fp->buffer+fp->ptr1,string_to_insert,insert_size);

   oldoffset=FOffset(fp);
   break_at=-1;
   for(i=1; i<fp->EolSize; i++) {
      if(FBolAt(fp,oldoffset+i)) {
         break_at=i;
         break;
      }
   }

   num_of_lines=0;
   num_of_columns=tp_col(fp->tp_current);
   oldline=tp_line(fp->tp_current);

   fp->ptr1+=insert_size;
   fp->GapSize-=size;

   CalculateLineCol(fp,&num_of_lines,&num_of_columns,oldoffset,oldoffset+insert_size);
   CalculateLineCol(fp,&num_of_lines,&num_of_columns,oldoffset+insert_size,oldoffset+size);

//   MESG("	ins: old_offset=%ld left=%ld size_to_delete=%ld lines=%ld break_at=%d join_at=%d",oldoffset,insert_size,size_size_to_delete,num_of_lines,break_at,join_at);
   TextPoint *scan;
   for(scan=fp->tp_last; scan; scan=scan->t_next) {
	  if(scan==fp->tp_current) {
//	  	MESG("	point: [%s] window %d,f=%d o=%ld l=%ld current skip!",tp_name[scan->tp_type],scan->window,scan->flags,scan->offset,scan->line);
	  	continue;
      } else { 
	  	if(scan->offset > oldoffset) 
		{
	         if(!(scan->flags&LINEUNDEFINED)) {
	            if(scan->line==oldline) scan->flags|=COLUNDEFINED;
	            scan->line+=num_of_lines;
	            if(break_at>0 && scan->offset>=oldoffset+break_at)
	               scan->line--;
	         }
	         scan->offset+=size;
//		  	MESG("	update point> [%s] window %d,f=%d o=%ld l=%ld",tp_name[scan->tp_type],scan->window,scan->flags,scan->offset,scan->line);
		} else {
//	  		MESG("	point: [%s] window %d,f=%d o=%ld l=%ld smaller skip!",tp_name[scan->tp_type],scan->window,scan->flags,scan->offset,scan->line);
	    };
	  }   
	}

	fp->b_state |= FS_CHG;
	update_lines(fp);
 }
 return true;
}
#endif