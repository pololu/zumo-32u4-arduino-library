// Copyright Pololu Corporation.  For more information, see http://www.pololu.com/

#include <PololuHD44780.h>

#define LCD_CLEAR       0x01
#define LCD_SHOW_BLINK  0x0F
#define LCD_SHOW_SOLID  0x0E
#define LCD_HIDE        0x0C
#define LCD_CURSOR_L    0x10
#define LCD_CURSOR_R    0x14
#define LCD_SHIFT_L     0x18
#define LCD_SHIFT_R     0x1C

PololuHD44780Base::PololuHD44780Base()
{
    initialized = false;
}

void PololuHD44780Base::init2()
{
    // The startup procedure comes from Figure 24 of the HD44780 datasheet.  The
    // delay times in the later part of this function come from Table 6.

    initPins();

    // We need to wait at least 15 ms after VCC reaches 4.5 V.
    //
    // Assumption: The AVR's power-on reset is already configured to wait for
    // tens of milliseconds, so no delay is needed here.

    sendCommand4Bit(3);        // Function set
    _delay_us(4200);           // Needs to be at least 4.1 ms.
    sendCommand4Bit(3);        // Function set
    _delay_us(150);            // Needs to be at least 100 us.
    sendCommand4Bit(3);        // Function set

    sendCommand4Bit(0b0010);   // 4-bit interface
    sendCommand(0b00101000);   // 4-bit, 2 line, 5x8 dots font

    setDisplayControl(0b000);  // display off, cursor off, blinking off
    clear();
    setEntryMode(0b10);        // cursor shifts right, no auto-scrolling
    setDisplayControl(0b100);  // display on, cursor off, blinking off
}

void PololuHD44780Base::sendAndDelay(uint8_t data, bool rsValue, bool only4bit)
{
    init();

    send(data, rsValue, only4bit);

    // Every data transfer or command takes at least 37 us to complete, and most
    // of them only take that long according to the HD44780 datasheet.  We delay
    // for 37 us here so we don't have to do it in lots of other places.
    //
    // NOTE: If we add support for configurations where the R/W line is
    // connected, then this delay and others like it should be disabled, and we
    // should instead wait for the busy flag before sending the next command.
    _delay_us(37);
}

size_t PololuHD44780Base::write(uint8_t data)
{
    sendData(data);
    return 1;
}

size_t PololuHD44780Base::write(const uint8_t * buffer, size_t length)
{
    size_t n = length;
    while (n--)
    {
        sendData(*buffer++);
    }
    return length;
}

void PololuHD44780Base::clear()
{
    sendCommand(LCD_CLEAR);

    // It's not clear how long this command takes because it doesn't say in
    // Table 6 of the HD44780 datasheet.  A good guess is that it takes 1.52 ms,
    // since the Return Home command does.
    _delay_us(2000);
}

void PololuHD44780Base::gotoXY(uint8_t x, uint8_t y)
{
    // Each entry is the RAM address of a line, with its most
    // significant bit set for convenience.
    const uint8_t line_mem[] = {0x80, 0xC0, 0x94, 0xD4};

    // Avoid out-of-bounds array access.
    if (y > 3) { y = 3; }

    sendCommand(line_mem[y] + x);

    // This could take up to 37 us according to Table 6 of the HD44780 datasheet.
    _delay_us(37);
}

void PololuHD44780Base::loadCustomCharacter(const uint8_t * picture, uint8_t number)
{
    uint8_t address = number * 8;

    for(uint8_t i = 0; i < 8; i++)
    {
        // Set CG RAM address.
        sendCommand(0b01000000 | (address + i));

        // Write character data.
        sendData(pgm_read_byte(picture + i));
    }
}

void PololuHD44780Base::loadCustomCharacterFromRam(const uint8_t * picture, uint8_t number)
{
    uint8_t address = number * 8;

    for(uint8_t i = 0; i < 8; i++)
    {
        // Set CG RAM address.
        sendCommand(0b01000000 | (address + i));

        // Write character data.
        sendData(picture[i]);
    }
}

void PololuHD44780Base::setDisplayControl(uint8_t displayControl)
{
    sendCommand(0b00001000 | displayControl);
    this->displayControl = displayControl;
}

void PololuHD44780Base::cursorSolid()
{
    setDisplayControl((displayControl | 0b010) & ~0b001);
}

void PololuHD44780Base::cursorBlinking()
{
    setDisplayControl((displayControl | 0b001) & ~0b010);
}

void PololuHD44780Base::hideCursor()
{
    setDisplayControl(displayControl & ~0b011);
}

void PololuHD44780Base::noDisplay()
{
    setDisplayControl(displayControl & ~0b100);
}

void PololuHD44780Base::display()
{
    setDisplayControl(displayControl | 0b100);
}

void PololuHD44780Base::noCursor()
{
    setDisplayControl(displayControl & ~0b010);
}

void PololuHD44780Base::cursor()
{
    setDisplayControl(displayControl | 0b010);
}

void PololuHD44780Base::noBlink()
{
    setDisplayControl(displayControl & ~0b001);
}

void PololuHD44780Base::blink()
{
    setDisplayControl(displayControl | 0b001);
}

void PololuHD44780Base::scrollDisplayLeft()
{
    sendCommand(0b00011000);
}

void PololuHD44780Base::scrollDisplayRight()
{
    sendCommand(0b00011100);
}

void PololuHD44780Base::home()
{
    sendCommand(0b00000010);
    _delay_us(1600); // needs to be at least 1.52 ms
}

void PololuHD44780Base::setEntryMode(uint8_t entryMode)
{
    sendCommand(0b00000100 | entryMode);
    this->entryMode = entryMode;
}

void PololuHD44780Base::leftToRight()
{
    setEntryMode(entryMode | 0b10);
}

void PololuHD44780Base::rightToLeft()
{
    setEntryMode(entryMode & ~0b10);
}

void PololuHD44780Base::autoscroll()
{
    setEntryMode(entryMode | 0b01);
}

void PololuHD44780Base::noAutoscroll()
{
    setEntryMode(entryMode & ~0b01);
}
