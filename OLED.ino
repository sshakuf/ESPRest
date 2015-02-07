#ifdef OLED

#include <U8glib.h>

U8GLIB_SH1106_128X64 u8g(7, 6, 5, 4);   // SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7
#define BUFFSIZE 15
#define NUMOFLINES 4
char* Lines[NUMOFLINES] = {"1               ","2               ","3               ","4               "};

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
 for (int i=0; i<NUMOFLINES; i++)
 {
    u8g.drawStr( 0, 10 + (i * 17), Lines[i]);
 }
}


void println(int inLineNumber, String inMessage)
{
  if (inLineNumber < NUMOFLINES)
  {
     inMessage.toCharArray(Lines[inLineNumber], BUFFSIZE);
  }
}
void println(int inLineNumber, char*inMessage)
{
  if (inLineNumber < NUMOFLINES)
  {
    bool shouldClear = false;
    for (int i=0; i < BUFFSIZE; i++)
    {


      if (!shouldClear)
      {
        Lines[inLineNumber][i] = inMessage[i];
        if (inMessage[i] =='\0')
        {
            shouldClear = true;
        }
      }
      else 
      {
        Lines[inLineNumber][i] = '\0';
      }
      Lines[inLineNumber][BUFFSIZE-1] = '\0';
    }
  }
}


void InitializeDisplay(void) {
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

}

void LCDLoop(void) {
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  // delay(50);
}

#endif OLED

