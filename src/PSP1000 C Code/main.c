/*
 * PSP Software Development Kit - http://www.pspdev.org
 * This app' was downloaded from http://goolstuff.empis.se.
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - PSP Phat/1000 Infrared Boe-Bot Project
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 * Copyright (c) 2005 Matthew H <matthewh@webone.com.au>
 *
 * $$
 */

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspsircs.h>
#include <stdlib.h>
#include <string.h>

/* Define the module info section */
PSP_MODULE_INFO("The PSP Boe-Bot Project", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

// sircs commands are easily found in lirc
// - interpret as described at 
//   http://sourceforge.net/mailarchive/message.php?msg_id=8833252

#define SIRCS_ADDR_BOE	     0x1b5a

void send_code(int type, int dev, int cmd) 
{
	struct sircs_data sd;
	int ret;
	int count = 1; // this seems like a good number

	sd.type = type;
	sd.cmd = cmd & 0x7f;
	sd.dev = dev & 0x1fff;

	ret = sceSircsSend(&sd, count); 
	if (ret < 0) 
	{
		printf ("sceSircsSend returned %d\n", ret);
	}
} 

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	sceKernelExitGame();

	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

int main(void)
{
	SceCtrlData pad;
	int sirc_bits = 15; // # of bits in code, choose from 12, 15 or 20
	int BoeCmd;
	char strMessage[50];

	SetupCallbacks();
	pspDebugScreenInit();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);

	printf ("\n PSP Boe-Bot Project V1.0\n");
  printf (" -------\n");
	printf (" This app' can be used with a Boe-Bot with a\n BS2 loaded with GooLStuFF_Boe-Bot_Project.bs2\n\n");
  printf ("  Press Cross to move Forwards\n  Press Left to Rotate Left\n  Press Right to Rotate Right\n  Press Square/Triangle to move Backwards\n  Hold Right Trigger and press any button to make beeping noises\n\n   This application and more fun\n   can be found at http://goolstuff.empis.se\n\n");
	
	while(1)
	{
		BoeCmd = 0;
		sceCtrlReadBufferPositive(&pad, 1);
		
		/*if(pad.Buttons & PSP_CTRL_LTRIGGER)
		{
    	 pspDebugScreenClear();
			 printf ("PSP Boe-Bot Project V1.0\n");
			 printf ("-------\n");
			 printf ("This app' can be used with a Boe-Bot with a\n BS2 loaded with GooLStuFF_Boe-Bot_Project.bs2\n\n");
       printf ("Press Cross to move Forwards\nPress Left to Rotate Left\nPress Right to Rotate Right\nPress Square/Triangle to move Backwards\nPress Left Trigger to Clear screen\nHold Right Trigger and press any button to make beeping noises\n\n");
    }*/
		
		if (pad.Buttons & PSP_CTRL_LEFT) 
		{
			//printf("Rotate Left");
						
			BoeCmd = 0x02;
		}

		if (pad.Buttons & PSP_CTRL_RIGHT)
		{
			//printf("Rotate Right");
	
			BoeCmd = 0x03;
		}	
		
		if (pad.Buttons & PSP_CTRL_CROSS) 
		{
			//printf("Forwards");
	
			BoeCmd = 0x01;
		}	
		
		if (pad.Buttons & PSP_CTRL_CIRCLE)
		{
			if (pad.Buttons & PSP_CTRL_LEFT)
			{
				//printf("Still-Left");
						
				BoeCmd = 0x05;
			}
			if (pad.Buttons & PSP_CTRL_RIGHT)
			{
				//printf("Still-Right");
						
				BoeCmd = 0x06;
			}
		}
		
		if (pad.Buttons & PSP_CTRL_LEFT && pad.Buttons & PSP_CTRL_CROSS)
		{
			//printf("Forwards-Left");
						
			BoeCmd = 0x0a;
		}
		if (pad.Buttons & PSP_CTRL_RIGHT && pad.Buttons & PSP_CTRL_CROSS)
		{
			//printf("Forwards-Right");
						
			BoeCmd = 0x09;
		}	
		
		if (pad.Buttons & PSP_CTRL_SQUARE || pad.Buttons & PSP_CTRL_TRIANGLE)
		{
			//printf("Backwards");
	
			BoeCmd = 0x04;
			if (pad.Buttons & PSP_CTRL_LEFT)
			{
				//printf("Backwards-Left");
					
				BoeCmd = 0x0b;
			}
			if (pad.Buttons & PSP_CTRL_RIGHT)
			{
				//printf("Backwards-Right");
	
				BoeCmd = 0x0c;
			}
		}
			
		//Och nu de skojiga ljuden
		if(pad.Buttons & PSP_CTRL_RTRIGGER)
		{
	  	if(pad.Buttons & PSP_CTRL_UP)
			{
	      BoeCmd = 0x14;
	    }
			if(pad.Buttons & PSP_CTRL_DOWN)
			{
	      BoeCmd = 0x15;
	    }
			if(pad.Buttons & PSP_CTRL_LEFT)
			{
	      BoeCmd = 0x16;
	    }
			if(pad.Buttons & PSP_CTRL_RIGHT)
			{
	      BoeCmd = 0x17;
	    }
			if(pad.Buttons & PSP_CTRL_TRIANGLE)
			{
	      BoeCmd = 0x18;
	    }
			if(pad.Buttons & PSP_CTRL_CROSS)
			{
	      BoeCmd = 0x19;
	    }
			if(pad.Buttons & PSP_CTRL_SQUARE)
			{
	      BoeCmd = 0x1a;
	    }
			if(pad.Buttons & PSP_CTRL_CIRCLE)
			{
	    	BoeCmd = 0x1b;
	    }
			if(pad.Buttons & PSP_CTRL_SELECT)
			{
	      BoeCmd = 0x1c;
	    }
			if(pad.Buttons & PSP_CTRL_START)
			{
	      BoeCmd = 0x1d;
	    }
    }
		
		if (BoeCmd != 0)
		{
			send_code(sirc_bits, SIRCS_ADDR_BOE, BoeCmd);
		}
		
      sceDisplayWaitVblankStart();
	}

	return 0;
}