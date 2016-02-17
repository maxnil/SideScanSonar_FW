/*
 * ssd1322.h
 *
 * Created: 2015-01-09 17:52:23
 *  Author: Max
 */ 


#ifndef SSD1322_H_
#define SSD1322_H_

#define SSD1322_ENABLE_GRAYSCALE				0x00
#define SSD1322_SET_COLUMN_ADDRESS				0x15
#define SSD1322_WRITE_RAM_CMD					0x5C
#define SSD1322_READ_RAM_CMD					0x5D
#define SSD1322_SET_ROW_ADDRESS					0x75
#define SSD1322_SET_REMAP						0xA0
#define SSD1322_SET_DISPLAY_START_LINE			0xA1
#define SSD1322_SET_DISPLAY_OFFSET				0xA2
#define SSD1322_DISPLAY_MODE_OFF				0xA4
#define SSD1322_DISPLAY_MODE_ON					0xA5
#define SSD1322_DISPLAY_MODE_NORMAL				0xA6
#define SSD1322_DISPLAY_MODE_INVERSE			0xA7
#define SSD1322_ENABLE_PARTIAL_DISPLAY			0xA8
#define SSD1322_EXIT_PARTIAL_DISPLAY			0xA9
#define SSD1322_FUNCTION_SELECTION				0xAB
#define SSD1322_SET_SLEEP_MODE_ON				0xAE
#define SSD1322_SET_SLEEP_MODE_OFF				0xAF
#define SSD1322_SET_PHASE_LENGTH				0xB1
#define SSD1322_SET_DISPLAY_CLOCK				0xB3
#define SSD1322_SET_DISPLAY_ENHANCEMENT_A		0xB4
#define SSD1322_SET_GPIO						0xB5
#define SSD1322_SET_SECOND_PRECHARE_PERIOD		0xB6
#define SSD1322_SET_GRAYSCALE_TABLE				0xB8
#define SSD1322_SET_DEFAULT_LINEAR_GRAYSCALE	0xB9
#define SSD1322_SET_PRECHARGE_VOLTAGE			0xBB
#define SSD1322_SET_VCOMH_VOLTAGE				0xBE
#define SSD1322_SET_CONTRAST_CONTROL			0xC1
#define SSD1322_MASTER_CONTRAST_CONTROL			0xC7
#define SSD1322_SET_MULTIPLEX_RATIO				0xCA
#define SSD1322_SET_DISPLAY_ENHANCEMENT_B		0xD1
#define SSD1322_SET_COMMAND_LOCK				0xFD

#define SSD1322_START_COL	28
#define SSD1322_NR_COL		64
#define SSD1322_START_ROW	0
#define SSD1322_NR_ROW		64

#define SSD1322_RESET_LENGTH_US	5	// Reset pulse length (us)

/* Initialize the SSD1322 */
void ssd1322_init(void);

/* Fill display with checkerboard pattern */
void ssd1322_checkerboard(uint8_t pat);

/* Set entire display on/off */
void ssd1322_set_display(uint8_t mode);

void ssd1322_write_text(const char *string, int row, int col);

void ssd1322_show_image(const uint8_t *image);

#endif /* SSD1322_H_ */