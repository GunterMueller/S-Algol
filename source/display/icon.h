/****************************************************************************/
/*                                                                          */
/*  To get a new icon - create an icon using Sun's iconedit..               */
/*  Place the icon file between the initialisation { } for icon_bitmap_bits */
/*  Divide the 4 digit hex numbers into two 2 digit hex numbers using:      */
/*			s/0x\(..\)\(..\)/0x\1,0x\2/g                        */
/*  over the range of lines containing the bitmap data.                     */
/*  The display program reorders the bits within the bytes for you.         */
/*                                                                          */
/****************************************************************************/

static int icon_bitmap_width = 64 ;
static int icon_bitmap_height = 64 ;
static char *initial_name = "Napier88" ;
static unsigned char icon_bitmap_bits[] = {
/* Format_version=1, Width=64, Height=64, Depth=1, Valid_bits_per_item=16
 */
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x7A,0x83,0xF8,0x00,0x00,0x00,
	0x00,0x01,0x80,0xE6,0x07,0x06,0x00,0x00,
	0x00,0x03,0x00,0x1C,0x00,0xDA,0xC0,0x00,
	0x00,0x02,0x00,0x18,0x00,0x70,0x30,0x00,
	0x00,0x02,0x00,0x08,0x00,0x20,0x08,0x00,
	0x00,0x02,0x00,0x08,0x00,0x20,0x04,0x00,
	0x00,0xFF,0x00,0x00,0x00,0x00,0x02,0x00,
	0x01,0x01,0x00,0x00,0x00,0x00,0x02,0x00,
	0x03,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
	0x02,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
	0x02,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
	0x06,0x00,0x00,0x00,0x00,0x00,0x02,0x00,
	0x06,0x00,0x00,0x00,0x00,0x00,0x05,0x00,
	0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
	0x0F,0x80,0x00,0x00,0x00,0x00,0x00,0xC0,
	0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
	0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x40,
	0x10,0x00,0x00,0x0F,0xF0,0x00,0x00,0x20,
	0x10,0x00,0x00,0x70,0x0E,0x00,0x00,0x20,
	0x08,0x00,0x01,0x80,0x01,0x80,0x00,0x60,
	0x0C,0x00,0x02,0x00,0x00,0x40,0x00,0xC0,
	0x07,0x00,0x0C,0x00,0x00,0x30,0x03,0x00,
	0x00,0x80,0x10,0x00,0x00,0x08,0x02,0x00,
	0x00,0x80,0x10,0x00,0x00,0x0C,0x02,0x00,
	0x00,0x40,0x18,0x7F,0xFE,0x13,0xCC,0x00,
	0x00,0x3F,0xE6,0x80,0x01,0x60,0x30,0x00,
	0x00,0x00,0x01,0x80,0x01,0x80,0x00,0x00,
	0x00,0x00,0x00,0x80,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x80,0x01,0x00,0x00,0x00,
	0x00,0x00,0x00,0x40,0x02,0x00,0x00,0x00,
	0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xF8,0x00,
	0x00,0x10,0x00,0x00,0x00,0x00,0x04,0x00,
	0x00,0x10,0x00,0x00,0x00,0x00,0x04,0x00,
	0x00,0x10,0x00,0x00,0x00,0x00,0x04,0x00,
	0x00,0x10,0x00,0x00,0x00,0x00,0x04,0x00,
	0x00,0x08,0x00,0x00,0x00,0x00,0x08,0x00,
	0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,
	0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
	0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
	0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
	0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
	0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
	0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,
	0x00,0x00,0x00,0x71,0x18,0x00,0x00,0x00,
	0x00,0x00,0x07,0xC1,0x06,0x00,0x00,0x00,
	0x00,0x00,0x3C,0x01,0x03,0x80,0x00,0x00,
	0x00,0x01,0xC0,0x01,0x00,0xF0,0x00,0x00,
	0x00,0x02,0x00,0x01,0x80,0x0E,0x00,0x00,
	0x00,0x06,0x00,0x06,0x60,0x03,0xB0,0x00,
	0x00,0x19,0x80,0x04,0x20,0x00,0xCC,0x00,
	0x00,0x10,0x80,0x06,0x60,0x00,0x84,0x00,
	0x00,0x19,0x80,0x05,0xA0,0x00,0xCC,0x00,
	0x00,0x16,0x80,0x04,0x20,0x00,0xB4,0x00,
	0x00,0x10,0x80,0x04,0x20,0x00,0x84,0x00,
	0x00,0x10,0x80,0x06,0x60,0x00,0x84,0x00,
	0x00,0x19,0x80,0x01,0x80,0x00,0xCC,0x00,
	0x00,0x06,0x00,0x00,0x00,0x00,0x30,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
				};

