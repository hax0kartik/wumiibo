
/*
*   This file is part of Luma3DS
*   Copyright (C) 2016-2020 Aurora Wright, TuxSH
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include <3ds.h>
#include <stdarg.h>
#include "fmt.h"
#include "draw.h"
#include "font.h"
#include "utils.h"
#include "csvc.h"

static u32 gpuSavedFramebufferAddr1, gpuSavedFramebufferAddr2, gpuSavedFramebufferFormat, gpuSavedFramebufferStride;
static u32 framebufferCacheSize;
static void *framebufferCache;
static RecursiveLock lock;

void Draw_Init(void)
{
   RecursiveLock_Init(&lock);
}

void Draw_Lock(void)
{
   RecursiveLock_Lock(&lock);
}

void Draw_Unlock(void)
{
   RecursiveLock_Unlock(&lock);
}

void Draw_DrawCharacter(u32 posX, u32 posY, u32 color, char character)
{
   u16 *const fb = (u16 *)FB_BOTTOM_VRAM_ADDR;

   s32 y;
   for(y = 0; y < 10; y++)
   {
      char charPos = font[character * 10 + y];

      s32 x;
      for(x = 6; x >= 1; x--)
      {
         u32 screenPos = (posX * SCREEN_BOT_HEIGHT * 2 + (SCREEN_BOT_HEIGHT - y - posY - 1) * 2) + (5 - x) * 2 * SCREEN_BOT_HEIGHT;
         u32 pixelColor = ((charPos >> x) & 1) ? color : COLOR_BLACK;
         fb[screenPos / 2] = pixelColor;
      }
   }
}

int strlen16(uint16_t *str)
{
	int len = 0;
	while(str[len] != NULL)
		len++;
	return len;
}

u32 Draw_DrawString16(u32 posX, u32 posY, u32 color, uint16_t *string)
{
   for(u32 i = 0, line_i = 0; i < ((u32) strlen16(string)); i++)
   {
      if(string[i] == '\n')
      {
         posY += SPACING_Y;
         line_i = 0;
         continue;
      }
      else if(line_i >= (SCREEN_BOT_WIDTH - posX) / SPACING_X)
      {
         // Make sure we never get out of the screen.
         posY += SPACING_Y;
         line_i = 0;
         if(string[i] == ' ')
            continue; // Spaces at the start look weird
      }

      Draw_DrawCharacter(posX + line_i * SPACING_X, posY, color, string[i]);
      line_i++;
   }

   return posY;
}

u32 Draw_DrawString(u32 posX, u32 posY, u32 color, const char *string)
{
   for(u32 i = 0, line_i = 0; i < strlen(string); i++)
      switch(string[i])
      {
         case '\n':
            posY += SPACING_Y;
            line_i = 0;
            break;

         case '\t':
            line_i += 2;
            break;

         default:
            //Make sure we never get out of the screen
            if(line_i >= ((SCREEN_BOT_WIDTH) - posX) / SPACING_X)
            {
               posY += SPACING_Y;
               line_i = 1; //Little offset so we know the same string continues
               if(string[i] == ' ') break; //Spaces at the start look weird
            }

            Draw_DrawCharacter(posX + line_i * SPACING_X, posY, color, string[i]);

            line_i++;
            break;
      }
   return posY;
}

u32 Draw_DrawFormattedString(u32 posX, u32 posY, u32 color, const char *fmt, ...)
{
   char buf[DRAW_MAX_FORMATTED_STRING_SIZE + 1];
   va_list args;
   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);
   return Draw_DrawString(posX, posY, color, buf);
}

void Draw_FillFramebuffer(u32 value)
{
   memset(FB_BOTTOM_VRAM_ADDR, value, FB_BOTTOM_SIZE);
}

void Draw_ClearFramebuffer(void)
{
   Draw_FillFramebuffer(0);
}

Result Draw_AllocateFramebufferCache(u32 size)
{
   // Can't use fbs in FCRAM when Home Menu is active (AXI config related maybe?)
   u32 addr = 0x0D000000;
   u32 tmp;

   size = (size + 0xFFF) >> 12 << 12; // round-up

   if (framebufferCache != NULL)
      __builtin_trap();

   Result res = svcControlMemoryEx(&tmp, addr, 0, size, MEMOP_ALLOC, MEMREGION_SYSTEM | MEMPERM_READWRITE, true);
   if (R_FAILED(res))
   {
        framebufferCache = NULL;
        framebufferCacheSize = 0;
   }
    else
    {
        framebufferCache = (u32 *)addr;
        framebufferCacheSize = size;
    }

    return res;
}

void Draw_FreeFramebufferCache(void)
{
    u32 tmp;
    if (framebufferCache != NULL)
        svcControlMemory(&tmp, (u32)framebufferCache, 0, framebufferCacheSize, MEMOP_FREE, 0);
    framebufferCacheSize = 0;
    framebufferCache = NULL;
}

void *Draw_GetFramebufferCache(void)
{
    return framebufferCache;
}

u32 Draw_GetFramebufferCacheSize(void)
{
    return framebufferCacheSize;
}

u32 Draw_SetupFramebuffer(void)
{
   while((GPU_PSC0_CNT | GPU_PSC1_CNT | GPU_TRANSFER_CNT | GPU_CMDLIST_CNT) & 1);

   Draw_FlushFramebuffer();
   memcpy(framebufferCache, FB_BOTTOM_VRAM_ADDR, FB_BOTTOM_SIZE);
   Draw_ClearFramebuffer();
   Draw_FlushFramebuffer();

   u32 format = GPU_FB_BOTTOM_FMT;

   gpuSavedFramebufferAddr1 = GPU_FB_BOTTOM_ADDR_1;
   gpuSavedFramebufferAddr2 = GPU_FB_BOTTOM_ADDR_2;
   gpuSavedFramebufferFormat = format;
   gpuSavedFramebufferStride = GPU_FB_BOTTOM_STRIDE;

   format = (format & ~7) | GSP_RGB565_OES;
   format |= 3 << 8; // set VRAM bits

   GPU_FB_BOTTOM_ADDR_1 = GPU_FB_BOTTOM_ADDR_2 = FB_BOTTOM_VRAM_PA;
   GPU_FB_BOTTOM_FMT = format;
   GPU_FB_BOTTOM_STRIDE = 240 * 2;

   return framebufferCacheSize;
}

void Draw_RestoreFramebuffer(void)
{
   memcpy(FB_BOTTOM_VRAM_ADDR, framebufferCache, FB_BOTTOM_SIZE);
   Draw_FlushFramebuffer();

   GPU_FB_BOTTOM_ADDR_1 = gpuSavedFramebufferAddr1;
   GPU_FB_BOTTOM_ADDR_2 = gpuSavedFramebufferAddr2;
   GPU_FB_BOTTOM_FMT = gpuSavedFramebufferFormat;
   GPU_FB_BOTTOM_STRIDE = gpuSavedFramebufferStride;
}

void Draw_FlushFramebuffer(void)
{
   svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)FB_BOTTOM_VRAM_ADDR, FB_BOTTOM_SIZE);
}