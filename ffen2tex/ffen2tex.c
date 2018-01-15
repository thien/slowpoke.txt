// ffen2tex.c v0.9 - 2015/01/04
// Alain Brobecker
// usage: ffen2tex in.tex out.tex
// Looks inside a .tex file for FFEN boards or CUBES and convert them to pspictures
//2015/01/04: added CUBES.
//2014/05/29: added HInner;VInner;HBorder;VBorder parameters.
//2014/02/17: modified square coloring.
#include <stdio.h>  // file functions, etc...
#include <stdlib.h> // exit(), malloc()
#include <math.h>
#define FALSE             0
#define TRUE              1

int Width,Height,Depth;

//*********************
//****             ****
//**** CUBES INFOS ****
//****             ****
//*********************
// %CUBES W/D/H/.../.../.../.../.../.../.../.../...
// . empty
// 0-9 are plain yellow/red/blue/black/white/green/orange/gray/cyan/magenta
// # for hatched cube
// a-z are for cubes with three different colors defined with, eg:
//    \definecolor{a0}{rgb}{.99, .99, .99} %left
//    \definecolor{a1}{rgb}{.33, .33, .33} %right
//    \definecolor{a2}{rgb}{.66, .66, .66} %up
// %CUBES_WAngle 30 (between [0;90])
// %CUBES_DAngle 30 (between [0;90])
// %CUBES_WSize 0.5
// %CUBES_DSize 0.5
// %CUBES_HSize 0.5
// %CUBES_Rotation 0    (in [0;3] rotation around the vertical axis)
// %CUBES_UpsideDown 0  (1=mirror around the vertical axis, made after rotation)
// %CUBES_BoxBorder 000000000011 (bit 0,1=down, 2,3=backleft, 4,5=backright, 6,7=up, 8,9=frontright, 10,11=frontleft)
// %CUBES_BoxInner 000000000011 (idem, those are binary numbers, bit0 is on the right, no need to put the leftmost zeroes)
// %CUBES_BoxBorderStyle 1 (0=none / 1=solid / 2=dashed / 3=dotted)
// %CUBES_BoxInnerStyle 1 (0=none / 1=solid / 2=dashed / 3=dotted)
// %CUBES_BoxBorderWidth 0.06
// %CUBES_BoxInnerWidth 0.03
// %CUBES_CubesWidth 0.03

#define Cubes_ES -1
#define Cubes_CrossHatched 10
#define Cubes_DefinedColors 11

#define CUBES_MaxSize 9
char CUBES[CUBES_MaxSize][CUBES_MaxSize][CUBES_MaxSize]; // [x][z][y]
char CUBES_Temp[CUBES_MaxSize][CUBES_MaxSize]; //[x][z]
int CUBES_WAngle=30; // between [0;90]
int CUBES_DAngle=30; // between [0;90]
double CUBES_WSize=1.0;
double CUBES_DSize=1.0;
double CUBES_HSize=1.0;
int CUBES_Rotation=0;   // [0;3] rotation around the vertical axis
int CUBES_UpsideDown=0; // 1=mirror around the vertical axis, made after rotation
int CUBES_BoxBorder=3;  // bit 0,1=down, 2,3=backleft, 4,5=backright, 6,7=up, 8,9=frontright, 10,11=frontleft
int CUBES_BoxInner=3;
int CUBES_BoxBorderStyle=1; // 0=none / 1=solid / 2=dashed / 3=dotted
int CUBES_BoxInnerStyle=1;  // 0=none / 1=solid / 2=dashed / 3=dotted
double CUBES_BoxBorderWidth=0.06;
double CUBES_BoxInnerWidth=0.03;
double CUBES_CubesWidth=0.03;

//********************
//****            ****
//**** FFEN INFOS ****
//****            ****
//********************
//Pieces and Modifiers array used are [1..m][1..n], surrounded by outside empty squares.

// Pieces:
//   0=Empty Square (ES)
//   255=Forbidden Square (FS)
//   KkQqRrBbNnPp (standard chess)
//   see ffen2tex_doc for the rest of pieces

//USED:  meta                /123456789'{}
//       as piece mofidiers  ^>-=@* (for *1,*2,*3,*0)
//       as square modifiers $#[]_~
//       for macros          {} (no space inside)
//USED for pieces            abcdefklnopqrstuvxy&"!?|+\%:.,
//REMAINING:                 ghijmwz0();
//NOT TO BE USED:            space ` ($60) et ($7f)
//ACCEPTED WITH ' or '':  (d>='!') && (d<='z')

// Same as FEN but with more possibilities:
// Piece_Modifiers:
//     >  (1 bit)  : piece is between this square and right square
//     ^  (1 bit)  : piece is between this square and up square
//     *1 (2 bits) : rotated 90 degree clockwise
//     *2          : rotated 180 degree clockwise
//     *3          : rotated 270 degree clockwise
//     *0          : not rotated
//     -  (2 bits) : neutral piece=lightgray fill (only for white pieces)
//     =           : gray fill
//     @  (1 bit)  : piece is circled

// Square_Modifiers (can apply on many empty squares):
//     $  (1 bit)  : change square fill property
//     #  (1 bit)  : square is outside board
//     [  (1 bit)  : a borderline is added on the left of the current square
//     ]  (1 bit)  : a borderline is added on the right of the current square
//     _  (1 bit)  : a borderline is added below the current square
//     ~  (1 bit)  : a borderline is added above the current square

#define MaxSize           102 //99 is said in the documentation, but we need extra squares

#define FS 255
#define ES 0
#define MacroCommand      1  //MacroCommand starts with { and ends with }
#define WK                2  //K
#define BK                3  //k
#define WQ                4  //Q
#define BQ                5  //q
#define WR                6  //R
#define BR                7  //r
#define WB                8  //B
#define BB                9  //b
#define WN                10 //N
#define BN                11 //n
#define WP                12 //P
#define BP                13 //p
#define WCircle           14 //C
#define BCircle           15 //c
#define WDame             16 //D
#define BDame             17 //d
#define WCross            18 //X
#define BCross            19 //x
#define ThinCross         20 //:
#define WSquare           21 //S
#define BSquare           22 //s
#define WTriangle         23 //T
#define BTriangle         24 //t
#define WStar             25 //E
#define BStar             26 //e
#define WhiteSmallCircle  29 //o
#define BlackSmallCircle  30 //O
#define BoldCircle        31 //.

#define CharsStart        33
// values 33 to 38 are for !"#$%&
#define DoubleSymbol      39 //double symbol after '', uses MacroOrDoubleSymbolPointer[] and MODSP_Pos
// values 40 to 47 are for ()*+,-./
// values 48 to 48+9=57 are for figures 0 to 9
// values 58 to 64 are for :;<=>?@
// values 65 to 65+25=90 are for letters A to Z
// values 91 to 97 are for [\]^_`
// values 97 to 97+25=122 are for letters a to z
#define CharsEnd          122

#define UpBar             123 //u
#define VerticalBar       124 //|
#define SmallVerticalBar  125 //,
#define UpLeftBar         126 //v
#define VerticalLeftBar   127 //y
#define CrossBar          128 //+
#define HalfDiagonal      129 //U
#define Diagonal          130 // backslash
#define VHalfDiagonals    131 //V
#define YDiagonal         132 //Y
#define CrossDiagonals    133 //%
#define UpLeftArrow       134 //A
#define UpArrow           135 //a
#define Club              136 //?
#define Diamond           137 //"
#define Heart             138 //&
#define Spade             139 //!
#define WhiteFill         140 //f, in fact used for lightgray/gray fill
#define CrossHatchFill    141 //F
#define HLineFill         142 //l
#define VLineFill         143 //L
#define LessThan          144 //<

#define Piece_Numbers 18
#define Piece_UppercaseLetter Piece_Numbers+10
#define Piece_LowercaseLetter Piece_UppercaseLetter+26

#define Square_FillBit 1     //!!! Must be on bit 0
#define Square_OutsideBit 2
#define Square_LeftBorderBit 4   //[
#define Square_RightBorderBit 8  //]
#define Square_BelowBorderBit 16 //_
#define Square_AboveBorderBit 32 //_


#define Piece_RotationBits 1 //2 bits
#define Piece_FillBit 4      //2 bits
#define Piece_RightBit 16    //1 bit
#define Piece_UpBit 32       //1 bit
#define Piece_CircledBit 64  //1 bit

FILE *filep;
unsigned char *In,*Out; // data
int InLength,OutLength;

unsigned char Pieces[MaxSize][MaxSize];
unsigned char Piece_Modifiers[MaxSize][MaxSize];
unsigned char Square_Modifiers[MaxSize][MaxSize];
unsigned char BorderInnerType[MaxSize];
unsigned char Coloring[MaxSize][MaxSize];

int MODSP_Pos;
int MacroOrDoubleSymbolPointer[MaxSize*MaxSize*2];
//??? Bug if i give them the same name???

//**********************
//**** FFEN OPTIONS ****
//**********************
double FFEN_SquareSize=0.5;
double FFEN_ThinLineWidth=0.03;
double FFEN_BoldLineWidth=0.15;
int FFEN_Fill=2; // 0=none / 1=odd / 2=even (chess) / 3=full
int FFEN_FillStyle=0; // 0=lightgray / 1=gray / 2=black / 3=vlines / 4=hlines / 5=crosshatch / 6=centered dot
int FFEN_HInnerStyle=0; // 0=none / 1=solid / 2=dashed / 3=dotted
int FFEN_VInnerStyle=0;
double FFEN_HInnerWidth=0.03;
double FFEN_VInnerWidth=0.03;
int FFEN_HBorderStyle=1; // 0=none / 1=solid / 2=dashed / 3=dotted
int FFEN_VBorderStyle=1;
double FFEN_HBorderWidth=0.06;
double FFEN_VBorderWidth=0.06;
int FFEN_BorderUp=1; // 0 (off)/1 (on)
int FFEN_BorderLeft=1; // 0 (off)/1 (on)
int FFEN_BorderDown=1; // 0 (off)/1 (on)
int FFEN_BorderRight=1; // 0 (off)/1 (on)
//FFEN_BorderAll // !!! works but affects all 4 borders variables, 0 (off)/1 (on)
int FFEN_PieceCount=0; //0 (off)/1 (W+B)/2 (?+?=W+B)
int NbWhitePieces;
int NbBlackPieces;
int NbOtherPieces;
double FFEN_CornersRadius=0.0; //0.0 (off) / value


//**** SearchNext routine, brute force version *****************************
//* To speed up this routine, copy beforehand the string to search at the  *
//* end of the text (you must allocate a bit more memory) and remove the   *
//* TextPos+StringLength<=TextLength condition in main loop. The routine   *
//* will always find a string, once this is done, compare Pos+Length and   *
//* return -1 if it is the one we added!                                   *
int SearchNext(unsigned char *String,int StringLength,
               unsigned char *Text,int TextLength,int TextPos) {
  int NbMatch=0;
  while((NbMatch<StringLength) && (TextPos+StringLength<=TextLength)) {
    if(Text[TextPos+NbMatch]==String[NbMatch]) {
      NbMatch++;
    } else {
      NbMatch=0;
      TextPos++;
    }
  }
  if(NbMatch==StringLength) {
    return(TextPos);
  } else {
    return(-1);
  }
}


//**** GetLineNumber ****
int GetLineNumber(unsigned char *Text,int TextPos) {
  int n,t;
  t=0;
  n=1;
  while(t<=TextPos) {
    if(Text[t++]==10) { n++; }
  }
  return(n);
}

//**** ReadInteger ****
int ReadInteger(unsigned char *Text,int TextLength,int TextPos) {
  int n,sign,end;
  unsigned char c;
  //skip spaces
  while((Text[TextPos]==' ') && (TextPos<=TextLength)) { TextPos++; }
  n=0;
  sign=0;
  end=FALSE;
  while((end==FALSE) && (TextPos<=TextLength)) {
    c=Text[TextPos++];
    if(c<=' ') {
      end=TRUE;
    } else if((c=='-') && (sign==0)) {
      sign=-1;
    } else if((c>='0') && (c<='9')) {
      n=n*10+c-'0';
      if(sign==0) { sign=1; }
    } else {
      printf("error: not a valid integer at line %d\n",GetLineNumber(Text,TextPos));
      fclose(filep);
      exit(-1);
    }
  }
  return(n*sign);
}

//**** ReadBinary ****
int ReadBinary(unsigned char *Text,int TextLength,int TextPos) {
  int n,end;
  unsigned char c;
  //skip spaces
  while((Text[TextPos]==' ') && (TextPos<=TextLength)) { TextPos++; }
  n=0;
  end=FALSE;
  while((end==FALSE) && (TextPos<=TextLength)) {
    c=Text[TextPos++];
    if(c<=' ') {
      end=TRUE;
    } else if((c>='0') && (c<='1')) {
      n=n*2+c-'0';
    } else {
      printf("error: not a valid binary number at line %d\n",GetLineNumber(Text,TextPos));
      fclose(filep);
      exit(-1);
    }
  }
  return(n);
}

//**** ReadFloat ****
double ReadFloat(unsigned char *Text,int TextLength,int TextPos) {
  int n,sign,divisor,end;
  unsigned char c;
  double f;
  //skip spaces
  while((Text[TextPos]==' ') && (TextPos<=TextLength)) { TextPos++; }
  n=0;
  sign=0;
  divisor=0;
  end=FALSE;
  while((end==FALSE) && (TextPos<=TextLength)) {
    c=Text[TextPos++];
    if(c<=' ') {
      end=TRUE;
    } else if((c=='-') && (sign==0)) {
      sign=-1;
    } else if((c=='.') && (divisor==0)) {
      divisor=1;
    } else if((c>='0') && (c<='9')) {
      n=n*10+c-'0';
      if(sign==0) { sign=1; }
      if(divisor!=0) { divisor=divisor*10; }
    } else {
      printf("error: not a valid float at line %d\n",GetLineNumber(Text,TextPos));
      fclose(filep);
      exit(-1);
    }
  }
  n=n*sign;
  if(divisor==0) {
    f=(double) n;
  } else {
    f=((double) (n))/((double) (divisor));
  }
  return(f);
}

//*******************
//****           ****
//**** ReadCUBES ****
//****           ****
//*******************
//OUT:     width,depth,height
//         CUBES[1..width][1..depth][1..height]
//RETURNS: index after text

// %CUBES Width/Depth/Height/.../.../.../.../.../.../.../.../...
// . empty
// 0-9 are plain yellow/red/blue/black/white/green/orange/gray/cyan/magenta
// # for hatched cube
// a-z are for cubes with three different colors defined with, eg:
//    \definecolor{a0}{rgb}{.99, .99, .99} %left
//    \definecolor{a1}{rgb}{.33, .33, .33} %right
//    \definecolor{a2}{rgb}{.66, .66, .66} %up
void ReadCUBES(unsigned char *Text,int TextLength,int TextPos) {
  int x,z,y;
  unsigned char c;

  //skip spaces
  while((Text[TextPos]==' ') && (TextPos<=TextLength)) { TextPos++; }
  //Read Width, Depth, Height
  Width=Text[TextPos++]-'0';
  c=Text[TextPos++];
  if((Width<1) || (Width>9) || (c!='/')) { printf("error: invalid width at line %d\n",GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
  Depth=Text[TextPos++]-'0';
  c=Text[TextPos++];
  if((Depth<1) || (Depth>9) || (c!='/')) { printf("error: invalid depth at line %d\n",GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
  Height=Text[TextPos++]-'0';
  c=Text[TextPos++];
  if((Height<1) || (Height>9) || (c!='/')) { printf("error: invalid height at line %d\n",GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
//!!!  printf("%dx%dx%d\n",Width,Depth,Height);
  //
  for(y=0;y<Height;y++) {
    for(z=0;z<Depth;z++) {
      for(x=0;x<Width;x++) {
        c=Text[TextPos++];
        if(c=='/') { printf("error: / incorrectly placed at line %d\n",GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
        else if(c=='.') { CUBES[x][z][y]=Cubes_ES; }
        else if((c>='0') && (c<='9')) { CUBES[x][z][y]=c-'0'; }
        else if(c=='#') { CUBES[x][z][y]=Cubes_CrossHatched; }
        else if((c>='a') && (c<='z')) { CUBES[x][z][y]=Cubes_CrossHatched+1+c-'a'; }
        else { printf("error: %c is not a valid CUBES at line %d\n",c,GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
      }
      c=Text[TextPos++];
      if(((y!=Height-1) || (z!=Depth-1) || (x!=Width)) && (c!='/')) { printf("error: missing / at line %d\n",GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
    }
  }
}

//**** DEBUG: PrintCUBES ****
void PrintCUBES() {
  int x,z,y;
  for(y=1;y<=Height;y++) {
    for(z=0;z<Depth;z++) {
      for(x=1;x<=Width;x++) {
        printf("%2d,",CUBES[x][z][y]);
      }
      printf("\n");
    }
    printf("\n");
  }
}



void PSLine(double Width,int Style,double x1, double y1, double x2, double y2) {
  fprintf(filep,"\\psline[linewidth=%f",Width);
  switch(Style) {
    case 2:fprintf(filep,",linestyle=dashed"); break; 
    case 3:fprintf(filep,",linestyle=dotted"); break;
  }
  fprintf(filep,"]{c-c}(%f,%f)(%f,%f)\n",x1,y1,x2,y2);
}

 
//**** MakePsPictureFromCUBES ****
/*      /|\
   D(z)/ | \W(x)
      /  |  \
     |\ /B\ /|
     | \   / |H(y)   S=Size
     |/ \ / \|
     A\  |  /C
       \ | /
        \|/
         D
*/
// 1: Rotate if need be
// 2: Upside Down if need be
// 3: Compute coordinates
// 4: Bottom, BackLeft, BackRight borders
// 5: Cubes
// 6: Top, FrontRight,FrontLeft borders
void MakePsPictureFromCUBES(unsigned char *Text,int TextLength) {
  double cosW,sinW,cosD,sinD;
  double xA,yA,xB,yB,xC,yC,xD,yD,xx,yy;
  int x,y,z,value;

  //**** 1: Rotate
  value=0;
  while(value<CUBES_Rotation) {
    z=Depth;
    Depth=Width;
    Width=z;    
    for(y=Height-1;y>=0;y--) {
      for(z=0;z<=Depth-1;z++) {
        for(x=0;x<=Width-1;x++) {
          CUBES_Temp[x][z]=CUBES[z][Width-1-x][y];
        }
      }
      for(z=0;z<=Depth-1;z++) {
        for(x=0;x<=Width-1;x++) {
          CUBES[x][z][y]=CUBES_Temp[x][z];
        }
      }
    }
    value++;
  }
  
  //**** 2: Upside down
  if(CUBES_UpsideDown==1) {
    for(y=0;y<Height/2;y++) {
      for(z=0;z<=Depth-1;z++) {
        for(x=0;x<=Width-1;x++) {
          value=CUBES[x][z][y];
          CUBES[x][z][y]=CUBES[x][z][Height-1-y];
          CUBES[x][z][Height-1-y]=value;
        }
      }
    }
  }
  
  //**** 3: Compute coordinates
  cosW=cos(CUBES_WAngle*0.017453292); // *pi/180
  sinW=sin(CUBES_WAngle*0.017453292);
  cosD=cos(CUBES_DAngle*0.017453292);
  sinD=sin(CUBES_DAngle*0.017453292);
  xA=0;
  yA=Width*sinW*CUBES_WSize;
  xD=Width*cosW*CUBES_WSize;
  yD=0;
  xB=Depth*cosD*CUBES_DSize;
  yC=Depth*sinD*CUBES_DSize;
  yB=yA+yC;
  xC=xB+xD;
  fprintf(filep,"\\begin{pspicture}(%f,%f)\n",xC,yB+Height*CUBES_HSize);
  
  //**** 4: Bottom, BackLeft, BackRight borders
// %CUBES_BoxBorder 3 (bit 0&1=Bottom, bit 2&3=BackLeft, bit 4&5=BackRight, bit 6&7=Top, bit 8&9=FrontRight, bit 10&11=FrontLeft)
// %CUBES_BoxInner 3 (idem)
// %CUBES_BoxBorderStyle 1 (0=none / 1=solid / 2=dashed / 3=dotted)
// %CUBES_BoxInnerStyle 1 (0=none / 1=solid / 2=dashed / 3=dotted)
// %CUBES_BoxBorderWidth 0.1
// %CUBES_BoxInnerWidth 0.05
  if(CUBES_BoxBorderStyle!=0) {
    //Bottom
    if((CUBES_BoxBorder & 1)!=0) {
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA,xB,yB);
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xD,yD,xC,yC);
    }
    if((CUBES_BoxBorder & 2)!=0) {
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA,xD,yD);
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xB,yB,xC,yC);
    }
    //BackLeft
    if((CUBES_BoxBorder & 4)!=0) {
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA,xA,yA+Height*CUBES_HSize);
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xB,yB,xB,yB+Height*CUBES_HSize);
    }
    if((CUBES_BoxBorder & 8)!=0) {
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA+Height*CUBES_HSize,xB,yB+Height*CUBES_HSize);
      if((CUBES_BoxBorder & 1)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA,xB,yB);
      }
    }
    //BakcRight
    if((CUBES_BoxBorder & 16)!=0) {
      if((CUBES_BoxBorder & 4)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xB,yB,xB,yB+Height*CUBES_HSize);
      }
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xC,yC,xC,yC+Height*CUBES_HSize);
    }
    if((CUBES_BoxBorder & 32)!=0) {
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xB,yB+Height*CUBES_HSize,xC,yC+Height*CUBES_HSize);
      if((CUBES_BoxBorder & 2)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xB,yB,xC,yC);
      }
    }
  }
  
  if(CUBES_BoxInnerStyle!=0) {
    //Bottom
    if((CUBES_BoxInner & 1)!=0) {
      for(x=1;x<Width;x++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA+x*cosW*CUBES_WSize,yA-x*sinW*CUBES_WSize,xB+x*cosW*CUBES_WSize,yB-x*sinW*CUBES_WSize);
      }
    }
    if((CUBES_BoxInner & 2)!=0) {
      for(z=1;z<Depth;z++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA+z*cosD*CUBES_DSize,yA+z*sinD*CUBES_DSize,xD+z*cosD*CUBES_DSize,yD+z*sinD*CUBES_DSize);
      }
    }
    //BackLeft
    if((CUBES_BoxInner & 4)!=0) {
      for(z=1;z<Depth;z++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA+z*cosD*CUBES_DSize,yA+z*sinD*CUBES_DSize,xA+z*cosD*CUBES_DSize,yA+z*sinD*CUBES_DSize+Height*CUBES_HSize);
      }
    }
    if((CUBES_BoxInner & 8)!=0) {
      for(y=1;y<Height;y++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA,yA+y*CUBES_HSize,xB,yB+y*CUBES_HSize);
      }
    }
    //BackRight
    if((CUBES_BoxInner & 16)!=0) {
      for(x=1;x<Width;x++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xB+x*cosW*CUBES_WSize,yB-x*sinW*CUBES_WSize,xB+x*cosW*CUBES_WSize,yB-x*sinW*CUBES_WSize+Height*CUBES_HSize);
      }
    }
    if((CUBES_BoxInner & 32)!=0) {
      for(y=1;y<Height;y++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xB,yB+y*CUBES_HSize,xC,yC+y*CUBES_HSize);
      }
    }
  }
  
  //**** 5: Cubes
// . empty
// 0-9 are plain yellow/red/blue/black/white/green/orange/gray/cyan/magenta
// # for hatched cube
// a-z are for cubes with three different colors defined with, eg:
//    \definecolor{a0}{rgb}{.99, .99, .99} %left
//    \definecolor{a1}{rgb}{.33, .33, .33} %right
//    \definecolor{a2}{rgb}{.66, .66, .66} %up
// %CUBES_CubesWidth 0.05
  for(y=Height-1;y>=0;y--) {
    for(z=0;z<=Depth-1;z++) {
      for(x=0;x<=Width-1;x++) {
        value=CUBES[x][z][y];
        if(value!=Cubes_ES) {
          xx=xD-(Width-1-x)*cosW*CUBES_WSize+(Depth-1-z)*cosD*CUBES_DSize;
          yy=yD+(Width-1-x)*sinW*CUBES_WSize+(Depth-1-z)*sinD*CUBES_DSize+(Height-1-y)*CUBES_HSize;
          //**** Plain cubes
          if(value<10) {
            fprintf(filep,"\\pspolygon[linewidth=%f,linearc=%f,fillstyle=solid,fillcolor=",CUBES_CubesWidth,0.02*CUBES_WSize);
            switch(value) {
              case 0: fprintf(filep,"yellow"); break;
              case 1: fprintf(filep,"red"); break;
              case 2: fprintf(filep,"blue"); break;
              case 3: fprintf(filep,"black,linecolor=white"); break;
              case 4: fprintf(filep,"white"); break;
              case 5: fprintf(filep,"green"); break;
              case 6: fprintf(filep,"orange"); break;
              case 7: fprintf(filep,"lightgray"); break;
              case 8: fprintf(filep,"cyan"); break;
              case 9: fprintf(filep,"magenta"); break;
            }
            fprintf(filep,"](%f,%f)(%f,%f)(%f,%f)(%f,%f)(%f,%f)(%f,%f)\n",xx,yy,xx-cosW*CUBES_WSize,yy+sinW*CUBES_WSize,xx-cosW*CUBES_WSize,yy+sinW*CUBES_WSize+CUBES_HSize,xx-cosW*CUBES_WSize+cosD*CUBES_DSize,yy+sinW*CUBES_WSize+CUBES_HSize+sinD*CUBES_DSize,xx+cosD*CUBES_DSize,yy+CUBES_HSize+sinD*CUBES_DSize,xx+cosD*CUBES_DSize,yy+sinD*CUBES_DSize);
            if(value==3) {
              fprintf(filep,"\\psline[linewidth=%f,linecolor=white](%f,%f)(%f,%f)(%f,%f) \\psline[linewidth=%f,linecolor=white](%f,%f)(%f,%f)\n",CUBES_CubesWidth,xx,yy,xx,yy+CUBES_HSize,xx-cosW*CUBES_WSize,yy+CUBES_HSize+sinW*CUBES_WSize,CUBES_CubesWidth,xx,yy+CUBES_HSize,xx+cosD*CUBES_DSize,yy+CUBES_HSize+sinD*CUBES_DSize);
            } else {
              fprintf(filep,"\\psline[linewidth=%f](%f,%f)(%f,%f)(%f,%f) \\psline[linewidth=%f](%f,%f)(%f,%f)\n",CUBES_CubesWidth,xx,yy,xx,yy+CUBES_HSize,xx-cosW*CUBES_WSize,yy+CUBES_HSize+sinW*CUBES_WSize,CUBES_CubesWidth,xx,yy+CUBES_HSize,xx+cosD*CUBES_DSize,yy+CUBES_HSize+sinD*CUBES_DSize);
            }
          //**** Crosshatched cubes
          } else if(value==Cubes_CrossHatched) {
//!!!!!!!            
          //**** Cubes with defined triples of colors
          } else {
            if((z==Depth-1) || (CUBES[x][z+1][y]==-1)) { fprintf(filep,"\\pspolygon[linewidth=%f,linearc=%f,fillstyle=solid,fillcolor=%c0](%f,%f)(%f,%f)(%f,%f)(%f,%f)\n",CUBES_CubesWidth,0.02*CUBES_WSize,value-Cubes_DefinedColors+'a',xx,yy,xx-cosW*CUBES_WSize,yy+sinW*CUBES_WSize,xx-cosW*CUBES_WSize,yy+sinW*CUBES_WSize+CUBES_HSize,xx,yy+CUBES_HSize); }
            if((x==Width-1) || (CUBES[x+1][z][y]==-1)) { fprintf(filep,"\\pspolygon[linewidth=%f,linearc=%f,fillstyle=solid,fillcolor=%c1](%f,%f)(%f,%f)(%f,%f)(%f,%f)\n",CUBES_CubesWidth,0.02*CUBES_WSize,value-Cubes_DefinedColors+'a',xx,yy,xx+cosD*CUBES_DSize,yy+sinD*CUBES_DSize,xx+cosD*CUBES_DSize,yy+sinD*CUBES_DSize+CUBES_HSize,xx,yy+CUBES_HSize); }
            if((y==0) || (CUBES[x][z][y-1]==-1)) { fprintf(filep,"\\pspolygon[linewidth=%f,linearc=%f,fillstyle=solid,fillcolor=%c2](%f,%f)(%f,%f)(%f,%f)(%f,%f)\n",CUBES_CubesWidth,0.02*CUBES_WSize,value-Cubes_DefinedColors+'a',xx,yy+CUBES_HSize,xx-cosW*CUBES_WSize,yy+sinW*CUBES_WSize+CUBES_HSize,xx-cosW*CUBES_WSize+cosD*CUBES_DSize,yy+sinW*CUBES_WSize+sinD*CUBES_DSize+CUBES_HSize,xx+cosD*CUBES_DSize,yy+sinD*CUBES_DSize+CUBES_HSize); }
          }
        }
      }
    }
  }
  
  //**** 6: Top, FrontRight,FrontLeft borders
/*      /|\
   D(z)/ | \W(x)
      /  |  \
     |\ /B\ /|
     | \   / |H(y)   S=Size
     |/ \ / \|
     A\  |  /C
       \ | /
        \|/
         D
*/
  if(CUBES_BoxBorderStyle!=0) {
    //Top
    if((CUBES_BoxBorder & 64)!=0) {
      if((CUBES_BoxBorder & 8)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA+Height*CUBES_HSize,xB,yB+Height*CUBES_HSize);
      }
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xD,yD+Height*CUBES_HSize,xC,yC+Height*CUBES_HSize);
    }
    if((CUBES_BoxBorder & 128)!=0) {
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA+Height*CUBES_HSize,xD,yD+Height*CUBES_HSize);
      if((CUBES_BoxBorder & 32)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xB,yB+Height*CUBES_HSize,xC,yC+Height*CUBES_HSize);
      }
    }
    //FrontRight
    if((CUBES_BoxBorder & 256)!=0) {
      if((CUBES_BoxBorder & 16)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xC,yC,xC,yC+Height*CUBES_HSize);
      }
      PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xD,yD,xD,yD+Height*CUBES_HSize);
    }
    if((CUBES_BoxBorder & 512)!=0) {
      if((CUBES_BoxBorder & 1)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xD,yD,xC,yC);
      }
      if((CUBES_BoxBorder & 64)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xD,yD+Height*CUBES_HSize,xC,yC+Height*CUBES_HSize);
      }
    }
    //FrontLeft
    if((CUBES_BoxBorder & 1024)!=0) {
      if((CUBES_BoxBorder & 4)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA,xA,yA+Height*CUBES_HSize);
      }
      if((CUBES_BoxBorder & 256)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xD,yD,xD,yD+Height*CUBES_HSize);
      }
    }
    if((CUBES_BoxBorder & 2048)!=0) {
      if((CUBES_BoxBorder & 2)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA,xD,yD);
      }
      if((CUBES_BoxBorder & 128)==0) {
        PSLine(CUBES_BoxBorderWidth,CUBES_BoxBorderStyle,xA,yA+Height*CUBES_HSize,xD,yD+Height*CUBES_HSize);
      }
    }
  }
  
  //Top
  if(CUBES_BoxInnerStyle!=0) {
    if((CUBES_BoxInner & 64)!=0) {
      for(x=1;x<Width;x++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA+x*cosW*CUBES_WSize,yA-x*sinW*CUBES_WSize+Height*CUBES_HSize,xB+x*cosW*CUBES_WSize,yB-x*sinW*CUBES_WSize+Height*CUBES_HSize);
      }
    }
    if((CUBES_BoxInner & 128)!=0) {
      for(z=1;z<Depth;z++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA+z*cosD*CUBES_DSize,yA+z*sinD*CUBES_DSize+Height*CUBES_HSize,xD+z*cosD*CUBES_DSize,yD+z*sinD*CUBES_DSize+Height*CUBES_HSize);
      }
    }
    //FrontRight
    if((CUBES_BoxInner & 256)!=0) {
      for(z=1;z<Depth;z++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xD+z*cosD*CUBES_DSize,yD+z*sinD*CUBES_DSize,xD+z*cosD*CUBES_DSize,yD+z*sinD*CUBES_DSize+Height*CUBES_HSize);
      }
    }
    if((CUBES_BoxInner & 512)!=0) {
      for(y=1;y<Height;y++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xD,yD+y*CUBES_HSize,xC,yC+y*CUBES_HSize);
      }
    }
    //FrontLeft
    if((CUBES_BoxInner & 1024)!=0) {
      for(x=1;x<Width;x++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA+x*cosW*CUBES_WSize,yA-x*sinW*CUBES_WSize,xA+x*cosW*CUBES_WSize,yA-x*sinW*CUBES_WSize+Height*CUBES_HSize);
      }
    }
    if((CUBES_BoxInner & 2048)!=0) {
      for(y=1;y<Height;y++) {
        PSLine(CUBES_BoxInnerWidth,CUBES_BoxInnerStyle,xA,yA+y*CUBES_HSize,xD,yD+y*CUBES_HSize);
      }
    }
  }
  fprintf(filep,"\\end{pspicture}\n");
}



//******************
//****          ****
//**** ReadFFEN ****
//****          ****
//******************
//OUT:     width,height
//         Pieces[1..width][1..height]
//         Piece_Modifiers[1..width][1..height]
//         Square_Modifiers[1..width][1..height]
//RETURNS: index after text

unsigned char Piece_Modifier,Square_Modifier;

void InsertPiece(int x,int y, unsigned char piece) {
  Pieces[x][y]=piece;
  Piece_Modifiers[x][y]=Piece_Modifier;
  Piece_Modifier=0;
  Square_Modifiers[x][y]=Square_Modifier;
  Square_Modifier=0;
}


void ReadFFEN(unsigned char *Text,int TextLength,int TextPos) {
  int x,y,end,imbrication;
  unsigned char c,d;

  //skip spaces
  while((Text[TextPos]==' ') && (TextPos<=TextLength)) { TextPos++; }
  //clear board
  for(y=0;y<MaxSize;y++) {
    for(x=0;x<MaxSize;x++) {
      Pieces[x][y]=FS;
      Piece_Modifiers[x][y]=0;
      Square_Modifiers[x][y]=Square_OutsideBit; // all squares outside
    }
  }

  end=FALSE;
  Width=0;
  Height=1;
  Piece_Modifier=0;
  Square_Modifier=0;
  x=0;

  do {
    c=Text[TextPos++];
    if(c<=' ') {
      if(Width==0) {
        Width=x;
      } else if (Width!=x) {
        printf("error: different widths at line %d\n",GetLineNumber(Text,TextPos));
        fclose(filep);
        exit(-1);
      }
      end=TRUE;
    } else if((c>='1') && (c<='9')) {
      c=c-'0';
      do {
        x++;
        Pieces[x][Height]=ES;
        Square_Modifiers[x][Height]=Square_Modifier;
        Piece_Modifiers[x][Height]=0;
        c--;
      } while(c>0);
      Piece_Modifier=0;
      Square_Modifier=0;
    } else {
      switch(c) {
        //New FFEN line
        case '/':
          if(Width==0) {
            Width=x;
          } else if (Width!=x) {
            printf("error: different widths at line %d\n",GetLineNumber(Text,TextPos));
            fclose(filep);
            exit(-1);
          }
          Height++;
          x=0;
          break;
        //Square Modifiers
        case '#': Square_Modifier=Square_Modifier | Square_OutsideBit; break;
        case '$': Square_Modifier=Square_Modifier | Square_FillBit; break;
        case '[': Square_Modifier=Square_Modifier | Square_LeftBorderBit; break;
        case ']': Square_Modifier=Square_Modifier | Square_RightBorderBit; break;
        case '_': Square_Modifier=Square_Modifier | Square_BelowBorderBit; break;
        case '~': Square_Modifier=Square_Modifier | Square_AboveBorderBit; break;
        //Piece Modifiers
        case '>': Piece_Modifier=Piece_Modifier | Piece_RightBit; break;
        case '^': Piece_Modifier=Piece_Modifier | Piece_UpBit; break;
        case '-': Piece_Modifier=Piece_Modifier | 1*Piece_FillBit; break;
        case '=': Piece_Modifier=Piece_Modifier | 2*Piece_FillBit; break;
        case '@': Piece_Modifier=Piece_Modifier | 1*Piece_CircledBit; break;
        case '*':
          switch(Text[TextPos++]) {
            case '0': break;
            case '1': Piece_Modifier=Piece_Modifier | 1*Piece_RotationBits; break;
            case '2': Piece_Modifier=Piece_Modifier | 2*Piece_RotationBits; break;
            case '3': Piece_Modifier=Piece_Modifier | 3*Piece_RotationBits; break;
            default:
              printf("error: invalid rotation at line %d\n",GetLineNumber(Text,TextPos));
              fclose(filep);
              exit(-1);
          }
          break;
        // Pieces
        case 'K': x++; InsertPiece(x,Height,WK); NbWhitePieces++; break;
        case 'k': x++; InsertPiece(x,Height,BK); NbBlackPieces++; break;
        case 'Q': x++; InsertPiece(x,Height,WQ); NbWhitePieces++; break;
        case 'q': x++; InsertPiece(x,Height,BQ); NbBlackPieces++; break;
        case 'R': x++; InsertPiece(x,Height,WR); NbWhitePieces++; break;
        case 'r': x++; InsertPiece(x,Height,BR); NbBlackPieces++; break;
        case 'B': x++; InsertPiece(x,Height,WB); NbWhitePieces++; break;
        case 'b': x++; InsertPiece(x,Height,BB); NbBlackPieces++; break;
        case 'N': x++; InsertPiece(x,Height,WN); NbWhitePieces++; break;
        case 'n': x++; InsertPiece(x,Height,BN); NbBlackPieces++; break;
        case 'P': x++; InsertPiece(x,Height,WP); NbWhitePieces++; break;
        case 'p': x++; InsertPiece(x,Height,BP); NbBlackPieces++; break;
        case 'C': x++; InsertPiece(x,Height,WCircle); NbWhitePieces++; break;
        case 'c': x++; InsertPiece(x,Height,BCircle); NbBlackPieces++; break;
        case 'D': x++; InsertPiece(x,Height,WDame); NbWhitePieces++; break;
        case 'd': x++; InsertPiece(x,Height,BDame); NbBlackPieces++; break;
        case 'X': x++; InsertPiece(x,Height,WCross); NbWhitePieces++; break;
        case 'x': x++; InsertPiece(x,Height,BCross); NbBlackPieces++; break;
        case 'S': x++; InsertPiece(x,Height,WSquare); NbWhitePieces++; break;
        case 's': x++; InsertPiece(x,Height,BSquare); NbBlackPieces++; break;
        case 'T': x++; InsertPiece(x,Height,WTriangle); NbWhitePieces++; break;
        case 't': x++; InsertPiece(x,Height,BTriangle); NbBlackPieces++; break;
        case 'E': x++; InsertPiece(x,Height,WStar); NbWhitePieces++; break;
        case 'e': x++; InsertPiece(x,Height,BStar); NbBlackPieces++; break;
        case '?': x++; InsertPiece(x,Height,Club); break;
        case '"': x++; InsertPiece(x,Height,Diamond); break;
        case '&': x++; InsertPiece(x,Height,Heart); break;
        case '!': x++; InsertPiece(x,Height,Spade); break;
        case 'O': x++; InsertPiece(x,Height,WhiteSmallCircle); NbWhitePieces++; break;
        case 'o': x++; InsertPiece(x,Height,BlackSmallCircle); NbBlackPieces++; break;
        case '.': x++; InsertPiece(x,Height,BoldCircle); break;
        case ':': x++; InsertPiece(x,Height,ThinCross); break;
        case 'u': x++; InsertPiece(x,Height,UpBar); break;
        case '|': x++; InsertPiece(x,Height,VerticalBar); break;
        case ',': x++; InsertPiece(x,Height,SmallVerticalBar); break;
        case 'v': x++; InsertPiece(x,Height,UpLeftBar); break;
        case 'y': x++; InsertPiece(x,Height,VerticalLeftBar); break;
        case '+': x++; InsertPiece(x,Height,CrossBar); break;
        case 'U': x++; InsertPiece(x,Height,HalfDiagonal); break;
        case '\\': x++; InsertPiece(x,Height,Diagonal); break;
        case 'V': x++; InsertPiece(x,Height,VHalfDiagonals); break;
        case 'Y': x++; InsertPiece(x,Height,YDiagonal); break;
        case '%': x++; InsertPiece(x,Height,CrossDiagonals); break;
        case 'A': x++; InsertPiece(x,Height,UpLeftArrow); break;
        case 'a': x++; InsertPiece(x,Height,UpArrow); break;
        case 'f': x++; InsertPiece(x,Height,WhiteFill); break;
        case 'F': x++; InsertPiece(x,Height,CrossHatchFill); break;
        case 'l': x++; InsertPiece(x,Height,HLineFill); break;
        case 'L': x++; InsertPiece(x,Height,VLineFill); break;
        case '<': x++; InsertPiece(x,Height,LessThan); break;
        case 39: //' for Symbols
          d=Text[TextPos++];
          // double Symbol?
          if(d==39) {
            x++;
            InsertPiece(x,Height,39);
            MacroOrDoubleSymbolPointer[MODSP_Pos]=TextPos;
            MODSP_Pos++;
            d=Text[TextPos++];
            if((d<'!') || (d>'z')) { printf("error: %c is not a valid char at line %d\n",d,GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
            d=Text[TextPos++];
            if((d<'!') || (d>'z')) { printf("error: %c is not a valid char at line %d\n",d,GetLineNumber(Text,TextPos)); fclose(filep); exit(-1); }
          } else if((d>='!') && (d<='z')) {
            x++;
            InsertPiece(x,Height,d);
          } else {
            printf("error: %c is not a valid char at line %d\n",d,GetLineNumber(Text,TextPos));
            fclose(filep);
            exit(-1);
          }
          break;
        case '{': //start of a macro
          x++; InsertPiece(x,Height,MacroCommand);
          MacroOrDoubleSymbolPointer[MODSP_Pos]=TextPos;
          MODSP_Pos++;
          imbrication=1;
          //jump to the end of the macro, ie after the '}' related to this '{'
          do{
            d=Text[TextPos++];
            if(d<=' ') {
              printf("error: macro not ended at line %d\n",GetLineNumber(Text,TextPos-1));
              fclose(filep);
              exit(-1);
            }
            if(d=='{') { imbrication++; }
            if(d=='}') { imbrication--; }
          } while((d!='}') || (imbrication!=0));
          break;
        case '}': //normally we don't see this, so there's an error
          printf("error: abnormal end of macro at line %d\n",GetLineNumber(Text,TextPos));
          fclose(filep);
          exit(-1);
          break;
        default:
          printf("error: %c is not a valid piece at line %d\n",c,GetLineNumber(Text,TextPos));
          fclose(filep);
          exit(-1);
      }
    }
  } while((end==FALSE) && (TextPos<TextLength));
}


//**** DEBUG: PrintFFEN ****
void PrintFFEN() {
  int x,y;
  for(y=1;y<=Height;y++) {
    for(x=1;x<=Width;x++) {
      switch(Pieces[x][y]) {
        case ES: fprintf(filep,"."); break;
        case WCircle: fprintf(filep,"c"); break;
        case BCircle: fprintf(filep,"C"); break;
        default:
          fprintf(filep,"?"); break;
      }
    }
    fprintf(filep,"\n");
  }
  fprintf(filep,"\n\n");
  for(y=1;y<=Height;y++) {
    for(x=1;x<=Width;x++) {
      switch(Square_Modifiers[x][y]) {
        case 0: fprintf(filep,"#"); break;
        default: fprintf(filep," "); break;
      }
    }
    fprintf(filep,"\n");
  }
  fprintf(filep,"\n");
}

//**** MakePsPictureFromFFEN ****
// 1: square coloring
// 2: corners drawing
// 3: line drawing
// 4: piece drawing
void MakePsPictureFromFFEN(unsigned char *Text,int TextLength) {
  int x,y,yy,start,yStart,Piece,Modifier,c,imbrication,color,NbColoredSquares;
  int dx,dy,BestX,BestY,BestDX,BestDY,BestBoxSize;
  int BestDXForThisCell,BestDYForThisCell,BestBoxSizeForThisCell,CurrentBoxSize,MaxDXForThisCell;
  unsigned char d;
  float fx,fy;
  int O1,O2; //values of OutsideBit
  int CurrentBorderInnerType;

  fprintf(filep,"\\begin{pspicture}(%g,%g)\n",Width*FFEN_SquareSize,Height*FFEN_SquareSize);

  //**** 1: square coloring
  //2013/02/17: try to mix filled squares together by filling biggest possible boxes.
  //Note: In the case aside, biggest box filling is not  111110
  //optimal, it gives 4 boxes while simple horizontal    011111
  //boxes would give 3. But that's quite rare IMO.       111110

  //surround coloring array with 0s
  for(y=0;y<=Height+1;y++) { for(x=0;x<=Width+1;x++) { Coloring[x][y]=0; } }
  //Make an array set to 1 where square is colored, to 0 otherwise. Count nb of 1s.
  NbColoredSquares=0;
  for(y=1;y<=Height;y++) {
    yy=Height+1-y;
    for(x=1;x<=Width;x++) {
      color=0;
      //x+y parity must be set according to FFEN_Fill
      // 0=none / 1=odd (chess) / 2=even / 3=full
      if(((((x+y)&1)+1)&FFEN_Fill)!=0) { color=1; }
      //outside squares must not be affected by FFEN_Fill
      if((Square_Modifiers[x][y] & Square_OutsideBit)==Square_OutsideBit) { color=0; }
      //user expressely asked for coloring modification with '$'?
      if((Square_Modifiers[x][y]&Square_FillBit)!=0) { color=1-color; }
      if(color==1) {
        if(FFEN_FillStyle==6) {
          fprintf(filep,"\\pscircle*(%g,%g){%g}\n",(x-0.5)*FFEN_SquareSize,(yy-0.5)*FFEN_SquareSize,1.5*FFEN_HBorderWidth*FFEN_SquareSize);
          //Don't mark it as colored square for this particular FillStyle
        } else {
          Coloring[x][y]=color;
          NbColoredSquares++;
        }
      }
    } //next x
  } //next y
  //As long as there are pixels set to 1:
  //  Search for the biggest box
  //    if Width*(Height+1-y) <= BestBoxSize we can stop searching for this iteration!
  //       (assuming we're making an y loop first)
  //    if (width-x)*(height-y) <= BiggestFoundBox we can stop searching for this cell!
  //  Output \pspolygon for this box
  //  Fill this box with 0s in the array
  //  Decrease nb of 1s by w*h
  //Loop
  while(NbColoredSquares>0) {
    BestBoxSize=0;
    y=1;
    while((y<=Height) && (Width*(Height+1-y)>BestBoxSize)) {
      x=1;
      while((x<=Width) && ((Width+1-x)*(Height+1-y)>BestBoxSize)) {
        if(Coloring[x][y]==1) {
          BestBoxSizeForThisCell=0;
          MaxDXForThisCell=Width-x;
          dy=0;
          while(Coloring[x][y+dy]==1) {
            dx=0;
            while((Coloring[x+dx+1][y+dy]==1) && (dx<=MaxDXForThisCell)) {
              dx++;
            }
            if(dx<MaxDXForThisCell) { MaxDXForThisCell=dx; }
            CurrentBoxSize=(MaxDXForThisCell+1)*(dy+1);
            if(CurrentBoxSize>BestBoxSizeForThisCell) {
              BestDXForThisCell=MaxDXForThisCell;
              BestDYForThisCell=dy;
              BestBoxSizeForThisCell=CurrentBoxSize;
            }
            dy++;
          }
          if(BestBoxSizeForThisCell>BestBoxSize) {
            BestX=x;
            BestY=y;
            BestDX=BestDXForThisCell;
            BestDY=BestDYForThisCell;
            BestBoxSize=BestBoxSizeForThisCell;
          }
        }
        x++;
      }
      y++;
    }
    //We know the best box, output FFEN code for it, fill it with 0s
    yy=Height+1-BestY;
    fprintf(filep,"\\psframe[linewidth=0,");
    switch(FFEN_FillStyle) {
      case 0: fprintf(filep,"linecolor=lightgray,fillstyle=solid,fillcolor=lightgray]"); break; // 0=lightgray
      case 1: fprintf(filep,"linecolor=gray,fillstyle=solid,fillcolor=gray]"); break; // 1=gray
      case 2: fprintf(filep,"linecolor=black,fillstyle=solid,fillcolor=black]"); break; // 2=black
      case 3: fprintf(filep,"linecolor=white,fillstyle=vlines]"); break; // 3=vlines
      case 4: fprintf(filep,"linecolor=white,fillstyle=hlines]"); break; // 4=hlines
      case 5: fprintf(filep,"linecolor=white,fillstyle=crosshatch]"); break; // 5=crosshatch
    }
    fprintf(filep,"(%g,%g)(%g,%g)\n",(BestX-1)*FFEN_SquareSize,yy*FFEN_SquareSize,(BestX+BestDX)*FFEN_SquareSize,(yy-BestDY-1)*FFEN_SquareSize);
    for(y=BestY;y<BestY+BestDY+1;y++) {
      for(x=BestX;x<BestX+BestDX+1;x++) {
        Coloring[x][y]=0;
      }
    }
    NbColoredSquares=NbColoredSquares-BestBoxSize;
  }

  //**** 2: corners drawing
  if(FFEN_CornersRadius!=0) {
    for(y=0;y<=Height;y++) {
      yy=Height-y;
      for(x=0;x<=Width;x++) {
        if(   ((Square_Modifiers[x][y] & Square_OutsideBit)!=0)
           && ((Square_Modifiers[x+1][y] & Square_OutsideBit)!=0)
           && ((Square_Modifiers[x+1][y+1] & Square_OutsideBit)!=0)
           && ((Square_Modifiers[x][y+1] & Square_OutsideBit)!=0)) {
         } else {
          fprintf(filep,"\\pscircle*(%g,%g){%g}\n",x*FFEN_SquareSize,yy*FFEN_SquareSize,FFEN_CornersRadius*FFEN_SquareSize);
         }
       }
     }
   }

  //**** 3: lines drawing
  //Horizontal lines
  for(y=0;y<=Height;y++) {
    yy=Height-y;
    //Get Border/Inner/Nothing infos for this horizontal line
    for(x=1;x<=Width;x++) {
      BorderInnerType[x]=0; //Nothing
      O1=Square_Modifiers[x][y] & Square_OutsideBit;
      O2=Square_Modifiers[x][y+1] & Square_OutsideBit;
      switch(O1*2+O2) {
        case Square_OutsideBit*2+Square_OutsideBit: break; //nothing
        case Square_OutsideBit*2:
          if(FFEN_BorderUp==1) { BorderInnerType[x]=1; } //Border
          break;
        case Square_OutsideBit:
          if(FFEN_BorderDown==1) { BorderInnerType[x]=1; }//Border
          break;
        case 0:
          BorderInnerType[x]=2; //Inner
          break;
        default:
          printf("ERREUR");
          break;
      }
      if((Square_Modifiers[x][y] & Square_BelowBorderBit)!=0) {
        BorderInnerType[x]=1; //Border
      }
      if((Square_Modifiers[x][y+1] & Square_AboveBorderBit)!=0) {
        BorderInnerType[x]=1; //Border
      }
    }

    //Now draw this horizontal line
    start=1;
    do {
      x=start;
      CurrentBorderInnerType=BorderInnerType[start];
      while((x<=Width) && (BorderInnerType[x]==CurrentBorderInnerType)) { x++; }
      //Inner or Border line
      if(CurrentBorderInnerType!=0) {
        //Inner line
        if(CurrentBorderInnerType==2) {
          if(FFEN_HInnerStyle!=0) {
            fprintf(filep,"\\psline[linewidth=%g",FFEN_HInnerWidth*FFEN_SquareSize);
            switch(FFEN_HInnerStyle) {
              // case 1: break;
              case 2: fprintf(filep,",linestyle=dashed"); break;
              case 3: fprintf(filep,",linestyle=dotted"); break;
            }
            fprintf(filep,"]{c-c}(%g,%g)(%g,%g)\n",(start-1)*FFEN_SquareSize,yy*FFEN_SquareSize,(x-1)*FFEN_SquareSize,yy*FFEN_SquareSize);
          }
        //Border line
        } else {
          if(FFEN_HBorderStyle!=0) {
            fprintf(filep,"\\psline[linewidth=%g",FFEN_HBorderWidth*FFEN_SquareSize);
            switch(FFEN_HBorderStyle) {
              // case 1: break;
              case 2: fprintf(filep,",linestyle=dashed"); break;
              case 3: fprintf(filep,",linestyle=dotted"); break;
            }
            fprintf(filep,"]{c-c}(%g,%g)(%g,%g)\n",(start-1)*FFEN_SquareSize,yy*FFEN_SquareSize,(x-1)*FFEN_SquareSize,yy*FFEN_SquareSize);
          }
        }
      }
      start=x;
    } while(start<=Width);
  } //next y

  //Vertical lines
  for(x=0;x<=Width;x++) {
    //Get Border/Inner/Nothing infos for this horizontal line
    for(y=1;y<=Height;y++) {
      BorderInnerType[y]=0; //Nothing
      O1=Square_Modifiers[x][y] & Square_OutsideBit;
      O2=Square_Modifiers[x+1][y] & Square_OutsideBit;
      switch(O1*2+O2) {
        case Square_OutsideBit*2+Square_OutsideBit: break; //Nothing
        case Square_OutsideBit*2:
          if(FFEN_BorderLeft==1) { BorderInnerType[y]=1; } //Border
          break;
        case Square_OutsideBit:
          if(FFEN_BorderRight==1) { BorderInnerType[y]=1; } //Border
          break;
        case 0:
          BorderInnerType[y]=2; //Inner
          break;
        default:
          printf("ERREUR");
          break;
      }
      if((Square_Modifiers[x+1][y] & Square_LeftBorderBit)!=0) {
        BorderInnerType[y]=1; //Border
      }
      if((Square_Modifiers[x][y] & Square_RightBorderBit)!=0) {
        BorderInnerType[y]=1; //Border
      }
    }

    //Now draw this vertical line
    start=1;
    do {
      y=start;
      CurrentBorderInnerType=BorderInnerType[start];
      while((y<=Height) && (BorderInnerType[y]==CurrentBorderInnerType)) { y++; }
      yy=Height+1-y;
      yStart=Height+1-start;
      //Inner or Border line
      if(CurrentBorderInnerType!=0) {
        //Inner line
        if(CurrentBorderInnerType==2) {
          if(FFEN_VInnerStyle!=0) {
            fprintf(filep,"\\psline[linewidth=%g",FFEN_VInnerWidth*FFEN_SquareSize);
            switch(FFEN_VInnerStyle) {
              // case 1: break;
              case 2: fprintf(filep,",linestyle=dashed"); break;
              case 3: fprintf(filep,",linestyle=dotted"); break;
            }
            fprintf(filep,"]{c-c}(%g,%g)(%g,%g)\n",x*FFEN_SquareSize,yStart*FFEN_SquareSize,x*FFEN_SquareSize,yy*FFEN_SquareSize);
          }
        //Border line
        } else {
          if(FFEN_VBorderStyle!=0) {
            fprintf(filep,"\\psline[linewidth=%g",FFEN_VBorderWidth*FFEN_SquareSize);
            switch(FFEN_VBorderStyle) {
              // case 1: break;
              case 2: fprintf(filep,",linestyle=dashed"); break;
              case 3: fprintf(filep,",linestyle=dotted"); break;
            }
            fprintf(filep,"]{c-c}(%g,%g)(%g,%g)\n",x*FFEN_SquareSize,yStart*FFEN_SquareSize,x*FFEN_SquareSize,yy*FFEN_SquareSize);
          }
        }
      }
      start=y;
    } while(start<=Height);
  } //next x

  //**** 4: pieces drawing
  MODSP_Pos=0;
  for(y=1;y<=Height;y++) {
    for(x=1;x<=Width;x++) {
      Piece=Pieces[x][y];
      if(Piece!=ES) {
        Modifier=Piece_Modifiers[x][y];
        //Right or Up displacement?
        fx=(x-1+0.5*(Modifier & Piece_RightBit)/Piece_RightBit)*FFEN_SquareSize;
        fy=(Height-y+0.5*(Modifier & Piece_UpBit)/Piece_UpBit)*FFEN_SquareSize;
        fprintf(filep,"\\rput[bl]");
        //Rotation?
        yy=Modifier & (3*Piece_RotationBits);
        switch(yy) {
          case 0: fprintf(filep,"(%g,%g){",fx,fy); break;
          case 1: fprintf(filep,"{90}(%g,%g){",fx+1*FFEN_SquareSize,fy); break;
          case 2: fprintf(filep,"{180}(%g,%g){",fx+1*FFEN_SquareSize,fy+1*FFEN_SquareSize); break;
          case 3: fprintf(filep,"{270}(%g,%g){",fx,fy+1*FFEN_SquareSize); break;
        }
        if((Modifier & Piece_CircledBit)!=0) {
          fprintf(filep,"\\pscircle[linewidth=%g](%g,%g){%g} ",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.4*FFEN_SquareSize);
        }

#define SetFillColor \
  switch((Modifier & (3*Piece_FillBit))/Piece_FillBit) { \
    case 0: break; \
    case 1: fprintf(filep,",fillcolor=lightgray"); break; \
    case 2: fprintf(filep,",fillcolor=gray"); break; \
  }

        //Draw piece
        switch(Piece) {
          case WK:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.15*FFEN_SquareSize,0.6*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize,0.85*FFEN_SquareSize,0.6*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.2*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize);
//            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.6*FFEN_SquareSize,0.45*FFEN_SquareSize,0.7*FFEN_SquareSize,0.35*FFEN_SquareSize,0.75*FFEN_SquareSize,0.45*FFEN_SquareSize,0.8*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.55*FFEN_SquareSize,0.8*FFEN_SquareSize,0.65*FFEN_SquareSize,0.75*FFEN_SquareSize,0.55*FFEN_SquareSize,0.7*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.5*FFEN_SquareSize,0.6*FFEN_SquareSize,0.35*FFEN_SquareSize,0.75*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.65*FFEN_SquareSize,0.75*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case BK:
            fprintf(filep,"\\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.15*FFEN_SquareSize,0.6*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize,0.85*FFEN_SquareSize,0.6*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\psline[linecolor=white,linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.2*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize);
//            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.6*FFEN_SquareSize,0.45*FFEN_SquareSize,0.7*FFEN_SquareSize,0.35*FFEN_SquareSize,0.75*FFEN_SquareSize,0.45*FFEN_SquareSize,0.8*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.55*FFEN_SquareSize,0.8*FFEN_SquareSize,0.65*FFEN_SquareSize,0.75*FFEN_SquareSize,0.55*FFEN_SquareSize,0.7*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.6*FFEN_SquareSize,0.35*FFEN_SquareSize,0.75*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.65*FFEN_SquareSize,0.75*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case WQ:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.75*FFEN_SquareSize,0.35*FFEN_SquareSize,0.45*FFEN_SquareSize,0.37*FFEN_SquareSize,0.9*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.63*FFEN_SquareSize,0.9*FFEN_SquareSize,0.65*FFEN_SquareSize,0.45*FFEN_SquareSize,0.9*FFEN_SquareSize,0.75*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case BQ:
            fprintf(filep,"\\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.75*FFEN_SquareSize,0.35*FFEN_SquareSize,0.45*FFEN_SquareSize,0.37*FFEN_SquareSize,0.9*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.63*FFEN_SquareSize,0.9*FFEN_SquareSize,0.65*FFEN_SquareSize,0.45*FFEN_SquareSize,0.9*FFEN_SquareSize,0.75*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case WR:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.25*FFEN_SquareSize,0.6*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.35*FFEN_SquareSize,0.8*FFEN_SquareSize,0.35*FFEN_SquareSize,0.7*FFEN_SquareSize,0.42*FFEN_SquareSize,0.7*FFEN_SquareSize,0.42*FFEN_SquareSize,0.83*FFEN_SquareSize,0.58*FFEN_SquareSize,0.83*FFEN_SquareSize,0.58*FFEN_SquareSize,0.7*FFEN_SquareSize,0.65*FFEN_SquareSize,0.7*FFEN_SquareSize,0.65*FFEN_SquareSize,0.8*FFEN_SquareSize,0.8*FFEN_SquareSize,0.8*FFEN_SquareSize,0.75*FFEN_SquareSize,0.6*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.3*FFEN_SquareSize,0.6*FFEN_SquareSize,0.7*FFEN_SquareSize,0.6*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case BR:
            fprintf(filep,"\\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.25*FFEN_SquareSize,0.6*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.35*FFEN_SquareSize,0.8*FFEN_SquareSize,0.35*FFEN_SquareSize,0.7*FFEN_SquareSize,0.42*FFEN_SquareSize,0.7*FFEN_SquareSize,0.42*FFEN_SquareSize,0.83*FFEN_SquareSize,0.58*FFEN_SquareSize,0.83*FFEN_SquareSize,0.58*FFEN_SquareSize,0.7*FFEN_SquareSize,0.65*FFEN_SquareSize,0.7*FFEN_SquareSize,0.65*FFEN_SquareSize,0.8*FFEN_SquareSize,0.8*FFEN_SquareSize,0.8*FFEN_SquareSize,0.75*FFEN_SquareSize,0.6*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.3*FFEN_SquareSize,0.6*FFEN_SquareSize,0.7*FFEN_SquareSize,0.6*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case WB:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.35*FFEN_SquareSize,0.2*FFEN_SquareSize,0.2*FFEN_SquareSize,0.45*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.63333*FFEN_SquareSize,0.7*FFEN_SquareSize,0.55*FFEN_SquareSize,0.52*FFEN_SquareSize,0.7*FFEN_SquareSize,0.6*FFEN_SquareSize,0.8*FFEN_SquareSize,0.45*FFEN_SquareSize,0.65*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case BB:
            fprintf(filep,"\\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.35*FFEN_SquareSize,0.2*FFEN_SquareSize,0.2*FFEN_SquareSize,0.45*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.63333*FFEN_SquareSize,0.7*FFEN_SquareSize,0.55*FFEN_SquareSize,0.52*FFEN_SquareSize,0.7*FFEN_SquareSize,0.6*FFEN_SquareSize,0.8*FFEN_SquareSize,0.45*FFEN_SquareSize,0.65*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case WN:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.385*FFEN_SquareSize,0.5*FFEN_SquareSize,0.25*FFEN_SquareSize,0.54*FFEN_SquareSize,0.24*FFEN_SquareSize,0.68*FFEN_SquareSize,0.448*FFEN_SquareSize,0.72*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pscircle[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g){%g}",0.45*FFEN_SquareSize,0.65*FFEN_SquareSize,.025*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case BN:
            fprintf(filep,"\\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.3*FFEN_SquareSize,0.2*FFEN_SquareSize,0.385*FFEN_SquareSize,0.5*FFEN_SquareSize,0.25*FFEN_SquareSize,0.54*FFEN_SquareSize,0.24*FFEN_SquareSize,0.68*FFEN_SquareSize,0.448*FFEN_SquareSize,0.72*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.7*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pscircle[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g){%g}",FFEN_ThinLineWidth*FFEN_SquareSize,0.45*FFEN_SquareSize,0.65*FFEN_SquareSize,.025*FFEN_SquareSize);
            fprintf(filep," \\pspolygon[linecolor=white,fillcolor=black,fillstyle=solid,linewidth=%g](%g,%g)(%g,%g)(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.2*FFEN_SquareSize,0.1*FFEN_SquareSize,0.25*FFEN_SquareSize,0.2*FFEN_SquareSize,0.75*FFEN_SquareSize,0.2*FFEN_SquareSize,0.8*FFEN_SquareSize,0.1*FFEN_SquareSize);
            break;
          case WP:
            fprintf(filep,"\\pswedge[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g){%g}{0}{180}",0.5*FFEN_SquareSize,0.1*FFEN_SquareSize,0.3*FFEN_SquareSize);
            fprintf(filep," \\pscircle[fillstyle=solid,linewidth=0,");
            switch((Modifier & (3*Piece_FillBit))/Piece_FillBit) {
              case 0: fprintf(filep,"linecolor=white,fillcolor=white");break;
              case 1: fprintf(filep,"linecolor=lightgray,fillcolor=lightgray"); break;
              case 2: fprintf(filep,"linecolor=gray,fillcolor=gray"); break;
            }
            fprintf(filep,"](%g,%g){%g}",0.5*FFEN_SquareSize,0.45*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\psarc[linewidth=%g](%g,%g){%g}{322}{218}",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.45*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pscircle[fillstyle=solid,linewidth=0,");
            switch((Modifier & (3*Piece_FillBit))/Piece_FillBit) {
              case 0: fprintf(filep,"linecolor=white,fillcolor=white");break;
              case 1: fprintf(filep,"linecolor=lightgray,fillcolor=lightgray"); break;
              case 2: fprintf(filep,"linecolor=gray,fillcolor=gray"); break;
            }
            fprintf(filep,"](%g,%g){%g}",0.5*FFEN_SquareSize,0.68*FFEN_SquareSize,0.13*FFEN_SquareSize);
            fprintf(filep," \\psarc[linewidth=%g](%g,%g){%g}{325}{215}",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.68*FFEN_SquareSize,0.13*FFEN_SquareSize);
            break;
          case BP:
            fprintf(filep,"\\pswedge[fillstyle=solid,fillcolor=black,linecolor=white,linewidth=%g](%g,%g){%g}{0}{180}",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.1*FFEN_SquareSize,0.3*FFEN_SquareSize);
            fprintf(filep," \\pscircle[fillstyle=solid,linewidth=0,linecolor=black,fillcolor=black](%g,%g){%g}",0.5*FFEN_SquareSize,0.45*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\psarc[linewidth=%g,linecolor=white](%g,%g){%g}{322}{218}",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.45*FFEN_SquareSize,0.2*FFEN_SquareSize);
            fprintf(filep," \\pscircle[fillstyle=solid,linewidth=0,linecolor=black,fillcolor=black](%g,%g){%g}",0.5*FFEN_SquareSize,0.68*FFEN_SquareSize,0.13*FFEN_SquareSize);
            fprintf(filep," \\psarc[linewidth=%g,linecolor=white](%g,%g){%g}{325}{215}",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.68*FFEN_SquareSize,0.13*FFEN_SquareSize);
            break;
          case WCircle:
            fprintf(filep,"\\pscircle[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g){%g}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.4*FFEN_SquareSize);
//!!!            if((Modifier & Piece_FillBit)!=0) {
//!!!              fprintf(filep," \\pswedge*(%g,%g){%g}{0}{90}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.35*FFEN_SquareSize);
//!!!              fprintf(filep," \\pswedge*(%g,%g){%g}{180}{270}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.35*FFEN_SquareSize);
//!!!            }
            break;
          case BCircle:
            fprintf(filep,"\\pscircle*(%g,%g){%g}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.4*FFEN_SquareSize);
            break;
          case WhiteSmallCircle:
            fprintf(filep,"\\pscircle[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g){%g}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.2*FFEN_SquareSize);
            break;
          case BlackSmallCircle:
            fprintf(filep,"\\pscircle*(%g,%g){%g}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.2*FFEN_SquareSize);
            break;
          case BoldCircle:
            fprintf(filep,"\\pscircle[fillstyle=solid,linewidth=%g",FFEN_BoldLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g){%g}",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.35*FFEN_SquareSize);
            break;
          case WDame:
            fprintf(filep,"\\pscircle[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g){%g} ",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.4*FFEN_SquareSize);
            fprintf(filep,"\\pscircle[linewidth=%g](%g,%g){%g}",FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.27*FFEN_SquareSize);
            break;
          case BDame:
            fprintf(filep,"\\pscircle*(%g,%g){%g} ",0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.4*FFEN_SquareSize);
            fprintf(filep,"\\pscircle[linewidth=%g,linecolor=white](%g,%g){%g}",1.5*FFEN_ThinLineWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.3*FFEN_SquareSize);
            break;
          case WCross:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.3*FFEN_SquareSize,0.15*FFEN_SquareSize,0.5*FFEN_SquareSize,0.35*FFEN_SquareSize,0.7*FFEN_SquareSize,0.15*FFEN_SquareSize,0.85*FFEN_SquareSize,0.3*FFEN_SquareSize,0.65*FFEN_SquareSize,0.5*FFEN_SquareSize,0.85*FFEN_SquareSize,0.7*FFEN_SquareSize,0.7*FFEN_SquareSize,0.85*FFEN_SquareSize,0.5*FFEN_SquareSize,0.65*FFEN_SquareSize,0.3*FFEN_SquareSize,0.85*FFEN_SquareSize,0.15*FFEN_SquareSize,0.7*FFEN_SquareSize,0.35*FFEN_SquareSize,0.5*FFEN_SquareSize,0.15*FFEN_SquareSize,0.3*FFEN_SquareSize,0.3*FFEN_SquareSize,0.15*FFEN_SquareSize);
            break;
          case BCross:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_BoldLineWidth*FFEN_SquareSize,0.25*FFEN_SquareSize,0.25*FFEN_SquareSize,0.75*FFEN_SquareSize,0.75*FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_BoldLineWidth*FFEN_SquareSize,0.75*FFEN_SquareSize,0.25*FFEN_SquareSize,0.25*FFEN_SquareSize,0.75*FFEN_SquareSize);
//            fprintf(filep,"\\pspolygon*[linewidth=0,linecolor=black](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.3*FFEN_SquareSize,0.15*FFEN_SquareSize,0.5*FFEN_SquareSize,0.35*FFEN_SquareSize,0.7*FFEN_SquareSize,0.15*FFEN_SquareSize,0.85*FFEN_SquareSize,0.3*FFEN_SquareSize,0.65*FFEN_SquareSize,0.5*FFEN_SquareSize,0.85*FFEN_SquareSize,0.7*FFEN_SquareSize,0.7*FFEN_SquareSize,0.85*FFEN_SquareSize,0.5*FFEN_SquareSize,0.65*FFEN_SquareSize,0.3*FFEN_SquareSize,0.85*FFEN_SquareSize,0.15*FFEN_SquareSize,0.7*FFEN_SquareSize,0.35*FFEN_SquareSize,0.5*FFEN_SquareSize,0.15*FFEN_SquareSize,0.3*FFEN_SquareSize,0.3*FFEN_SquareSize,0.15*FFEN_SquareSize);
            break;
          case ThinCross:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.3*FFEN_SquareSize,0.3*FFEN_SquareSize,0.7*FFEN_SquareSize,0.7*FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_ThinLineWidth*FFEN_SquareSize,0.7*FFEN_SquareSize,0.3*FFEN_SquareSize,0.3*FFEN_SquareSize,0.7*FFEN_SquareSize);
            break;
          case WSquare:
            fprintf(filep,"\\psframe[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)",0.21*FFEN_SquareSize,0.21*FFEN_SquareSize,0.79*FFEN_SquareSize,0.79*FFEN_SquareSize);
            break;
          case BSquare:
            fprintf(filep,"\\psframe*(%g,%g)(%g,%g)",0.21*FFEN_SquareSize,0.21*FFEN_SquareSize,0.79*FFEN_SquareSize,0.79*FFEN_SquareSize);
            break;
          case WTriangle:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)",0.21*FFEN_SquareSize,0.25*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize,0.79*FFEN_SquareSize,0.25*FFEN_SquareSize);
            break;
          case BTriangle:
            fprintf(filep,"\\pspolygon*[linewidth=0,linecolor=black](%g,%g)(%g,%g)(%g,%g)",0.21*FFEN_SquareSize,0.25*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize,0.79*FFEN_SquareSize,0.25*FFEN_SquareSize);
            break;
          case WStar:
#define StarRadius 0.35
#define cos60divsqrt3 0.288675134
#define sin60divsqrt3 0.5
#define cos30 0.866025403
#define sin30 0.5
#define divsqrt3 0.577350269
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.5*FFEN_SquareSize,(0.5+StarRadius)*FFEN_SquareSize,(0.5+StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5+StarRadius*sin60divsqrt3)*FFEN_SquareSize,(0.5+StarRadius*cos30)*FFEN_SquareSize,(0.5+StarRadius*sin30)*FFEN_SquareSize,(0.5+StarRadius*divsqrt3)*FFEN_SquareSize,0.5*FFEN_SquareSize,(0.5+StarRadius*cos30)*FFEN_SquareSize,(0.5-StarRadius*sin30)*FFEN_SquareSize,(0.5+StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5-StarRadius*sin60divsqrt3)*FFEN_SquareSize,0.5*FFEN_SquareSize,(0.5-StarRadius)*FFEN_SquareSize,(0.5-StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5-StarRadius*sin60divsqrt3)*FFEN_SquareSize,(0.5-StarRadius*cos30)*FFEN_SquareSize,(0.5-StarRadius*sin30)*FFEN_SquareSize,(0.5-StarRadius*divsqrt3)*FFEN_SquareSize,0.5*FFEN_SquareSize,(0.5-StarRadius*cos30)*FFEN_SquareSize,(0.5+StarRadius*sin30)*FFEN_SquareSize,(0.5-StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5+StarRadius*sin60divsqrt3)*FFEN_SquareSize);
            break;
          case BStar:
            fprintf(filep,"\\pspolygon*[linewidth=0,linecolor=black](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.5*FFEN_SquareSize,(0.5+StarRadius)*FFEN_SquareSize,(0.5+StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5+StarRadius*sin60divsqrt3)*FFEN_SquareSize,(0.5+StarRadius*cos30)*FFEN_SquareSize,(0.5+StarRadius*sin30)*FFEN_SquareSize,(0.5+StarRadius*divsqrt3)*FFEN_SquareSize,0.5*FFEN_SquareSize,(0.5+StarRadius*cos30)*FFEN_SquareSize,(0.5-StarRadius*sin30)*FFEN_SquareSize,(0.5+StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5-StarRadius*sin60divsqrt3)*FFEN_SquareSize,0.5*FFEN_SquareSize,(0.5-StarRadius)*FFEN_SquareSize,(0.5-StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5-StarRadius*sin60divsqrt3)*FFEN_SquareSize,(0.5-StarRadius*cos30)*FFEN_SquareSize,(0.5-StarRadius*sin30)*FFEN_SquareSize,(0.5-StarRadius*divsqrt3)*FFEN_SquareSize,0.5*FFEN_SquareSize,(0.5-StarRadius*cos30)*FFEN_SquareSize,(0.5+StarRadius*sin30)*FFEN_SquareSize,(0.5-StarRadius*cos60divsqrt3)*FFEN_SquareSize,(0.5+StarRadius*sin60divsqrt3)*FFEN_SquareSize);
            break;
          case Club:
            fprintf(filep,"\\rput[B](%g,%g){$\\clubsuit$}",0.5*FFEN_SquareSize,0.25*FFEN_SquareSize);
            break;
          case Diamond:
            fprintf(filep,"\\rput[B](%g,%g){$\\diamondsuit$}",0.5*FFEN_SquareSize,0.25*FFEN_SquareSize);
            break;
          case Heart:
            fprintf(filep,"\\rput[B](%g,%g){$\\heartsuit$}",0.5*FFEN_SquareSize,0.25*FFEN_SquareSize);
            break;
          case Spade:
            fprintf(filep,"\\rput[B](%g,%g){$\\spadesuit$}",0.5*FFEN_SquareSize,0.25*FFEN_SquareSize);
            break;
          case UpBar:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize);
            break;
          case VerticalBar:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,0)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize);
            break;
          case SmallVerticalBar:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.25*FFEN_SquareSize,0.5*FFEN_SquareSize,0.75*FFEN_SquareSize);
            break;
          case UpLeftBar:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,%g)(%g,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize);
            break;
          case VerticalLeftBar:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,0)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(0,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize);
            break;
          case CrossBar:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize,0.5*FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(%g,0)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize);
            break;
          case HalfDiagonal:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize);
            break;
          case Diagonal:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,%g)(%g,0)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case VHalfDiagonals:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,%g)(%g,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case YDiagonal:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,0)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(0,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize);
            break;
          case CrossDiagonals:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(0,%g)(%g,0)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            fprintf(filep," \\psline[linewidth=%g]{c-c}(0,0)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case LessThan:
            fprintf(filep,"\\psline[linewidth=%g]{c-c}(%g,%g)(%g,%g)(%g,%g)",FFEN_HBorderWidth*FFEN_SquareSize,0.65*FFEN_SquareSize,0.8*FFEN_SquareSize,0.25*FFEN_SquareSize,0.5*FFEN_SquareSize,0.65*FFEN_SquareSize,0.2*FFEN_SquareSize);
            break;
          case UpLeftArrow:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",(.5+.212132)*FFEN_SquareSize,(.5-.353553)*FFEN_SquareSize,(.5+.353553)*FFEN_SquareSize,(.5-.212132)*FFEN_SquareSize,(.5+.0)*FFEN_SquareSize,(.5+.141421)*FFEN_SquareSize,(.5+.141421)*FFEN_SquareSize,(.5+.282842)*FFEN_SquareSize,(.5-.282842)*FFEN_SquareSize,(.5+.282842)*FFEN_SquareSize,(.5-.282842)*FFEN_SquareSize,(.5-.141421)*FFEN_SquareSize,(.5-.141421)*FFEN_SquareSize,(.5+.0)*FFEN_SquareSize);
            break;
          case UpArrow:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=%g",FFEN_ThinLineWidth*FFEN_SquareSize);
            SetFillColor
            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.4*FFEN_SquareSize,0.1*FFEN_SquareSize,0.6*FFEN_SquareSize,0.1*FFEN_SquareSize,0.6*FFEN_SquareSize,0.6*FFEN_SquareSize,0.8*FFEN_SquareSize,0.6*FFEN_SquareSize,0.5*FFEN_SquareSize,0.9*FFEN_SquareSize,0.2*FFEN_SquareSize,0.6*FFEN_SquareSize,0.4*FFEN_SquareSize,0.6*FFEN_SquareSize);
//!!!smaller arrow            fprintf(filep,"](%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)",0.4*FFEN_SquareSize,0.2*FFEN_SquareSize,0.6*FFEN_SquareSize,0.2*FFEN_SquareSize,0.6*FFEN_SquareSize,0.5*FFEN_SquareSize,0.8*FFEN_SquareSize,0.5*FFEN_SquareSize,0.5*FFEN_SquareSize,0.8*FFEN_SquareSize,0.2*FFEN_SquareSize,0.5*FFEN_SquareSize,0.4*FFEN_SquareSize,0.5*FFEN_SquareSize);
            break;

#define SetLineColorLikeFillColor \
  switch((Modifier & (3*Piece_FillBit))/Piece_FillBit) { \
    case 0: fprintf(filep,",linecolor=white"); break; \
    case 1: fprintf(filep,",linecolor=lightgray"); break; \
    case 2: fprintf(filep,",linecolor=gray"); break; \
  }
          case WhiteFill:
            fprintf(filep,"\\pspolygon[fillstyle=solid,linewidth=0");
            SetFillColor
            SetLineColorLikeFillColor
            fprintf(filep,"](0,0)(0,%g)(%g,%g)(%g,0)",FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case CrossHatchFill:
            fprintf(filep,"\\pspolygon[fillstyle=crosshatch,linewidth=0,linecolor=white](0,0)(0,%g)(%g,%g)(%g,0)",FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case HLineFill:
            fprintf(filep,"\\pspolygon[fillstyle=hlines,linewidth=0,linecolor=white](0,0)(0,%g)(%g,%g)(%g,0)",FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case VLineFill:
            fprintf(filep,"\\pspolygon[fillstyle=vlines,linewidth=0,linecolor=white](0,0)(0,%g)(%g,%g)(%g,0)",FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize,FFEN_SquareSize);
            break;
          case 39: //' for Double Symbol ('' in the FFEN, ' in Pieces[x][y]
            c=MacroOrDoubleSymbolPointer[MODSP_Pos];
            fprintf(filep,"\\rput[B](%g,%g){%c%c}",0.5*FFEN_SquareSize,0.25*FFEN_SquareSize,Text[c],Text[c+1]);
            MODSP_Pos++;
            break;
          case MacroCommand: //
            c=MacroOrDoubleSymbolPointer[MODSP_Pos];
            MODSP_Pos++;
            imbrication=1;
            //copy till the end of the macro, ie after the '}' related to this '{'
            do{
              d=Text[c++];
              switch(d) {
                case'{': imbrication++; fprintf(filep,"{"); break;
                case'}':
                  imbrication--;
                  if(imbrication!=0) { fprintf(filep,"}"); }
                  break;
                default:
                  fprintf(filep,"%c",d);
                  break;
              }
            } while((d!='}') || (imbrication!=0));
            break;
          default: //Symbol between '!' and 'z'
            fprintf(filep,"\\rput[B](%g,%g){%c}",0.5*FFEN_SquareSize,0.25*FFEN_SquareSize,Pieces[x][y]);
            break;
        }
        fprintf(filep,"}\n");

      }
    }
  }

  fprintf(filep,"\\end{pspicture}\n");
  if(FFEN_PieceCount==1) {
    fprintf(filep,"~\\\\%d+%d:\n",NbWhitePieces,NbBlackPieces);
  }
  if(FFEN_PieceCount==2) {
    fprintf(filep,"~\\\\?+?=%d:\n",NbWhitePieces+NbBlackPieces);
  }
}


//**** Main *****************************************************************
int main(argc,argv) int argc; char *argv[]; {
  int LastPos,CurrentFFENPos,CurrentCUBESPos,CurrentPos,OptionPos;

  setbuf(stdout, NULL); // no buffering for output!
  if(argc!=3) { printf("usage: ffen2tex InFile.tex OutFile.tex\n"); exit(1); }

  //**** load InFile ****
  filep=fopen(argv[1],"rb");
  if(filep==0) { printf("error: %s not found\n",argv[1]); exit(1); }
  fseek(filep,0,2); // 2 for EOF
  InLength=(int) ftell(filep)+1;
  fseek(filep,0,0); // 0 for SOF
  In=(unsigned char *) malloc((unsigned int) InLength+1);
  fread(In,1,InLength-1,filep);
  In[InLength]=10; //LF at the end
  fclose(filep);

  //**** open OutFile ****
  filep=fopen(argv[2],"wb");
  if(filep==0) { printf("error: %s not found\n",argv[2]); exit(1); }
  fseek(filep,0,0); // 0 for SOF

  LastPos=0;
  CurrentFFENPos=SearchNext((unsigned char *) "%FFEN ",6,In,InLength,LastPos);
  CurrentCUBESPos=SearchNext((unsigned char *) "%CUBES ",7,In,InLength,LastPos);
  CurrentPos=CurrentFFENPos;
  if((CurrentPos==-1) || ((CurrentCUBESPos>0) && (CurrentCUBESPos<CurrentFFENPos))) { CurrentPos=CurrentCUBESPos; }
  while(CurrentPos!=-1) {
    //**** READ "%FFEN_" OPTIONS *****
    //between LastPos and CurrentPos
    OptionPos=SearchNext((unsigned char *) "%FFEN_SquareSize ",sizeof("%FFEN_SquareSize ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_SquareSize=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_SquareSize ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_ThinLineWidth ",sizeof("%FFEN_ThinLineWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_ThinLineWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_ThinLineWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BoldLineWidth ",sizeof("%FFEN_BoldLineWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_BoldLineWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_BoldLineWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_Fill ",sizeof("%FFEN_Fill ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_Fill=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_Fill ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_FillStyle ",sizeof("%FFEN_FillStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_FillStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_FillStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_InnerStyle ",sizeof("%FFEN_InnerStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HInnerStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_InnerStyle ")-1); FFEN_VInnerStyle=FFEN_HInnerStyle; }
    OptionPos=SearchNext((unsigned char *) "%FFEN_HInnerStyle ",sizeof("%FFEN_HInnerStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HInnerStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_HInnerStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_VInnerStyle ",sizeof("%FFEN_VInnerStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_VInnerStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_VInnerStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_InnerWidth ",sizeof("%FFEN_InnerWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HInnerWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_InnerWidth ")-1); FFEN_VInnerWidth=FFEN_HInnerWidth; }
    OptionPos=SearchNext((unsigned char *) "%FFEN_HInnerWidth ",sizeof("%FFEN_HInnerWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HInnerWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_HInnerWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_VInnerWidth ",sizeof("%FFEN_VInnerWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_VInnerWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_VInnerWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderStyle ",sizeof("%FFEN_BorderStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HBorderStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_BorderStyle ")-1); FFEN_VBorderStyle=FFEN_HBorderStyle; }
    OptionPos=SearchNext((unsigned char *) "%FFEN_HBorderStyle ",sizeof("%FFEN_HBorderStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HBorderStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_HBorderStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_VBorderStyle ",sizeof("%FFEN_VBorderStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_VBorderStyle=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_VBorderStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderWidth ",sizeof("%FFEN_BorderWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HBorderWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_BorderWidth ")-1); FFEN_VBorderWidth=FFEN_HBorderWidth; }
    OptionPos=SearchNext((unsigned char *) "%FFEN_HBorderWidth ",sizeof("%FFEN_HBorderWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_HBorderWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_HBorderWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_VBorderWidth ",sizeof("%FFEN_VBorderWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_VBorderWidth=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_VBorderWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderAll ",sizeof("%FFEN_BorderAll ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) {
      FFEN_BorderUp=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_BorderAll ")-1);
      FFEN_BorderLeft=FFEN_BorderUp;
      FFEN_BorderDown=FFEN_BorderUp;
      FFEN_BorderRight=FFEN_BorderUp;
    }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderUp ",sizeof("%FFEN_BorderUp ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_BorderUp=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_BorderUp ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderLeft ",sizeof("%FFEN_BorderLeft ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_BorderLeft=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_BorderLeft ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderDown ",sizeof("%FFEN_BorderDown ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_BorderDown=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_BorderDown ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_BorderRight ",sizeof("%FFEN_BorderRight ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_BorderRight=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_BorderRight ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_PieceCount ",sizeof("%FFEN_PieceCount ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_PieceCount=ReadInteger(In,InLength,OptionPos+sizeof("%FFEN_PieceCount ")-1); }
    OptionPos=SearchNext((unsigned char *) "%FFEN_CornersRadius ",sizeof("%FFEN_CornersRadius ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { FFEN_CornersRadius=ReadFloat(In,InLength,OptionPos+sizeof("%FFEN_CornersRadius ")-1); }
    //**** READ "%CUBES_" OPTIONS *****
    OptionPos=SearchNext((unsigned char *) "%CUBES_WAngle ",sizeof("%CUBES_WAngle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_WAngle=ReadInteger(In,InLength,OptionPos+sizeof("%CUBES_WAngle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_DAngle ",sizeof("%CUBES_DAngle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_DAngle=ReadInteger(In,InLength,OptionPos+sizeof("%CUBES_DAngle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_WSize ",sizeof("%CUBES_WSize ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_WSize=ReadFloat(In,InLength,OptionPos+sizeof("%CUBES_WSize ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_DSize ",sizeof("%CUBES_DSize ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_DSize=ReadFloat(In,InLength,OptionPos+sizeof("%CUBES_DSize ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_HSize ",sizeof("%CUBES_HSize ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_HSize=ReadFloat(In,InLength,OptionPos+sizeof("%CUBES_HSize ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_Rotation ",sizeof("%CUBES_Rotation ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_Rotation=ReadInteger(In,InLength,OptionPos+sizeof("%CUBES_Rotation ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_UpsideDown ",sizeof("%CUBES_UpsideDown ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_UpsideDown=ReadInteger(In,InLength,OptionPos+sizeof("%CUBES_UpsideDown ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_BoxBorder ",sizeof("%CUBES_BoxBorder ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_BoxBorder=ReadBinary(In,InLength,OptionPos+sizeof("%CUBES_BoxBorder ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_BoxBorderStyle ",sizeof("%CUBES_BoxBorderStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_BoxBorderStyle=ReadInteger(In,InLength,OptionPos+sizeof("%CUBES_BoxBorderStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_BoxBorderWidth ",sizeof("%CUBES_BoxBorderWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_BoxBorderWidth=ReadFloat(In,InLength,OptionPos+sizeof("%CUBES_BoxBorderWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_BoxInner ",sizeof("%CUBES_BoxInner ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_BoxInner=ReadBinary(In,InLength,OptionPos+sizeof("%CUBES_BoxInner ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_BoxInnerStyle ",sizeof("%CUBES_BoxInnerStyle ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_BoxInnerStyle=ReadInteger(In,InLength,OptionPos+sizeof("%CUBES_BoxInnerStyle ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_BoxInnerWidth ",sizeof("%CUBES_BoxInnerWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_BoxInnerWidth=ReadFloat(In,InLength,OptionPos+sizeof("%CUBES_BoxInnerWidth ")-1); }
    OptionPos=SearchNext((unsigned char *) "%CUBES_CubesWidth ",sizeof("%CUBES_CubesWidth ")-1,In,CurrentPos,LastPos);
    if(OptionPos!=-1) { CUBES_CubesWidth=ReadFloat(In,InLength,OptionPos+sizeof("%CUBES_CubesWidth ")-1); }
    //**** Read "%FFEN " and convert ****
    if(CurrentPos==CurrentFFENPos) {
      MODSP_Pos=0;
      NbWhitePieces=0;
      NbBlackPieces=0;
      NbOtherPieces=0;
      ReadFFEN(In,InLength,CurrentPos+6);
//!!!    PrintFFEN();
      //Search end of this "%FFEN " line and copy from LastPos to end of
      //this line. Also LastPos=CurrentPos+1.
      while((In[CurrentPos]!=10) && (CurrentPos<InLength)) { CurrentPos++; }
      while(LastPos<=CurrentPos) {
        fprintf(filep,"%c",In[LastPos]);
        LastPos++;
      }
      //insert pspicture for this FFEN
      MODSP_Pos=0;
      MakePsPictureFromFFEN(In,InLength);
    }
    //**** Read "%CUBES " and convert ****
    if(CurrentPos==CurrentCUBESPos) {
      ReadCUBES(In,InLength,CurrentPos+7);
//!!!      PrintCUBES();
      //Search end of this "%CUBES " line and copy from LastPos to end of
      //this line. Also LastPos=CurrentPos+1.
      while((In[CurrentPos]!=10) && (CurrentPos<InLength)) { CurrentPos++; }
      while(LastPos<=CurrentPos) {
        fprintf(filep,"%c",In[LastPos]);
        LastPos++;
      }
      MakePsPictureFromCUBES(In,InLength);
    }
    //**** Search next %FFEN or %CUBES ****
    CurrentFFENPos=SearchNext((unsigned char *) "%FFEN ",6,In,InLength,LastPos);
    CurrentCUBESPos=SearchNext((unsigned char *) "%CUBES ",7,In,InLength,LastPos);
    CurrentPos=CurrentFFENPos;
    if((CurrentPos==-1) || ((CurrentCUBESPos>0) && (CurrentCUBESPos<CurrentFFENPos))) { CurrentPos=CurrentCUBESPos; }
  }
  //Copy chars up to end of file
  while(LastPos<InLength-1) {
    fprintf(filep,"%c",In[LastPos]);
    LastPos++;
  }
  fclose(filep); //Close OutputFile
  exit(0);
}
