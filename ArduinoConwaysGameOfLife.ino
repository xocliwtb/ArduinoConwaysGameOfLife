#include "U8glib.h"

// Number of 8-bit words horizontal display.
#define HH 16
// Number of 1-pixel rows down on display.
#define RR 64

// U8glib documentation: https://code.google.com/p/u8glib/ 
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST);

// one extra row at the bottom so we don't have to wrap
uint8_t full_bitmap[(RR+1)*HH];

//#define STATS_ON
// stats
#ifdef STATS_ON
int cellsOn;
int cellsChanged;
int generation = 0;
#endif

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

  // Acorn
  full_bitmap[32*HH+8]=32;
  full_bitmap[33*HH+8]=8;
  full_bitmap[34*HH+8]=103;  
*/

  // R-Pentomino
  full_bitmap[32*HH+8]=3;
  full_bitmap[33*HH+8]=6;
  full_bitmap[34*HH+8]=2;  
  
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
  uint8_t new_top[HH];      // new row is stored only when row +1 is already re-calculated
  uint8_t new_row[HH];      // this is the new row we are always working on
  
  // Initialize row #64 to row#0 values
  Serial.println(F("Copy Row0 to RowRR"));
  for(int x=0;x<HH;x++) {
    full_bitmap[HH*RR+x] = full_bitmap[x];
  }

  // Re-calculate each row
  for(int r=0;r<RR;r++) {
    
    uint8_t Nrow = (r-1+RR)%RR;
    uint8_t Srow = r+1; // no %RR, we copied row#0

    // for each row, calculate one byte/column at a time
    for(int c=0;c<HH;c++) {
      uint8_t Wcol = (c-1+HH)%HH;
      uint8_t Ecol = (c+1)%HH;

      // N neighbor  = byte# full_bitmap[((r-1)%64)*16+c] // r+1%64
      // S neighbor  = byte# full_bitmap[(r+1)*16+c+16] // do not use %64 on the last row, we copied row#0 so we don't need to roll over
      // E neighbor  = byte# full_bitmap[r*16+((c+1)%16)]  // c+1%16
      // W neighbor  = byte# full_bitmap[r*16+((c-1)%16)]  // c-1%16
      // NE neighbor = byte# full_bitmap[((r-1)%64)*16+((c+1)%16)]  // r-1%64, c+1%16
      // NW neighbor = byte# full_bitmap[((r-1)%64)*16+((c-1)%16)]  // r-1%64, c-1%16
      // SE neighbor = byte# full_bitmap[((r+1)%64)*16+((c+1)%16)]  // r+1%64, c+1%16
      // SW neighbor = byte# full_bitmap[((r+1)%64)*16+((c-1)%16)]  // r+1%64, c-1%16

      // Top row
      uint8_t NWCell = full_bitmap[Nrow*HH + Wcol];
      uint8_t NCell  = full_bitmap[Nrow*HH + c];
      uint8_t NECell = full_bitmap[Nrow*HH + Ecol];

      // Middle row
      uint8_t WCell  = full_bitmap[r*HH + Wcol];
      uint8_t XCell  = full_bitmap[r*HH + c];
      uint8_t ECell  = full_bitmap[r*HH + Ecol];

      // Bottom row
      uint8_t SWCell = full_bitmap[Srow*HH + Wcol];
      uint8_t SCell  = full_bitmap[Srow*HH + c];
      uint8_t SECell = full_bitmap[Srow*HH + Ecol];

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
      uint8_t newXCell = 0;

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

    // save next gen top row back to matrix
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
  // row RR is ignored, it will be over-written next time we refresh

}

void setup(void) {
  Serial.begin(9600);
//  initialize(255); // random
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
}

void loop(void) {
  // picture loop
  u8g.firstPage();
  do {
    draw();
#ifdef STATS_ON
//    drawCounts();
    drawGeneration();
#endif
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  recalculate();

  //delay(10);
}

