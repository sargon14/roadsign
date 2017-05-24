#include "Serial5/Serial5.h"
#include "buf.h"

#define DISPLAY_WIDTH   96
#define DISPLAY_HEIGHT  27
#define DISPLAY_SIZE    (DISPLAY_WIDTH * DISPLAY_HEIGHT)

#define PANEL_WIDTH     24
#define PANEL_HEIGHT    9
#define PANEL_SIZE      (PANEL_WIDTH * PANEL_HEIGHT)
#define BLOCK_SIZE      (DISPLAY_WIDTH * PANEL_HEIGHT)

unsigned char SIGN_INIT[] = {0xFE, 0x6C, 0x00};
unsigned char SIGN_SYNC[] = {0xFE, 0xAA, 0x00};

unsigned char SignFrameBuffer[DISPLAY_SIZE / 8];

SYSTEM_MODE(MANUAL);

void setup()
{
    Serial5.begin(57600);

    delay(1000);
    SerialPrintSignInit();

    delay(1000);
    EncodeFramebuffer();
    SerialPrintSignBuffer();
}

void loop()
{
    static bool connecting = false;

    if (Particle.connected())
    {
        Particle.process();
        delay(1000);
        EncodeFramebuffer();
        SerialPrintSignBuffer();
    }
    else
    {
        if (!connecting)
        {
            Particle.connect();
            connecting = true;
        }
    }
}

void SerialPrintSignInit()
{
    RS485Out(SIGN_INIT, sizeof(SIGN_INIT));
}

void SerialPrintSignSync()
{
    RS485Out(SIGN_SYNC, sizeof(SIGN_SYNC));
}

void SerialPrintSignBuffer()
{
    SerialPrintSignSync();
    RS485Out(SignFrameBuffer, sizeof(SignFrameBuffer));
}

//int CartesianToPixelOffset(int x, int y)
//{
//    return ((y / PANEL_HEIGHT) * BLOCK_SIZE) + ((y % PANEL_HEIGHT) * PANEL_WIDTH) + (((x / PANEL_WIDTH) * PANEL_SIZE) + PANEL_WIDTH - (x % PANEL_WIDTH)) - 1;
//}

int CartesianIndexToSignIndex(int offset)
{
    int x1 = (offset/PANEL_WIDTH) % (DISPLAY_WIDTH/PANEL_WIDTH);
    int y1 = offset / BLOCK_SIZE;
    int x2 = PANEL_WIDTH - 1 - (offset % PANEL_WIDTH);
    int y2 = (offset / DISPLAY_WIDTH) % PANEL_HEIGHT;

    return x2 + PANEL_WIDTH*y2 + PANEL_SIZE*x1 + BLOCK_SIZE*y1;
}

void EncodeFramebuffer()
{
  int i;
  int offset;

  memset(SignFrameBuffer, 0, sizeof(SignFrameBuffer));

  for (i = 0; i < DISPLAY_SIZE; i++)
  {
      offset = CartesianIndexToSignIndex(i);
      SignFrameBuffer[offset / 8] |= (buf[i] ? 1 : 0) << (i % 8);
  }
}

void RS485Out(unsigned char *data, int len)
{
    Serial5.write(data, len);
}
