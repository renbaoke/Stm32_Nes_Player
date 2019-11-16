#include "InfoNES_System.h"

WORD NesPalette[ 64 ] =
{
  0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
  0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
  0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
  0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
  0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
  0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

int InfoNES_Menu(){return 0;}

int InfoNES_ReadRom( const char *pszFileName ){return 0;}

void InfoNES_ReleaseRom(){}

void InfoNES_LoadFrame(){}

void InfoNES_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem ){}

void *InfoNES_MemoryCopy( void *dest, const void *src, int count ){return 0;}

void *InfoNES_MemorySet( void *dest, int c, int count ){return 0;}

void InfoNES_DebugPrint( char *pszMsg ){}

void InfoNES_Wait(){}

void InfoNES_SoundInit( void ){}

int InfoNES_SoundOpen( int samples_per_sync, int sample_rate ){return 0;}

void InfoNES_SoundClose( void ){}

void InfoNES_SoundOutput(int samples, BYTE *wave1, BYTE *wave2, BYTE *wave3, BYTE *wave4, BYTE *wave5){}

void InfoNES_MessageBox( char *pszMsg, ... ){}
    