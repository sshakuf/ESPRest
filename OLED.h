#ifndef OLED_H
#define OLED_H

#ifdef OLED

void draw(void);
void InitializeDisplay(void);
void LCDLoop(void);
void println(int inLineNumber, String inMessage);
void println(int inLineNumber, char*inMessage);

#endif OLED

#endif OLED_H
