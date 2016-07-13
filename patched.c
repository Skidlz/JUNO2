/* patched.c patch edit routines for patchlib.c */

#include <stdio.h>		/* compiler library module header */
#include <ctype.h>
#include <conio.h>
#include <math.h>

#include "standard.h"	/* header files for patchlib.c */
#include "writscrn.h"
#include "patchlib.h"
#include "patched.h"
#include "chain.h"

#include "edstruct.h"
#include "io401.h"
#include "bioscall.h"

extern unsigned _floatconvert;
#pragma extref _floatconvert


edpatch(int patchno, int param[], char name[],
		unsigned char tonebyte[NPATCH][NCODPARAM],
		struct strchain *chain[])	/* edit patch parameters */
{
	int pick;
	pick = 0;

	while (1) {
		clearsc();
		while (kbhit()) getch();				/* clear any stray keypress */
		dispchain(chain[1]);					/* display screen */

		decode(patchno, param, name,tonebyte);	/* get patch data */
		writword(RVC,name,68,0);				/* put patch name in upper left */

		pick = movescrn(ajuno2,pick,7);
		switch (pick){
		case (0): edval(NPARAMS3,ajuno3,chain[2],param,name); break;
		case (1):
		case (2): edval(NPARAMS5,ajuno5,chain[4],param,name); break;
		case (3):
		case (4): edval(NPARAMS6,ajuno6,chain[5],param,name); break;
		case (5): edval(NPARAMS4,ajuno4,chain[3],param,name); break;
		case (6): edval(NPARAMS7,ajuno7,chain[6],param,name); break;
		case (-2):
		case (7): return(1);
		default:
			writerr("Esc to exit, ret to select.");
			pick = 0;
		}
		encode(patchno, param, name, tonebyte);	/* put data back in tonebyte */
	}
}


int writVal(int key, int val){
	int xpos, ypos, i;
	char sval[16];
	float bar;
	const int bLen = 16;

	xpos = ajscrn[key].xpos;
	ypos = ajscrn[key].ypos;
	itoa(val,sval,10);
	writword(BWC,"   ",xpos,ypos);
	writword(BWC,sval,xpos,ypos);
	gotoxy(ajscrn[key].xpos, ajscrn[key].ypos+1);

	if (ajscrn[key].max < 6) return(-1); /* exit if too few states */

	bar = (float)(val*bLen)/ajscrn[key].max;
	for (i = 0; i < bLen; i++){
		if ((bar - i) >= 1)sval[i] = 219; /* full block */
		else if ((bar - i) > .5) sval[i] = 221; /* half */
		else sval[i] = ' '; /* blank */
	}
	sval[i] = '\0'; /* end */
	xpos += 4; /* draw to the right of the value */

	writword(BWC,sval,xpos,ypos); /* draw bar */
	gotoxy(ajscrn[key].xpos, ajscrn[key].ypos+1);
	return(1);
}

edval(int numel, struct selement *scrn,
		struct strchain *chain, int param[], char name[])
{
	int key, n, val, xpos, ypos, pick, i;
	char sval[16], ans, loopFlg, c, numFlg;

	clearsc();
	dispchain(chain);				/* put screen image on crt */

	/* writword(RVC,name,8,0);		 put patch name in upper left*/

	for (n = 0; n < numel; n++){		/* put all current values on */
		key = scrn[n].key;
		val = param[key];
		writVal(key, val );
	}

	pick = 0;
	while (1){
		pick = movescrn(scrn,pick,numel -1);
		if (pick == -2){
			writword(BWC, "Quit Patchlib?(Y/N) ",0,SCRNTALL);
			ans = getche();
			if (toupper(ans) != 'Y'){
				clearcsr(numel,scrn);
				pick = 0;
				csrplot(0,SCRNTALL);
				writchr(' ',79);
			} else {
				clearsc();
				exit(0);
			}
			/*return(1); */
		} else if (pick == -3){
			writerr("Pick selection using arrow keys, then hit return");
			clearcsr(numel,scrn);
			pick = 0;
		} else if (pick == -1){
			writerr("Unexpected input, try again.");
			clearcsr(numel,scrn);
			pick = 0;
		} else if (pick == F1){				/* load one patch */
			dlpatch(param, name);	/* wait for MIDI or ESC */
			for (n = 0; n < numel -1; n++){		/* put all current values on */
				key = scrn[n].key;
				val = param[key];
				writVal(key, val );
			}
			clearcsr(numel,scrn);
			pick = 0;
		} else {
			writword(BWC,"Enter value: ",0,SCRNTALL -1);
			numFlg = 1;
			loopFlg = 1;
			while (loopFlg) {
				while (!kbhit()) ;		/* wait for keypress */
				c = getch();
				if (!c) c = getch();	/* delete null chars and get next */
				switch (c) {
				case KUP:	/* inc */
					numFlg = 0; /* not entering a number */
					key = scrn[pick].key;
					val = (param[key] > (param[key] + 8)%ajscrn[key].max)? ajscrn[key].max: param[key] + 8;
					if(val != param[key]){
						sendparam(key, val); /* send over midi */
						param[key] = val;
						writVal(key, val);
					}
					break;
				case KRIGHT:
					numFlg = 0; /* not entering a number */
					writword(BWC,"                       ",0,SCRNTALL-1);
					/* sendexcl(unsigned int code) */
					key = scrn[pick].key;
					val = param[key] + 1;
					if (val <= ajscrn[key].max){
						sendparam(key, val); /* send over midi */
						param[key] = val;
						writVal(key, val);
					}
					break;
				case KDOWN:	/* dec */
					numFlg = 0; /* not entering a number */
					key = scrn[pick].key;
					val = ((param[key] - 8) < 0)? 0: param[key] - 8;
					if(val != param[key]){
						sendparam(key, val); /* send over midi */
						param[key] = val;
						writVal(key, val);
					}
					break;
				case KLEFT:
					numFlg = 0; /* not entering a number */
					writword(BWC,"                       ",0,SCRNTALL-1);
					key = scrn[pick].key;
					val = param[key] - 1;
					if (val >= 0){
						sendparam(key, val); /* send over midi */
						param[key] = val;
						writVal(key, val);
					}
					break;
				case KHOME:	/* max */
				case KPGUP:
					numFlg = 0; /* not entering a number */
					writword(BWC,"                       ",0,SCRNTALL-1);
					key = scrn[pick].key;
					val = ajscrn[key].max;
					param[key] = val;
					sendparam(key, val); /* send over midi */
					writVal(key, val);
					break;
				case KEND:	/* min */
				case KPGDN:
					numFlg = 0; /* not entering a number */
					writword(BWC,"                       ",0,SCRNTALL-1);
					key = scrn[pick].key;
					val = 0;
					param[key] = val;
					sendparam(key, val); /* send over midi */
					writVal(key, val);
					break;
				case ESC:
					loopFlg = 0;
					writword(BWC,"                       ",0,SCRNTALL-1);
					break;
				case CR:
					loopFlg = 0;
					break;
				default:
					if(!numFlg) break;
					loopFlg = 0;
					*sval = c;
					putc(c, stdout);
					fgets(sval+1, 3, stdin);
					if (*sval != '\0'){
						val = atoi(sval);
						key = scrn[pick].key;

						if (val < 0 || val > ajscrn[key].max)
							writerr("Value entered is out of range.");
						else{
							param[key] = val;
							writVal(key, val);
						}
					}
				}
			}
			writword(BWC,"                       ",0,SCRNTALL-1);
		}
	}
}





clearcsr(int numel,struct selement *scrn)	/* clear curser areas following error recovery */
{
	int n;

	for (n = 0; n< numel; n++){
		writword(BWC,scrn[n].content,scrn[n].xpos,scrn[n].ypos);
	}
}



clearval(int patchno, int param[], char name[],
		unsigned char tonebyte[NPATCH][NCODPARAM])	/* sets all patch values to zero */
{
	int n;

	for (n = 0; n <= 35; n++){
		param[n] = 0;
	}
	strcpy(name,"         ");
	encode(patchno, param, name, tonebyte);
}



namepatch(int patchno, int param[], char name[],
		unsigned char tonebyte[NPATCH][NCODPARAM])	/* name patch being edited */
{
	char newname[12];

	decode(patchno, param, name, tonebyte);
	writword(BWC,"Current patch name is: ",0,SCRNTALL - 4);
	puts(name);
	while (kbhit()) getch();			/* clear any stray keypress */

	writword(BWC,"Enter new name (ret to quit) -> ",0,SCRNTALL - 3);
	fgets(newname,11,stdin);

	if (*newname != '\n'){
		if (isupper(*newname) || islower(*newname) || isdigit(*newname)){
			strcpy(name,"          ");
			strcpy(name,newname);
			encode(patchno, param, name, tonebyte);
		} else {
			writerr("Not a valid name.");
		}
	}
}

