#include "U8glib.h"
#include <Bounce.h>

// Number of 8-bit bytes horizontal display.
#define HH 16
// Number of 1-pixel rows down on display.
#define RR 64

// U8glib documentation: https://code.google.com/p/u8glib/ 
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE); // 215ms draw, 82ms recalculate
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST); // 142ms draw, 82ms recalculate

// one extra row at the bottom so we don't have to wrap
uint8_t full_bitmap[(RR+1)*HH];

// new row is stored only when row +1 is already re-calculated
uint8_t new_top[HH];

// this is the new row we are always working on
uint8_t new_row[HH];

//#define STATS_ON
// stats
#ifdef STATS_ON
int cellsOn;
int cellsChanged;
int generation = 0;
#endif

//unsigned long time1;
//unsigned long time2;

// Recalculate Globals
uint8_t Nrow, Srow, Wcol, Ecol, NWCell, NCell, NECell, WCell, XCell, ECell, SWCell, SCell, SECell, newXCell;

// Button will be used to reset, press and hold
#define BUTTON 9
// Instantiate a Bounce object with a 5 millisecond debounce time
Bounce bouncer = Bounce( BUTTON,5 ); 

void initializeAcorn() {
  initialize(0);  // clear array
  // Acorn
  full_bitmap[32*HH+8]=32;
  full_bitmap[33*HH+8]=8;
  full_bitmap[34*HH+8]=103;  
}

void initializePentomino() {
  initialize(0);  // clear array
  // R-Pentomino
  full_bitmap[32*HH+8]=3;
  full_bitmap[33*HH+8]=6;
  full_bitmap[34*HH+8]=2;  
}

void initializeGosperGun() {
  initialize(0);  // clear array
  // Gosper Gun.
  // Top left is row 30, column 4
  full_bitmap[35*HH+4]=B01100000;
  full_bitmap[36*HH+4]=B01100000;

  full_bitmap[33*HH+5]=B00000110;  
  full_bitmap[34*HH+5]=B00001000;  
  full_bitmap[35*HH+5]=B00010000;  
  full_bitmap[36*HH+5]=B00010001;  
  full_bitmap[37*HH+5]=B00010000;  
  full_bitmap[38*HH+5]=B00001000;  
  full_bitmap[39*HH+5]=B00000110;  

  full_bitmap[32*HH+6]=B00000001;  
  full_bitmap[33*HH+6]=B00000110;  
  full_bitmap[34*HH+6]=B10000110;  
  full_bitmap[35*HH+6]=B01000110;  
  full_bitmap[36*HH+6]=B01100001;  
  full_bitmap[37*HH+6]=B01000000;  
  full_bitmap[38*HH+6]=B10000000;  

  full_bitmap[31*HH+7]=B01000000;  
  full_bitmap[32*HH+7]=B01000000;  
  full_bitmap[36*HH+7]=B01000000;  
  full_bitmap[37*HH+7]=B01000000;  

  full_bitmap[33*HH+8]=B00011000;  
  full_bitmap[34*HH+8]=B00011000;  

}

// Seed with some objects
void initialize2(void) {
  initialize(0);  // clear array

/*
  // Block
  full_bitmap[5*HH+1]=3;
  full_bitmap[6*HH+1]=3;

  // Beehive
  full_bitmap[10*HH+2]=6;
  full_bitmap[11*HH+2]=9;
  full_bitmap[12*HH+2]=6;
  
  // Loaf
  full_bitmap[15*HH+1]=6;
  full_bitmap[16*HH+1]=9;
  full_bitmap[17*HH+1]=5;
  full_bitmap[18*HH+1]=2;

  // Boat
  full_bitmap[1*HH+10]=6;
  full_bitmap[2*HH+10]=5;
  full_bitmap[3*HH+10]=2;

  // Blinker
  full_bitmap[3*HH+15]=7;

  // Blinker 2 - test top rollover
  full_bitmap[0*HH+12]=7;

  // Blinker 3 - test side rollover
  full_bitmap[15*HH+15]=1;
  full_bitmap[16*HH+15]=1;
  full_bitmap[17*HH+15]=1;

  // Toad
  // ...

  // Beacon
  full_bitmap[2*HH+12]=12;
  full_bitmap[3*HH+12]=8;
  full_bitmap[4*HH+12]=1;  
  full_bitmap[5*HH+12]=3;  

  // Pulsar
  // ...
  
  // Glider
  // ...
  
  // Lightweight Spaceship
  // ...
*/
  // Acorn
  full_bitmap[32*HH+8]=32;
  full_bitmap[33*HH+8]=8;
  full_bitmap[34*HH+8]=103;  

/*
  // R-Pentomino
  full_bitmap[32*HH+8]=3;
  full_bitmap[33*HH+8]=6;
  full_bitmap[34*HH+8]=2;  
*/  
}

// Generate random content to fill the display
void initialize(int maxnum) {
  // fill with 1k random values
  for(int px=0;px<HH*RR;px++)
    full_bitmap[px] = random(maxnum);
}

// Write bitmap to the display
void draw(void) {
  u8g.drawBitmap( 0, 0, HH, RR, full_bitmap);  
}

#ifdef STATS_ON
void drawCounts(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  char charBuf[10];
  sprintf(charBuf,"%d,%d",cellsOn, cellsChanged);
  u8g.drawStr( 0, 63, charBuf);
}

void drawGeneration(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  char charBuf[10];
  sprintf(charBuf,"%d",generation);
  u8g.drawStr( 0, 64, charBuf);
}
#endif

// Used to draw resetOption when user presses the button
void drawResetOption(int optNum) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  char charBuf[12];
//  sprintf(charBuf,"%d",optNum);
  sprintf(charBuf,"%s",optNum==0?"Random"
                      :optNum==1?"Acorn"
                      :optNum==2?"Pentomino"
                      :optNum==3?"Gosper Gun"
                      :"???");
  u8g.drawStr( 0, 64, charBuf);
}

// Recalculate next iteration of Conway's game of life
void recalculate() {
  
#ifdef STATS_ON
  generation++;
  cellsOn = 0;
  cellsChanged = 0;
#endif

  // copy "row #0" to "row #64" so we don't have to roll over at last row (array now has 65 rows)
  // save "row-1" original row for use in calculations
  // save "row" for use in next row calculations
  
  // Initialize row #64 to row#0 values
  Serial.println(F("Copy Row0 to RowRR"));
  for(int x=0;x<HH;x++) {
    full_bitmap[HH*RR+x] = full_bitmap[x];
  }

//memcpy(&full_bitmap+HH*RR,&full_bitmap,HH);

  // Re-calculate each row
  for(int r=0;r<RR;r++) {
    
    Nrow = (r-1+RR)%RR;
    Srow = r+1; // no %RR, we copied row#0

    // for each row, calculate one byte/column at a time
    for(int c=0;c<HH;c++) {
      Wcol = (c-1+HH)%HH;
      Ecol = (c+1)%HH;

      // N neighbor  = byte# full_bitmap[((r-1)%64)*16+c] // r+1%64
      // S neighbor  = byte# full_bitmap[(r+1)*16+c+16] // do not use %64 on the last row, we copied row#0 so we don't need to roll over
      // E neighbor  = byte# full_bitmap[r*16+((c+1)%16)]  // c+1%16
      // W neighbor  = byte# full_bitmap[r*16+((c-1)%16)]  // c-1%16
      // NE neighbor = byte# full_bitmap[((r-1)%64)*16+((c+1)%16)]  // r-1%64, c+1%16
      // NW neighbor = byte# full_bitmap[((r-1)%64)*16+((c-1)%16)]  // r-1%64, c-1%16
      // SE neighbor = byte# full_bitmap[((r+1)%64)*16+((c+1)%16)]  // r+1%64, c+1%16
      // SW neighbor = byte# full_bitmap[((r+1)%64)*16+((c-1)%16)]  // r+1%64, c-1%16

      // Top row
      NWCell = full_bitmap[Nrow*HH + Wcol];
      NCell  = full_bitmap[Nrow*HH + c];
      NECell = full_bitmap[Nrow*HH + Ecol];

      // Middle row
      WCell  = full_bitmap[r*HH + Wcol];
      XCell  = full_bitmap[r*HH + c];
      ECell  = full_bitmap[r*HH + Ecol];

      // Bottom row
      SWCell = full_bitmap[Srow*HH + Wcol];
      SCell  = full_bitmap[Srow*HH + c];
      SECell = full_bitmap[Srow*HH + Ecol];

      // Start with the values themselves, shift left.  We have 8-bits surrounded by zeroes now
      unsigned int topRow = NCell << 1;
      unsigned int midRow = XCell << 1;
      unsigned int botRow = SCell << 1;
      
      // Set the West border bits
      bitWrite(topRow,9,bitRead(NWCell,0));
      bitWrite(midRow,9,bitRead(WCell,0));
      bitWrite(botRow,9,bitRead(SWCell,0));
      
      // Set the Eest border bits
      bitWrite(topRow,0,bitRead(NECell,7));
      bitWrite(midRow,0,bitRead(ECell,7));
      bitWrite(botRow,0,bitRead(SECell,7));

      // We now have bit 0 from the east, bits 1-8 from the center and bit 9 from the west
      // Initialize new byte with old byte values, we will only flip the changed bits
//      newXCell = XCell; // this byte will be overwritten each time

      // Now start with bit 1 in the middle row and re-calculate bits 1-8
      for(int b=0;b<8;b++) {
        
        // We will always shift the bits over so the bit being calculated is the midRow bit #1
        
        int neighbors = bitRead(topRow,0)
                       +bitRead(topRow,1)
                       +bitRead(topRow,2)
                       +bitRead(midRow,0)
                       +bitRead(midRow,2)
                       +bitRead(botRow,0)
                       +bitRead(botRow,1)
                       +bitRead(botRow,2);

        // Calculate Next Generation
        if(neighbors==3 || (bitRead(midRow,1)==1 && neighbors==2)) {
#ifdef STATS_ON
          if (bitRead(midRow,1) == 0)
            cellsChanged++;
          cellsOn++;
#endif
          bitWrite(newXCell,b,1);
        } else {
#ifdef STATS_ON
          if (bitRead(midRow,1) == 1)
            cellsChanged++;
#endif
          bitWrite(newXCell,b,0);
        }
      
        // Shift right so now the next bit is in midRow bit#1 and we will calculate that one
        topRow = topRow >> 1;
        midRow = midRow >> 1;
        botRow = botRow >> 1;
      } 

      // Copy the new generation to the new_row
      new_row[c] = newXCell;

    }

/*
    // Save the top row that we no longer need to use.  Skip for the very first row since we haven't pushed new_top to new_row yet
    if (r>0) {
      memcpy(&full_bitmap[HH*(r-1)],&new_top,HH);
    }

    // bump new row into the top slot
    memcpy(&new_top,&new_row,HH);
*/
    for(int x=0;x<HH;x++) {
      // Save the top row that we no longer need to use.  Skip for the very first row since we haven't pushed new_top to new_row yet
      if(r>0) {
        full_bitmap[HH*(r-1)+x] = new_top[x];
      }
      // bump new row into the top slot
      new_top[x] = new_row[x];
    }

  }

  // write the last row
  for(int x=0;x<HH;x++) {
    full_bitmap[(RR-1)*HH+x] = new_row[x];
  }
//  memcpy(&full_bitmap+(RR-1)*HH,&new_row,HH);
  // row RR is ignored, it will be over-written next time we refresh

}

void setup(void) {
  pinMode(BUTTON, OUTPUT);
  Serial.begin(115200);
  initialize2();

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
    Serial.println(F("Initialized"));
}

void loop(void) {
//  Serial.println("loop");

  bouncer.update();
  int value = bouncer.read();
  if(value==HIGH) {

    // reset loop
    int resetOption = 0;
    do {
      //Serial.println(resetOption);
      // show option, then wait 1 second.  If button is released we will reset with option 1.  Repeat for each additional reset option

      // picture loop
      u8g.firstPage();
      do {
        drawResetOption(resetOption);
      } while( u8g.nextPage() );

      delay(1000);
      bouncer.update();
      value = bouncer.read();
      if(value==LOW)
        break;
      resetOption++;
      resetOption %= 5;
    } while(true);
    
//  Serial.println("Resetting");
    if(resetOption==0) {
      initialize(255); // random
    } else if(resetOption==1) {
      initializeAcorn();
    } else if(resetOption==2) {
      initializePentomino();
    } else if(resetOption==3) {
      initializeGosperGun();
    } else {
      initialize2();
    }
  //Serial.println(F("Done"));

  }

  // picture loop
  u8g.firstPage();
  do {
    draw();
#ifdef STATS_ON
//    drawCounts();
    drawGeneration();
#endif
  } while( u8g.nextPage() );

  // calculate next generation
  recalculate();

}

