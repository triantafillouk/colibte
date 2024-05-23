#define	TEST1	1
#define	TEST0	0

void FindLineCol(TextPoint *tp)	/* -- 1 --*/
{
 FILEBUF *fp=tp->fp;

  if(tp->offset<1)
   {
      tp->col=tp->line=tp->offset=0;
      tp->flags = FULLDEFINED;
      return;
   }
   if(!(tp->flags & LINECOLUNDEFINED))  {	/* already defined!  */
		ERROR("findlinecol: already defined!");	/* to be removed !!!!  */
		return;
	};
	if(fp->view_mode & VMHEX) {
		tp->line = tp->offset / HEX_LINE_LEN;
		tp->col = tp->offset % HEX_LINE_LEN;
		tp->flags = FULLDEFINED;
		return;
	};

	TextPoint   *found=NULL;
	TextPoint	*scan;
	offs	 dist=INT_MAX;
	offs	 new_dist=INT_MAX-1;
	// MESG("FindLineCol: [%s] o=%ld",tp_name[tp->tp_type],tp->offset);
	for(scan=fp->tp_last; scan; scan=scan->t_next) {
		if(scan==tp) continue;
		// MESG("flc: check [%s] o=%ld",tp_name[scan->tp_type],scan->offset);
		if(!(scan->flags)) 
		{
#if	TEST0
			new_dist=tp->offset-scan->offset;
			if(new_dist<0) continue;
#else
			new_dist=llabs(tp->offset-scan->offset);
#endif
	 		// if(new_dist <= dist )
	 		if(new_dist < dist )
			// if(scan->offset<=tp->offset)
			{
	    		dist=new_dist;
	    		if(dist==0) {
				   tp->col=scan->col;
				   tp->line=scan->line;
				   tp->flags = FULLDEFINED;
				   return;				
				};
				// MESG("	findlinecol:[%s] based on [%s] o=%lld col=%lld line=%lld dist=%d",tp_name[tp->tp_type],tp_name[scan->tp_type],tp->offset,tp->col,tp->line,dist);
	    		found=scan;
	 		}
      	}
	}

   offs  o;
   num   l,c;

   if(found) {
     o=found->offset;
     c=found->col;
     l=found->line;
#if	WRAPD
	 // MESG("findlinecol: o=%ld found o=%ld c=%ld l=%ld",tp->offset,o,c,l);
#endif
   } else {
      o=c=l=0;
   };

	// MESG_time(";flc:[%s] o=%lld from [%s] o=%lld col=%lld line=%lld ",tp_name[tp->tp_type],tp->offset,tp_name[found->tp_type],o,c,l);
   if(o>tp->offset)  {	/* go back lines  */
	  // MESG("	go back lines: o=%ld offset=%ld",o,tp->offset);
      o=FLineBegin(fp,o);
      c=0;
      while(o>tp->offset) {
         o=FPrevLine(fp,o);
         l--;
      };
#if	WRAPD
	  MESG("	go back and start from o=%ld l=%ld c=0",o,l);
#endif
   };

   // MESG("findlinecol: o=%ld c=%ld l=%ld offset=%ld",o,c,l,tp->offset);
#if	TEST1
	char eol=fp->EolStr[fp->EolSize-1];
	
	while(o<tp->offset) {	/* go forward lines  */
		if(FCharAt_NoCheck(fp,o)==eol) {
			o++;
			if(fp->EolSize>1) {
				if(FCharAt_NoCheck(fp,o)==fp->EolStr[1]) {l++;c=0;o++;};
			} else {
				l++;c=0;			
			};
		} else {
			int c1;
			utfchar uc;
			o=FUtfCharAt(fp,o,&uc);
			c1=uc.uval[0];
			if(c1==CHR_TAB) c=next_tab(c);
			else c+=get_utf_length(&uc);
		};
	}
	// MESG("findlinecol: new o=%ld c=%ld l=%ld",o,c,l);
#else
   offs fsize=FSize(fp);
   while(o<tp->offset) {	/* go forward lines  */
      offs next_line=FNextLine(fp,o);

      if(next_line>tp->offset||(o+1)==fsize) break;

	  if( !FBolAt(fp,next_line)) { 
		if(next_line==fsize) {
			o=FLineBegin(fp,next_line);
			c=0;break;
		} else {
			msg_log(1,"FindLineCol:3 !fbol o=%lld",next_line);	/* should never pass from here!!  */
	  		o=next_line; continue;
		}
	  };

      o=next_line;
      l++;
      c=0;
   };
	c += DiffColumn(fp,&o,tp->offset,"FindLineCol:OK");
#endif
#if	WRAPD
	// MESG("	we are on the same line %ld, find col = %ld",l,c);
#endif
   tp->col=c;
   tp->line=l;
   if(o!=tp->offset) {
#if	WRAPD
	   MESG("FindLineCol: offset modified!, o=%ld != tp->offset=%ld l=%ld c=%ld",o,tp->offset,l,c);
#endif
	   tp->offset=o;
   };
   // MESG("           : o=%ld c=%ld l=%ld offset=%ld",o,c,l,tp->offset);

   tp->flags = FULLDEFINED;
	// MESG_time(";flc:[%s] -> o=%lld col=%lld line=%lld ",tp_name[tp->tp_type],o,c,l);
}
