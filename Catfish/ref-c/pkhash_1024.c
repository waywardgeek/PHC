/*
 * Copyright (c) 2014, University of Waterloo
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the University of Waterloo nor the
 *   names of its contributors may be used to endorse or promote
 *   products derived from this software without specific prior
 *   written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author Xinxin Fan <fan@openecc.org>
 * @date   March 28, 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "pkhash_1024.h"

typedef uint8_t uint8;
typedef uint32_t uint32;
typedef int32_t sint32;
typedef uint64_t uint64;


#define XTOY(Y, X){\
	uint32 comsec;\
	for(comsec = 0; comsec < 32; comsec++)\
	   Y[comsec] = X[comsec];\
}

#define XIS1(X){\
	uint32 comsec;\
	X[0] = 1;\
	for(comsec = 1; comsec < 32; comsec++)\
	   X[comsec] = 0;\
}

//n = p * q
//= 0x432F0BC1 0EDA1D05 38E16FD4 C1882C01 736D2EBB 4A21CA00 F8DE5085 51C6AFF6 78BA19E7 A9162C00 C993CE4F B251BAB1 0981DABF 84154DDF 7C53C7B3 CC987165 
//    BCFFCE39 A4FCBAD8 3E1A8BBB 11A6FF92 8E09AAC0 5037A216 36A6671A CFED77D7 E109B243 F5BA9B5C CE8AA56E 85914785 096F33BC A9EBAC78 D074E52A 69F8C82B 
const static uint32 n[32] = 
{0x69F8C82B,0xD074E52A,0xA9EBAC78,0x096F33BC,0x85914785,0xCE8AA56E,0xF5BA9B5C,0xE109B243,0xCFED77D7,0x36A6671A,0x5037A216,0x8E09AAC0,0x11A6FF92,0x3E1A8BBB,0xA4FCBAD8,0xBCFFCE39,
 0xCC987165,0x7C53C7B3,0x84154DDF,0x0981DABF,0xB251BAB1,0xC993CE4F,0xA9162C00,0x78BA19E7,0x51C6AFF6,0xF8DE5085,0x4A21CA00,0x736D2EBB,0xC1882C01,0x38E16FD4,0x0EDA1D05,0x432F0BC1};

//u = [(2^32)^64/n]
//= 0x3 CF7940C3 DE91C915 1B5F6C50 9405B8E8 B53B627F DE336CE7 AD07CE21 DF456CD4 79B90D33 F34C6B9A 305CD8CB B21EE1FB 5E0872B5 6A1CB87F F7EB839A 2A90A7FF   
//      79FD048B 55D237D5 33D6995A 87BDE68F 4D359372 8D3FC57D 4FE464C2 126CBD30 D947EC7C 7DA43F2B E4FBCA6F CB4A4E91 F306B8B8 AAD13C12 C76E378E 0B34BF70
const static uint32 u[33] = 
{0x0B34BF70,0xC76E378E,0xAAD13C12,0xF306B8B8,0xCB4A4E91,0xE4FBCA6F,0x7DA43F2B,0xD947EC7C,0x126CBD30,0x4FE464C2,0x8D3FC57D,0x4D359372,0x87BDE68F,0x33D6995A,0x55D237D5,0x79FD048B,
 0x2A90A7FF,0xF7EB839A,0x6A1CB87F,0x5E0872B5,0xB21EE1FB,0x305CD8CB,0xF34C6B9A,0x79B90D33,0xDF456CD4,0xAD07CE21,0xDE336CE7,0xB53B627F,0x9405B8E8,0x1B5F6C50,0xDE91C915,0xCF7940C3,0x3};

//generator g of Zn*
//= 0x1F91B2B3 7B136F38 86DF5C68 71551EB9 C8977ADE 58B77693 999A30D8 AD11FC00 43D9AD55 451DFC2F 805D46DE A4B5D3B6 31AC841C 8D46CAE8 CDD40E1E E8E036A6 
//    C6ACDAD4 E7F06523 203195FD 78A9D541 240A18AF E024DC9E 0F241017 E99D5904 5D8D3B8C B3D91859 A71F027E 22773B0B 14057D59 383C4EAA D0B49382 BBFA5665
const static uint32 g[32] = 
{0xBBFA5665,0xD0B49382,0x383C4EAA,0x14057D59,0x22773B0B,0xA71F027E,0xB3D91859,0x5D8D3B8C,0xE99D5904,0x0F241017,0xE024DC9E,0x240A18AF,0x78A9D541,0x203195FD,0xE7F06523,0xC6ACDAD4,
 0xE8E036A6,0xCDD40E1E,0x8D46CAE8,0x31AC841C,0xA4B5D3B6,0x805D46DE,0x451DFC2F,0x43D9AD55,0xAD11FC00,0x999A30D8,0x58B77693,0xC8977ADE,0x71551EB9,0x86DF5C68,0x7B136F38,0x1F91B2B3};

//Pre-computation table for m-ary method
const static uint32 T[14][32] = {
//g^0
//{0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
// 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0},
//g^1
//{0xBBFA5665,0xD0B49382,0x383C4EAA,0x14057D59,0x22773B0B,0xA71F027E,0xB3D91859,0x5D8D3B8C,0xE99D5904,0x0F241017,0xE024DC9E,0x240A18AF,0x78A9D541,0x203195FD,0xE7F06523,0xC6ACDAD4,
// 0xE8E036A6,0xCDD40E1E,0x8D46CAE8,0x31AC841C,0xA4B5D3B6,0x805D46DE,0x451DFC2F,0x43D9AD55,0xAD11FC00,0x999A30D8,0x58B77693,0xC8977ADE,0x71551EB9,0x86DF5C68,0x7B136F38,0x1F91B2B3},
//g^2
//0x22833450 6BEC56DD FAD1A01B B327AFEF 321D15E4 2FB924AD 804C1710 C36F97CD 29602994 52EF3E68 399F0DF6 1D46EABA 97E88F3C 0AB115FF B38BB314 EE0EAEBD  
//  554FC85A 7F1D5422 C556CA75 733CF7C6 846C282B 8D7BE1F6 A0BB999A 34B28069 A12F2814 338B7836 A32C99DE 15C53FD8 3EE86F95 E1967D39 2092ED9D 55B9FACD
{0x55B9FACD,0x2092ED9D,0xE1967D39,0x3EE86F95,0x15C53FD8,0xA32C99DE,0x338B7836,0xA12F2814,0x34B28069,0xA0BB999A,0x8D7BE1F6,0x846C282B,0x733CF7C6,0xC556CA75,0x7F1D5422,0x554FC85A,
 0xEE0EAEBD,0xB38BB314,0x0AB115FF,0x97E88F3C,0x1D46EABA,0x399F0DF6,0x52EF3E68,0x29602994,0xC36F97CD,0x804C1710,0x2FB924AD,0x321D15E4,0xB327AFEF,0xFAD1A01B,0x6BEC56DD,0x22833450},
//g^3
//0x1A375455 00383C5A C6253464 0CC29509 ACEFAEEE 1DFECE56 3CB89DC5 89690E43 1EDB77BE 37FD3CF1 B8D12E52 15D991FB 170494D5 7E2D972D 2DA7320B 054E08BB 
//  5009A603 105C953F 0DF627C8 9031F3E2 C087615D 5EC7CB25 F1244092 7C6A1922 A279A951 18728672 0759029D 0E3C8ACD EE62FB70 C86E669E 1E24884B 4759BCA7
{0x4759BCA7,0x1E24884B,0xC86E669E,0xEE62FB70,0x0E3C8ACD,0x0759029D,0x18728672,0xA279A951,0x7C6A1922,0xF1244092,0x5EC7CB25,0xC087615D,0x9031F3E2,0x0DF627C8,0x105C953F,0x5009A603,
 0x054E08BB,0x2DA7320B,0x7E2D972D,0x170494D5,0x15D991FB,0xB8D12E52,0x37FD3CF1,0x1EDB77BE,0x89690E43,0x3CB89DC5,0x1DFECE56,0xACEFAEEE,0x0CC29509,0xC6253464,0x00383C5A,0x1A375455},
//g^4
//0x00EDB7B4 B6D026D7 DEF6CAA4 B9213E27 67EF4235 097D7EC8 43F028A5 C7CB11BA 3E03EA91 18A49D8F 2A3C3BD9 A245C1AE 4B7EDDEE 5C5BEB57 2AFB37F5 FAFCCE51 
//  689192D1 93E15A4D 2F516F73 DE32C8F3 CF2D019F 009B10DB E5326B8C 6C5670A4 2CB81B21 D86F8699 65A7F719 E01A2712 550B3E78 2BCE65D4 76EF0B11 C37A2560
{0xC37A2560,0x76EF0B11,0x2BCE65D4,0x550B3E78,0xE01A2712,0x65A7F719,0xD86F8699,0x2CB81B21,0x6C5670A4,0xE5326B8C,0x009B10DB,0xCF2D019F,0xDE32C8F3,0x2F516F73,0x93E15A4D,0x689192D1,
 0xFAFCCE51,0x2AFB37F5,0x5C5BEB57,0x4B7EDDEE,0xA245C1AE,0x2A3C3BD9,0x18A49D8F,0x3E03EA91,0xC7CB11BA,0x43F028A5,0x097D7EC8,0x67EF4235,0xB9213E27,0xDEF6CAA4,0xB6D026D7,0x00EDB7B4},
//g^5
//0x3B609906 FBF372BD 4D4E9915 840A3708 15DDBC07 CD630E0C 299CEEAA 28003570 7FEA51D5 6833E9C2 30B0C928 92BC8C3C 87D99F7D 795CF275 ABF3E943 D21734FC 
//  1373E0C3 7535FEBA 76E54B65 5C1D2962 E527ABE1 05D73AE4 9F177F80 5A247E66 B4BBEC7F 368FE84E 4E167C47 90DC40BF 779103CA A0544FE4 C556166E 4F8ED734
{0x4F8ED734,0xC556166E,0xA0544FE4,0x779103CA,0x90DC40BF,0x4E167C47,0x368FE84E,0xB4BBEC7F,0x5A247E66,0x9F177F80,0x05D73AE4,0xE527ABE1,0x5C1D2962,0x76E54B65,0x7535FEBA,0x1373E0C3,
 0xD21734FC,0xABF3E943,0x795CF275,0x87D99F7D,0x92BC8C3C,0x30B0C928,0x6833E9C2,0x7FEA51D5,0x28003570,0x299CEEAA,0xCD630E0C,0x15DDBC07,0x840A3708,0x4D4E9915,0xFBF372BD,0x3B609906},
//g^6
//0x020FDFA8 EC81D3D8 486E3578 53FAD1DF AEE2540A 51E85E65 9720BBCC 221BDF70 47B7CFFD C9550D44 14F2B2C8 5DEE1A35 8B6758CA A86F2817 9FB63C9F DEFE1DBD 
//  B1C8C6D9 2B882229 A88A1A46 41CE194F 4B752924 C216E07B 4E648F10 8A96E8C7 D3834A63 A3D10725 4986212D A92A376E 6B631256 E8AE0E3F 8E8F37BA B8D3BF93
{0xB8D3BF93,0x8E8F37BA,0xE8AE0E3F,0x6B631256,0xA92A376E,0x4986212D,0xA3D10725,0xD3834A63,0x8A96E8C7,0x4E648F10,0xC216E07B,0x4B752924,0x41CE194F,0xA88A1A46,0x2B882229,0xB1C8C6D9,
 0xDEFE1DBD,0x9FB63C9F,0xA86F2817,0x8B6758CA,0x5DEE1A35,0x14F2B2C8,0xC9550D44,0x47B7CFFD,0x221BDF70,0x9720BBCC,0x51E85E65,0xAEE2540A,0x53FAD1DF,0x486E3578,0xEC81D3D8,0x020FDFA8},
//g^7
//0x030B4CC6 B9CAC548 FC968DCB 00E39F79 E93FC639 A52A3805 57DA639B 42007616 62454604 64702727 B8EF8E57 93659A35 E288D1ED 976F5770 AB784CB6 5B2960A2 
//  10B8C855 5A30B95D 7958992B 73E83B79 2488AB23 0F1002D7 9978481B 058EA451 340BD632 F239A848 9B90FFFA 656F7D26 82E622BD F6209436 49323D4A 2DA59D40
{0x2DA59D40,0x49323D4A,0xF6209436,0x82E622BD,0x656F7D26,0x9B90FFFA,0xF239A848,0x340BD632,0x058EA451,0x9978481B,0x0F1002D7,0x2488AB23,0x73E83B79,0x7958992B,0x5A30B95D,0x10B8C855,
 0x5B2960A2,0xAB784CB6,0x976F5770,0xE288D1ED,0x93659A35,0xB8EF8E57,0x64702727,0x62454604,0x42007616,0x57DA639B,0xA52A3805,0xE93FC639,0x00E39F79,0xFC968DCB,0xB9CAC548,0x030B4CC6},
//g^8
//0x11C5CA9D 18C31D6A 4E9D740E 5372D589 657571DA A83C9908 0A6B47FB 9FE2C440 F0062DDF 6B97DF07 B8D6DBB3 818ACD90 31E68CCA 4071FEF5 753199FD A4CFA6E4 
//  5DE1B6E9 F9BEA956 110118BD EFBBF44F D52919DE 968EC248 33C4A991 AA855247 33657644 93C23BD9 DA245195 401BBA48 5767AE41 6B489181 477DC73E 5DEFA49D
{0x5DEFA49D,0x477DC73E,0x6B489181,0x5767AE41,0x401BBA48,0xDA245195,0x93C23BD9,0x33657644,0xAA855247,0x33C4A991,0x968EC248,0xD52919DE,0xEFBBF44F,0x110118BD,0xF9BEA956,0x5DE1B6E9,
 0xA4CFA6E4,0x753199FD,0x4071FEF5,0x31E68CCA,0x818ACD90,0xB8D6DBB3,0x6B97DF07,0xF0062DDF,0x9FE2C440,0x0A6B47FB,0xA83C9908,0x657571DA,0x5372D589,0x4E9D740E,0x18C31D6A,0x11C5CA9D},
//g^9
//0x1B251AE4 317C7B6E B0CD7618 5643203F E6C3ACC9 F3B31CE3 3D1FEF7D A5068A4A A5C36FA9 31CA4CA9 DDF907E0 BC416B8F 6122EE67 08F6DB49 72B93EB2 37989AFB 
//  E59437F8 5FFC860D FBE8C4BB 20EE627F 427DAFBC B5371DF8 1BDCE4A8 EED616D4 EAF2851C 1C2C1C98 A365C676 849F1B3F FDDF2A2B EA8907FB 53338487 09018672
{0x09018672,0x53338487,0xEA8907FB,0xFDDF2A2B,0x849F1B3F,0xA365C676,0x1C2C1C98,0xEAF2851C,0xEED616D4,0x1BDCE4A8,0xB5371DF8,0x427DAFBC,0x20EE627F,0xFBE8C4BB,0x5FFC860D,0xE59437F8,
 0x37989AFB,0x72B93EB2,0x08F6DB49,0x6122EE67,0xBC416B8F,0xDDF907E0,0x31CA4CA9,0xA5C36FA9,0xA5068A4A,0x3D1FEF7D,0xF3B31CE3,0xE6C3ACC9,0x5643203F,0xB0CD7618,0x317C7B6E,0x1B251AE4},
//g^10
//0x013D34E0 94AB9AE4 52AF02BD FA434152 0F4D48FC 6931CF98 CB4442D4 9C441E59 ED0422C5 5A6E338A 6587F6B7 0F84FE10 9D5B17F4 663F4FC6 BF3DF267 9D5E7D93 
//  8DCB5188 3B71F86F 7078BF9D 48567AA9 082E1E9E 84A6F631 952E8FF1 5E043894 AB8DA4C6 BCA0881D 8ADEFF49 17778879 4DC52C9D 3C67E398 E1A73A11 A9D010F6
{0xA9D010F6,0xE1A73A11,0x3C67E398,0x4DC52C9D,0x17778879,0x8ADEFF49,0xBCA0881D,0xAB8DA4C6,0x5E043894,0x952E8FF1,0x84A6F631,0x082E1E9E,0x48567AA9,0x7078BF9D,0x3B71F86F,0x8DCB5188,
 0x9D5E7D93,0xBF3DF267,0x663F4FC6,0x9D5B17F4,0x0F84FE10,0x6587F6B7,0x5A6E338A,0xED0422C5,0x9C441E59,0xCB4442D4,0x6931CF98,0x0F4D48FC,0xFA434152,0x52AF02BD,0x94AB9AE4,0x013D34E0},
//g^11
//0x2267F961 B7649684 0200FFDD C9B7C6A5 D766FC8A 0F62AD77 0DC3FE3F B7D6DC09 6AACD2BB C0ED4703 5DF40AC3 451109B8 C70ADA47 6CC18459 3C67CD09 B64CE206 
//  B488CFBA CDA57499 79FAEA57 FEF565EE 2EE29504 33A8751B C1900599 2700552E 4A06E6FB 99EACED0 B35F1F91 1F4D573E 0CAEAFB0 5BAA1066 59CBF7BD 7A87294B
{0x7A87294B,0x59CBF7BD,0x5BAA1066,0x0CAEAFB0,0x1F4D573E,0xB35F1F91,0x99EACED0,0x4A06E6FB,0x2700552E,0xC1900599,0x33A8751B,0x2EE29504,0xFEF565EE,0x79FAEA57,0xCDA57499,0xB488CFBA,
 0xB64CE206,0x3C67CD09,0x6CC18459,0xC70ADA47,0x451109B8,0x5DF40AC3,0xC0ED4703,0x6AACD2BB,0xB7D6DC09,0x0DC3FE3F,0x0F62AD77,0xD766FC8A,0xC9B7C6A5,0x0200FFDD,0xB7649684,0x2267F961},
//g^12
//0x401CAEE7 25438428 A66E16EB E33B18AD 706031A4 66C0234A E0F4D0E7 23104F0F CA8895CB FED5FBD3 060C7BAC 3157580F BF1B9132 F7085497 1DF097F3 BF9196E7 
//  AD1A74BE 8E239466 32A304F7 6879BBB1 8D400282 FA3623C1 4F5F530C 6FCD9478 CF7860CF BF838FED 6EEB8F60 D1CF8287 3D6EC4A6 1F00E475 5FFF806C 41ADC894
{0x41ADC894,0x5FFF806C,0x1F00E475,0x3D6EC4A6,0xD1CF8287,0x6EEB8F60,0xBF838FED,0xCF7860CF,0x6FCD9478,0x4F5F530C,0xFA3623C1,0x8D400282,0x6879BBB1,0x32A304F7,0x8E239466,0xAD1A74BE,
 0xBF9196E7,0x1DF097F3,0xF7085497,0xBF1B9132,0x3157580F,0x060C7BAC,0xFED5FBD3,0xCA8895CB,0x23104F0F,0xE0F4D0E7,0x66C0234A,0x706031A4,0xE33B18AD,0xA66E16EB,0x25438428,0x401CAEE7},
//g^13
//0x064DB23D AA6240FA 11CA7C54 EFE83734 45467462 2B26EF4F E3807228 E6F35BCB 0E210A8B 020C9146 C5BE2678 67EDA620 5332A28C 0A8D72D4 7356127E FBB26017 
//  98C7CA60 40164150 D37E1F2D 2B06A928 0B7A8256 FBDD2D78 95D15CD7 7AC91800 A237C389 F956939D C2F01647 D375A7CF D64F47C1 D74316D2 EF647F9A FB30A4A5
{0xFB30A4A5,0xEF647F9A,0xD74316D2,0xD64F47C1,0xD375A7CF,0xC2F01647,0xF956939D,0xA237C389,0x7AC91800,0x95D15CD7,0xFBDD2D78,0x0B7A8256,0x2B06A928,0xD37E1F2D,0x40164150,0x98C7CA60,
 0xFBB26017,0x7356127E,0x0A8D72D4,0x5332A28C,0x67EDA620,0xC5BE2678,0x020C9146,0x0E210A8B,0xE6F35BCB,0xE3807228,0x2B26EF4F,0x45467462,0xEFE83734,0x11CA7C54,0xAA6240FA,0x064DB23D},
//g^14
//0x0BAD2272 C55CBACD 5D531145 D3816727 127259E8 6B241D99 3D9F7FA0 0053427F C34D29D3 53F7BDFF 1E8B4BB0 3C6BFA6C AC686653 CD437C61 C0E01168 762DB944 
//  3982F72E 17F7D367 CB38836F EB3F0C50 36A193B3 76C26DF2 6B1B1F70 65E549DA 329B0D4E 86AA15B3 543296D1 C3E9EFA4 DFA7E051 EFAECBBD 2A0D6A00 8367B892
{0x8367B892,0x2A0D6A00,0xEFAECBBD,0xDFA7E051,0xC3E9EFA4,0x543296D1,0x86AA15B3,0x329B0D4E,0x65E549DA,0x6B1B1F70,0x76C26DF2,0x36A193B3,0xEB3F0C50,0xCB38836F,0x17F7D367,0x3982F72E,
 0x762DB944,0xC0E01168,0xCD437C61,0xAC686653,0x3C6BFA6C,0x1E8B4BB0,0x53F7BDFF,0xC34D29D3,0x0053427F,0x3D9F7FA0,0x6B241D99,0x127259E8,0xD3816727,0x5D531145,0xC55CBACD,0x0BAD2272},
//g^15
//0x0952C448 5443DEFF B302F5C7 E078DEA5 6149CCB6 5A7BA802 339516BE 19AFB996 706004B7 D380138F 54CD8C2B 4A54BF4B 1E656143 EE5974F2 BACE8926 74650FE6
//  44A79A19 71A3F47F 609CB653 180AA9C8 FA3C81A0 54186C4A A2063B67 7C8B7D0A F6F91D8D BC977704 0449373E E4044435 DC3E2C55 AFA3DF54 CD6DC9CE E08A707A
{0xE08A707A,0xCD6DC9CE,0xAFA3DF54,0xDC3E2C55,0xE4044435,0x0449373E,0xBC977704,0xF6F91D8D,0x7C8B7D0A,0xA2063B67,0x54186C4A,0xFA3C81A0,0x180AA9C8,0x609CB653,0x71A3F47F,0x44A79A19,
 0x74650FE6,0xBACE8926,0xEE5974F2,0x1E656143,0x4A54BF4B,0x54CD8C2B,0xD380138F,0x706004B7,0x19AFB996,0x339516BE,0x5A7BA802,0x6149CCB6,0xE078DEA5,0xB302F5C7,0x5443DEFF,0x0952C448}};


static uint32 sub(uint32 *a, uint32 *b, uint32 *asubb, uint32 m)
{
	uint32 i, borrow;
	uint64 T;

	borrow = 1; 
	for(i = 0; i < m; i++)
	{
		T = (uint64)*(a + i) + ~*(b + i) + borrow;
		*(asubb + i) = (uint32)T;
		borrow = (uint32)(T >> 32);
	}
	if(borrow == 0) return 1;     
	else return 0;
}

static sint32 compare(uint32 *a, uint32 *b, uint32 m)
{
	uint32 i;

	for(i = m; i != 0; i--)
	{
		if(*(a + i - 1) < *(b + i - 1)) return -1;  
		if(*(a + i - 1) > *(b + i - 1)) return 1;  
	}
	return 0;                                      
}

static void intmul(uint32 *a, const uint32 *b, uint32 *ab)
{
	uint32 i, j, k;
	uint32 R0, R1, R2, carry;
	uint64 T1, T2;

	R0 = 0; 
	R1 = 0; 
	R2 = 0;

	for(k = 0; k <= 62; k++)
	{
		for(i = (k <= 31 ? 0 : (k - 31)); i <= (k <= 31 ? k : 31); i++)
		{
			j = k - i;
			T1 = (uint64)(*(a + i)) *(*(b + j));
			//(carry, R0) = R0 + V
			T2 = (T1 & 0xFFFFFFFF) + R0;
			carry = (uint32)(T2 >> 32);
			R0 = (uint32)T2 & 0xFFFFFFFF;
			//(carry, R1) = R1 + U + carry
			T2 = (T1 >> 32) + R1 + carry;
			carry = (uint32)(T2 >> 32);
			R1 = (uint32)T2 & 0xFFFFFFFF;
			//R2 = R2 + carry
			R2 = R2 + carry;
		}
		*(ab + k) = R0;
		R0 = R1;
		R1 = R2;
		R2 = 0;
	}
	*(ab + 63) = R0;
}


static void intsq(uint32 *a, uint32 *aa)
{
	uint32 i, j, k;
	uint32 R0, R1, R2, carry;
	uint64 T1, T2;

	R0 = 0; 
	R1 = 0; 
	R2 = 0;

	for(k = 0; k <= 62; k++)
	{
		for(i = (k <= 31 ? 0 : (k - 31)); i <= (k >> 1); i++)
		{
			j = k - i;
			T1 = (uint64)(*(a + i)) *(*(a + j));
			if(i != j)
			{
				carry = (uint32)(T1 >> 63);
				T1 = T1 << 1;
				R2 += carry;
			}
			//(carry, R0) = R0 + V
			T2 = (T1 & 0xFFFFFFFF) + R0;
			carry = (uint32)(T2 >> 32);
			R0 = (uint32)T2 & 0xFFFFFFFF;
			//(carry, R1) = R1 + U + carry
			T2 = (T1 >> 32) + R1 + carry;
			carry = (uint32)(T2 >> 32);
			R1 = (uint32)T2 & 0xFFFFFFFF;
			//R2 = R2 + carry
			R2 = R2 + carry;
		}
		*(aa + k) = R0;
		R0 = R1;
		R1 = R2;
		R2 = 0;
	}
	*(aa + 63) = R0;
}

//Barrett Modular Reduction
static void modn(uint32 *c, uint32 *b)
{
	uint32 i, j;
	uint64 T1, T2, T3;
	uint32 r[33], s[33];

	T1 = (uint64)c[31] * u[0];
	T1 = T1 >> 32;

	for(i = 32; i < 64; i++)
	{
		T3 = 0;
		for(j = 31; j <= i; j++)
		{
			T2 = (uint64)c[j] * u[i - j];
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		T1 = (T1 >> 32) + T3;
	}

	for(i = 0; i < 31; i++)
	{
		T3 = 0;
		for(j = i + 32; j < 64; j++)
		{
			T2 = (uint64)c[j] * u[64 + i - j];
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		r[i] = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	T2 = (uint64)c[63] * u[32];
	T1 += (uint32)T2;
	r[31] = (uint32)T1;
	r[32] = (uint32)(T1 >> 32) + (uint32)(T2 >> 32);

	//s = r * n
	T1 = (uint64)r[0] * n[0];
	s[0] = (uint32)T1;
	T1 = T1 >> 32;

	for(i = 1; i < 33; i++)
	{
		T3 = 0;
		for(j = (i < 32 ? 0 : 1); j <= i; j++)
		{
			T2 = (uint64)r[j] * n[i - j];
			T1 += (uint32)T2;
			T3 += (T2 >> 32);
		}
		s[i] = (uint32)T1;
		T1 = (T1 >> 32) + T3;
	}

	for(i = 31; i != 0; i--)  
		r[i] = n[i];
	r[0] = n[0];
	r[32] = 0;

	if(sub(c, s, s, 33) == 1)                 
	{
		for(i = 31; i != 0; i--)  
			*(b + i) = *(s + i);
		*b = *s;
		return;
	}

	while(compare(s, r, 33) != -1)  
		sub(s, r, s, 33);
	for(i = 31; i != 0; i--)  
		*(b + i) = *(s + i);
	*b = *s;
}


//Compute a^2 mod n
static void modnsq(uint32 *a, uint32 *asq)
{
	uint32 c[64];

	intsq(a, c);
	modn(c, asq);
}

//Compute a*b mod n
static void modnmul(uint32 *a, const uint32 *b, uint32 *amulb)
{
	uint32 c[64];

	intmul(a, b, c);
	modn(c, amulb);
}

//Compute public key hash function
void pkhash_slow(const uint8_t *in, const size_t len, uint8_t *out)
{
	uint32 i, j;
	uint8 b, bh, bl;
	uint32 d[32];   
    // uint8 tmp;

    uint8_t msg[256] = {0};
    // reverse for little-endian
    for (i = 0; i < len; i++) {
        msg[len - i - 1] = in[i];
    }

	//Extract the first byte
	b = *msg;
	bh = b >> 4;
	bl = b & 0xF;
	//Initialization
	if(bh == 0)
		XIS1(d);
	if(bh == 1)
		XTOY(d, g);
	for(i = 2; i < 16; i++)
	{
		if(bh == i)
		{
			XTOY(d, T[i - 2]);
			break;
		}
	}

	if(bh != 0)
	{
		modnsq(d, d);
		modnsq(d, d);
		modnsq(d, d);
		modnsq(d, d);

		if(bl != 0)
		{
			if(bl == 1)
				modnmul(d, g, d);
			for(i = 2; i < 16; i++)
			{
				if(bl == i)
				{
					modnmul(d, T[i - 2], d);
					break;
				}
			}
		}
	}
	else
	{
		if(bl != 0)
		{
			if(bl == 1)
				XTOY(d, g);
			for(i = 2; i < 16; i++)
			{
				if(bl == i)
				{
					XTOY(d, T[i - 2]);
					break;
				}
			}
		}
	}

	for(i = 1; i < len; i++)
	{
		//Extract the i-th byte
		b = *(msg + i);
		bh = b >> 4;
		bl = b & 0xF;
		//Compute exponentiation using the m-ary method
		modnsq(d, d);
		modnsq(d, d);
		modnsq(d, d);
		modnsq(d, d);
		if(bh != 0)
		{
			if(bh == 1)
				modnmul(d, g, d);
			for(j = 2; j < 16; j++)
			{
				if(bh == j)
				{
					modnmul(d, T[j - 2], d);
					break;
				}
			}
		}
		modnsq(d, d);
		modnsq(d, d);
		modnsq(d, d);
		modnsq(d, d);
		if(bl != 0)
		{
			if(bl == 1)
				modnmul(d, g, d);
			for(j = 2; j < 16; j++)
			{
				if(bl == j)
				{
					modnmul(d, T[j - 2], d);
					break;
				}
			}
		}
	}

	// for(i = 0; i < 32; i++)
	//	out[i] = d[i];
    memcpy(out, d, 128);
}

/*
//Test
int main()
{
	uint8 msg[32] = {0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF6,0xF5,0xF4,0xF3,0xF2,0xF1,0xF0,
	                 0xFF,0xFE,0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF6,0xF5,0xF4,0xF3,0xF2,0xF1,0xF0};
	// uint32 digest[32] = {0};
	uint8 digest[128] = {0};
	int i;

	pkhash_slow(msg, sizeof(msg) / sizeof(uint8), digest);
	printf("The message digest is:\n");

	for(i = 0; i < 128; i++)
		printf("%02x", (unsigned int)digest[i]);
	printf("\n");

    return 0;
}	
*/