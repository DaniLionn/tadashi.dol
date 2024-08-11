/****************************************************************************
 * tadashi.dol
 * DaniLionn 2024
 * Based on the Wii OGG Playback example by Tantric
 ***************************************************************************/

#include <stdio.h>

#include <stdlib.h>

#include <gccore.h>

#include <wiiuse/wpad.h>

#include <asndlib.h>

#include <time.h>

#include <string.h>

#include<unistd.h>

#include "oggplayer.h"

// include generated header
#include "sample_ogg.h"

char * colours[] = {
  "\x1b[31;40m",
  "\x1b[32;40m",
  "\x1b[33;40m",
  "\x1b[34;40m",
  "\x1b[35;40m",
  "\x1b[36;40m",
  "\x1b[37;40m"
};
int current = 0;
bool partyMode = false;
bool refreshLock = true;
bool debug = false;
static void * xfb = NULL;
static GXRModeObj * rmode = NULL;

int random_number(int min, int max) {
  srand(time(NULL));
  return (rand() % (max - min + 1)) + min;
};
const char * tadashi = "tadashi.dol\n                                                                   \n                             ####  ###                             \n                    ############       #                           \n                 ###################                               \n               #+##################+###                            \n             ##+++#################++# ##                          \n            ##++#++++++++########--#####  #                        \n           ##++####+######-######---#####  #                       \n          ################-###-#+--#-####  ##                      \n         # ##############--##+-#----+####  #                       \n         # ###############-#--#---+--####                          \n          +#############--#---------- ####                         \n           ######-+#####---------+##-    #####                     \n             ####+-++####-----#####-      ## ######                \n                  ########-----###-#        #   ###+               \n                    +#--###-------+          ##  ##++    #         \n                ####-+-----###        #######   ##+#+   #          \n    ###########   ###------- #######+-+-       ##+######           \n    ####       ########--#+#    ###-------####+++#+#               \n     ##+#    ###########---########-------+- #### #                \n     ###++##############-++########---------##++####               \n       ##++#############--+#######--+------+# ++###                \n         ################-########---------+#   ####               \n          ################-######------++---#              ##      \n          #################-####------###---#####     ###          \n          ##################-#####+-######+####+##                 \n          Press HOME to exit (Don't know why you would.)          ";
void refresh(int colour) {

  printf("\e[1;1H\e[2J");
  printf(colours[colour]);

  if (debug == true) {
    printf("%d", colour);

  }

  printf(tadashi);
};
//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
  //---------------------------------------------------------------------------------

  // Initialise the video system
  VIDEO_Init();

  // Initialise the attached controllers
  WPAD_Init();

  // Initialise the audio subsystem
  ASND_Init();

  // Obtain the preferred video mode from the system
  // This will correspond to the settings in the Wii menu
  rmode = VIDEO_GetPreferredMode(NULL);

  // Allocate memory for the display in the uncached region
  xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

  // Initialise the console, required for printf
  console_init(xfb, 20, 20, rmode -> fbWidth, rmode -> xfbHeight, rmode -> fbWidth * VI_DISPLAY_PIX_SZ);

  // Set up the video registers with the chosen mode
  VIDEO_Configure(rmode);

  // Tell the video hardware where our display memory is
  VIDEO_SetNextFramebuffer(xfb);

  // Make the display visible
  VIDEO_SetBlack(false);

  // Flush the video register changes to the hardware
  VIDEO_Flush();

  // Wait for Video setup to complete
  VIDEO_WaitVSync();
  if (rmode -> viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

  refresh(6);

  PlayOgg(sample_ogg, sample_ogg_size, 0, OGG_INFINITE_TIME);

  while (1) {

    // Call WPAD_ScanPads each loop, this reads the latest controller states
    WPAD_ScanPads();

    // WPAD_ButtonsDown tells us which buttons were pressed in this loop
    // this is a 'one shot' state which will not fire again until the button has been released
    u32 pressed = WPAD_ButtonsDown(0);

    // We return to the launcher application via exit
    if (pressed & WPAD_BUTTON_HOME) break;
    if (pressed & WPAD_BUTTON_A) {
      if (partyMode == true) {
        partyMode = false;
      } else {
        partyMode = true;
      };

    };

    if (partyMode == true) {
      refreshLock = false;
      refresh(current);
      current++;

      if (current > 5) {
        current = 0;
      };
      usleep(261000);
    } else {
      if (refreshLock == false) {

        refreshLock = true;
        refresh(6);

      };
    };
    // Wait for the next frame
    VIDEO_WaitVSync();
  }
  StopOgg();
  return 0;
}