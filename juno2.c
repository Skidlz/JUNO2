/*
 * juno2.c
 *
 *  Created on: Jul 6, 2016
 *      Author: Zack
 */


#include <stdio.h>		/* compiler library module header */
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "standard.h"	/* header files for patchlib.c */
#include "chain.h"
#include "writscrn.h"
#include "patchlib.h"
#include "bioscall.h"
#include "patched.h"
#include "patchlod.h"

struct selement ajuno1[NPARAMS1] = {		/* primary menu cursor block */
		{7,		1,	"DCO OCT", 			34,	2,	1,	1,	6}, 	/* 06 DCO OCT		*/
		{43,	1,	"DCO LFO  AMT",		35,	3,	0,	0,	11},	/* 11 DCO LFO MOD	*/
		{7,		2,	"DCO ENV AMT",		0,	4,	3,	3,	12},	/* 12 DCO ENV MOD	*/
		{43,	2,	"DCO AFTR AMT",		1,	10,	2,	2,	13},	/* 13 DCO AFTER		*/
		{7,		3,	"DCO ENV MODE",		2,	5,	4,	4,	0},		/* 00 DCO ENV MODE	*/
		{7,		4,	"DCO BEND RNG",		4,	6,	5,	5,	35},	/* 35 PITCH BEND 	*/
		{7,		5,	"PULSE", 			5,	7,	6,	6,	3},		/* 03 DCO PULS		*/
		{7,		6,	"SAWTOOTH", 		6,	8,	7,	7,	4},		/* 04 DCO SAW		*/
		{7,		7,	"SUB WAVE", 		7,	9,	8,	8,	5},		/* 05 DCO SUB HARM	*/
		{7,		8,	"SUB VOL", 			8,	11,	10,	10,	7},		/* 07 DCO SUB LVL	*/
		{43,	8,	"NOIS VOL", 		3,	12,	9, 	9,	8},		/* 08 DCO NOIS LVL	*/
		{7,		9,	"PWM AMT", 			9,	13,	12,	12,	14},	/* 14 DCO PWM AMT	*/
		{43,	9,	"PWM RATE", 		10,	14,	11,	11, 15},	/* 15 DCO PWM RATE	*/
		{7,		10,	"LFO RATE", 		11,	15,	14,	14,	24},	/* 24 LFO RATE		*/
		{43,	10,	"LFO DELAY", 		12,	17,	13,	13, 25},	/* 25 LFO DELAY		*/
		{7,		11,	"HPF FREQ", 		13,	16,	15,	15,	9},		/* 09 HPF FREQ		*/
		{7,		12,	"VCF FREQ", 		15,	18,	17,	17,	16},	/* 16 VCF FREQ		*/
		{43,	12,	"VCF RES",		 	14,	19,	16,	16,	17},	/* 17 VCF RES		*/
		{7,		13,	"VCF ENV AMT", 		16,	20,	19,	19,	19},	/* 19 VCF ENV AMT	*/
		{43,	13,	"VCF LFO  AMT", 	17,	21,	18,	18,	18},	/* 18 VCF LFO AMT	*/
		{7,		14,	"VCF KEY AMT", 		18,	22,	21,	21,	20},	/* 20 VCF KEY AMT	*/
		{43,	14,	"VCF AFTR AMT", 		19,	24,	20,	20,	21},	/* 21 VCF AFTER		*/
		{7,		15,	"VCF ENV MODE", 	20,	23,	22,	22,	1},		/* 01 VCF ENV MODE	*/
		{7,		16,	"VCA VOL", 			22,	25,	24,	24,	22},	/* 22 VCA LVL		*/
		{43,	16,	"VCA AFTR AMT", 	21,	27,	23,	23,	23},	/* 23 VCA AFTER		*/
		{7,		17,	"VCA ENV MODE", 	23,	26,	25,	25,	2},		/* 02 VCA ENV MODE	*/
		{7,		18,	"CHORUS ON", 		25,	28,	27,	27,	10},	/* 10 CHORUS on/off	*/
		{43,	18,	"CHORUS RATE", 		24,	29,	26,	26,	34},	/* 34 CHORUS RATE	*/
		{7,		19,	"ATTACK  TIME", 	26,	30,	29,	29,	26},	/* 26 ATTACK TIME	*/
		{43,	19,	"ATTACK  LVL",	 	27,	31,	28,	28,	27},	/* 27 ATTACK LVL	*/
		{7,		20,	"BREAK   TIME", 	28,	32,	31,	31,	28},	/* 28 BREAK TIME 	*/
		{43,	20,	"BREAK   LVL", 		29,	33,	30,	30,	29},	/* 29 BREAK LEVEL 	*/
		{7,		21,	"DECAY   TIME", 	30,	34,	33,	33,	30},	/* 30 DECAY TIME	*/
		{43,	21,	"SUSTAIN LVL", 		31,	35,	32,	32,	31},	/* 31 SUSTAIN LVL	*/
		{7,		22,	"RELEASE TIME", 	32,	0,	35,	35,	32}, 	/* 32 RELEASE TIME	*/
		{43,	22,	"ENV KEYBOARD",		33,	1,	34,	34,	33}};	/* 33 ENV KEY AMT	*/


main()
{
	int status, pick,lastpick,patchno, ans,i;

					/* pointers to screen images (see chain.h) */
	struct strchain *chain [NSCREEN];

					/* array to hold coded synth data */
	static unsigned char tonebyte[NPATCH] [NCODPARAM];
	static int ajparam[NPARAM];	/* working data for one patch's parameters */
	static char ajname[11];		/* also save name of patch */
	static char buf[17];		/* misc. char buffer */
	static char nbuf[10];		/* buffer for ascii value of a number */

/* load screens into memory */

	clearsc();
	fputs("Loading screen images...",stdout);

	for (i = 0; i < NSCREEN; i++){
		strcpy(buf,"ajuno");
		nbuf[0] = '1' + i;
		nbuf[1] = '\0';
		strcat(buf,nbuf);
		strcat(buf,".scr");
		chain[i] = inpchain(buf,SCRNWIDE + 1);
	}

	for (i = 0; i < NSCREEN; i++){
		if (chain[i] == NULL){
			fputs("Failed to load screen file ajuno",stdout);
			itoa(i+1, buf, 10);
			fputs(buf,stdout);
			fputs(".scr\n",stdout);
			fputs("Be sure you are running Patchlib from the default drive.",
				stdout);
			exit(0);
		}
	}

	patchno = -1;
	for (i = 0; i < 63; i++) clearval(i, ajparam, ajname, tonebyte); /* fixed */

/* Put the primary menu on the screen, plus current settings */
/* Loop to and from dependent program segments based on selection */

	/* edpatch(patchno, ajparam, ajname, tonebyte, chain); */
	edval(NPARAMS1,ajuno1,chain[0],ajparam,ajname);

	pick = 0;
	while (1){
		clearsc();
		/*dispchain(chain[0]); */

		while (kbhit()) getch();

		lastpick = pick;
		pick = movescrn(ajuno1,pick,34);
		switch (pick){
		case (3):								/* edit */
			if (patchno < 0)
				writerr("err");
			else
				edpatch(patchno, ajparam, ajname, tonebyte, chain);
			break;
		case (6):								/* upload */
			if (patchno < 0)
				writerr("err");
			else
				upload(tonebyte);
			break;
		case (7):								/* download */
			status = download(ajparam, ajname, tonebyte);
			if (status != -1){
				/*strcpy(libname,"NO_NAME.LBR");*/
				patchno = 0;
			}
			break;
		case (-2):								/* esc key */
		case (34):								/* quit */
			writword(BWC, "Quit Patchlib?(Y/N) ",0,SCRNTALL);
			ans = getche();
			if (toupper(ans) != 'Y')
				pick = 0;
			else {
				clearsc();
				exit(0);
			}
			break;
		default:
				writerr("Use arrow keys to move cursor, ret to select.");
				pick = lastpick;
		}
	}
}

