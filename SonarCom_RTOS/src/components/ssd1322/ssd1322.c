/*
 * ssd1322.c
 *
 * Created: 2015-01-09 17:52:37
 *  Author: Max
 */ 

#include <asf.h>

#include "conf_spi_master.h"
//#include "font5x7.h"
//#include "Font_Px16.h"
#include "ssd1322.h"
#include "sonarcom_image.h"


void ssd1322_send_cmd(uint8_t cmd);
void ssd1322_send_data(uint8_t data);
void ssd1322_set_column_address(uint8_t a, uint8_t b);
void ssd1322_set_row_address(uint8_t a, uint8_t b);

void ssd1322_send_cmd2(uint8_t cmd, uint8_t *data, int len);

void ssd1322_set_display(uint8_t mode) {
	ssd1322_send_cmd(mode);
}


void ssd1322_send_cmd2(uint8_t cmd, uint8_t *data, int len) {
	int i;
	
	struct spi_device spi_device_disp = {
		.id = CONF_SPI_DISP_DEVICE_ID
	};
	
	// Select the Display
	spi_select_device(CONF_SPI, &spi_device_disp);

	spi_put(CONF_SPI, 0x000 | (uint16_t)cmd);
	
	for (i = 0; i < len; i++) {
		spi_put(CONF_SPI, 0x100 | (uint16_t)data[i]);
	}
	
	// Deselect the Display
	spi_deselect_device(CONF_SPI, &spi_device_disp);
}


void ssd1322_send_cmd(uint8_t cmd) {
	struct spi_device spi_device_disp = {
		.id = CONF_SPI_DISP_DEVICE_ID
	};
	
	// Select the Display
	spi_select_device(CONF_SPI, &spi_device_disp);

	spi_put(CONF_SPI, 0x000 | (uint16_t)cmd);

	// Deselect the Display
	spi_deselect_device(CONF_SPI, &spi_device_disp);
}

void ssd1322_send_data(uint8_t data) {
	struct spi_device spi_device_disp = {
		.id = CONF_SPI_DISP_DEVICE_ID
	};
	
	// Select the Display
	spi_select_device(CONF_SPI, &spi_device_disp);

	// Send the Status Register Read command followed by a dummy data.
	spi_put(CONF_SPI, 0x100 | (uint16_t)data);

	// Deselect the Display
	spi_deselect_device(CONF_SPI, &spi_device_disp);
}

void ssd1322_set_column_address(uint8_t a, uint8_t b) {
	ssd1322_send_cmd(SSD1322_SET_COLUMN_ADDRESS);
	ssd1322_send_data(a);
	ssd1322_send_data(b);
}

void ssd1322_set_row_address(uint8_t a, uint8_t b) {
	ssd1322_send_cmd(SSD1322_SET_ROW_ADDRESS);
	ssd1322_send_data(a);
	ssd1322_send_data(b);
}


//............................................................................................
// Initialize the SSD1322
//............................................................................................
void ssd1322_init(void) {
	int i;
	
	/* Toggle Display Reset */
	ioport_set_pin_level(DISP_RST_GPIO, DISP_RST_ACTIVE_LEVEL);
	delay_us(SSD1322_RESET_LENGTH_US);
	ioport_set_pin_level(DISP_RST_GPIO, DISP_RST_INACTIVE_LEVEL);
	delay_us(SSD1322_RESET_LENGTH_US);
#if 1
	ssd1322_send_cmd(SSD1322_SET_COMMAND_LOCK);		// Unlock Basic Commands (0x12/0x16)
	ssd1322_send_data(0x12);

	ssd1322_send_cmd(SSD1322_DISPLAY_MODE_OFF);				// Display Off

//	ssd1322_send_cmd(SSD1322_SET_COLUMN_ADDRESS);
//	ssd1322_send_data(0x1C);
//	ssd1322_send_data(0x5B);
	ssd1322_set_column_address(SSD1322_START_COL, SSD1322_START_COL + SSD1322_NR_COL);
		
//	ssd1322_send_cmd(SSD1322_SET_ROW_ADDRESS);
//	ssd1322_send_data(0x00);
//	ssd1322_send_data(0x3F);
	ssd1322_set_row_address(SSD1322_START_ROW, SSD1322_START_ROW + SSD1322_NR_ROW);
	
	ssd1322_send_cmd(SSD1322_SET_DISPLAY_CLOCK);			// Set Clock as 80 Frames/Sec
	ssd1322_send_data(0x91);

	ssd1322_send_cmd(SSD1322_SET_DISPLAY_OFFSET);			// Shift Mapping RAM Counter (0x00~0x3F)
	ssd1322_send_data(0x00);
		
	ssd1322_send_cmd(SSD1322_SET_DISPLAY_START_LINE);		// Set Mapping RAM Display Start Line (0x00~0x7F)
	ssd1322_send_data(0x00);
	
	ssd1322_send_cmd(SSD1322_SET_REMAP);					// Set Horizontal Address Increment
															// Column Address 0 Mapped to SEG0
															// Disable Nibble Remap
															// Scan from COM[N-1] to COM0
															// Disable COM Split Odd Even
															// Enable Dual COM Line Mode
	ssd1322_send_data(0x14);
	ssd1322_send_data(0x11);

	ssd1322_send_cmd(SSD1322_SET_GPIO);						// Disable GPIO Pins Input
	ssd1322_send_data(0x00);

	ssd1322_send_cmd(SSD1322_FUNCTION_SELECTION);			// Enable Internal VDD Regulator
	ssd1322_send_data(0x01);
	
	ssd1322_send_cmd(SSD1322_SET_DISPLAY_ENHANCEMENT_A);	// Enable External VSL
	ssd1322_send_data(0xA0);
	ssd1322_send_data(0xFD);
	
	ssd1322_send_cmd(SSD1322_SET_CONTRAST_CONTROL);			// Set Segment Output Current
	ssd1322_send_data(0x9F);
	
	ssd1322_send_cmd(SSD1322_MASTER_CONTRAST_CONTROL);		// Set Scale Factor of Segment Output Current Control
	ssd1322_send_data(0x0F);
	
	ssd1322_send_cmd(SSD1322_SET_DEFAULT_LINEAR_GRAYSCALE);	// Set default linear gray scale table
	
	ssd1322_send_cmd(SSD1322_SET_PHASE_LENGTH);				// Set Phase 1 as 5 Clocks & Phase 2 as 14 Clocks
	ssd1322_send_data(0xE2);
	
	ssd1322_send_cmd(SSD1322_SET_DISPLAY_ENHANCEMENT_B);	// Enhance Driving Scheme Capability (0x00/0x20)
	ssd1322_send_data(0xA2);
	ssd1322_send_data(0x20);
	
	ssd1322_send_cmd(SSD1322_SET_PRECHARGE_VOLTAGE);		// Set Pre-Charge Voltage Level as 0.60*VCC
	ssd1322_send_data(0x1F);

	ssd1322_send_cmd(SSD1322_SET_SECOND_PRECHARE_PERIOD);	// Set Second Pre-Charge Period as 8 Clocks
	ssd1322_send_data(0x08);

	ssd1322_send_cmd(SSD1322_SET_VCOMH_VOLTAGE);			// Set Common Pins Deselect Voltage Level as 0.86*VCC
	ssd1322_send_data(0x07);

	ssd1322_send_cmd(SSD1322_DISPLAY_MODE_NORMAL);			// Normal Display Mode

	ssd1322_send_cmd(SSD1322_EXIT_PARTIAL_DISPLAY);			// Disable Partial Display
	
	ssd1322_send_cmd(SSD1322_SET_SLEEP_MODE_OFF);			// Set Common Pins Deselect Voltage Level as 0.86*VCC
	
	ssd1322_set_column_address(0, 119);
	ssd1322_set_row_address(0, 127);
	ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

	// Clear entire display RAM
	for (i = 0; i < 120*128; i++) {
		ssd1322_send_data(0x00);	// Pixel 0, 1
		ssd1322_send_data(0x00);	// Pixel 2, 3
	}
#endif
}


void ssd1322_checkerboard(uint8_t pat) {
	int i,j;

	ssd1322_set_column_address(SSD1322_START_COL, SSD1322_START_COL + SSD1322_NR_COL - 1);
	ssd1322_set_row_address(SSD1322_START_ROW, SSD1322_START_ROW + SSD1322_NR_ROW - 1);
	ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

	for (i = 0; i < 100; i++) {
		for (j = 0; j < SSD1322_NR_COL-4; j++) {
			ssd1322_send_data((0xF<<4) | (0<<0));	// Pixel 0, 1
			delay_us(300);
			ssd1322_send_data((0xF<<4) | (0<<0));	// Pixel 2, 3
			delay_us(300);
		}
	}
	
	while (1) {
	ssd1322_set_column_address(SSD1322_START_COL, SSD1322_START_COL+SSD1322_NR_COL-1);
	ssd1322_set_row_address(SSD1322_START_ROW, SSD1322_START_ROW+SSD1322_NR_ROW-1);
	ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

	for (i = 0; i < SSD1322_NR_ROW-20; i++) {
		for (j = 0; j < SSD1322_NR_COL-20; j++) {
			ssd1322_send_data((0<<4) | (0<<0));	// Pixel 0, 1
			delay_us(300);
			ssd1322_send_data((0<<4) | (0<<0));	// Pixel 2, 3
			delay_us(300);
			}
	}

	ssd1322_set_column_address(SSD1322_START_COL+2, SSD1322_START_COL+SSD1322_NR_COL-2);
	ssd1322_set_row_address(SSD1322_START_ROW+2, SSD1322_START_ROW+SSD1322_NR_ROW-2);
	ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

	for (i = 0; i < SSD1322_NR_ROW-20; i++) {
		for (j = 0; j < SSD1322_NR_COL-20; j++) {
			ssd1322_send_data((0xF<<4) | (0xF<<0));	// Pixel 0, 1
			delay_ms(1);
			ssd1322_send_data((0xF<<4) | (0xF<<0));	// Pixel 2, 3
			delay_ms(1);
		}
	}

	}
	
//	while(1) {
		ssd1322_set_column_address(SSD1322_START_COL, SSD1322_START_COL + SSD1322_NR_COL);
		ssd1322_set_row_address(SSD1322_START_ROW, SSD1322_START_ROW + SSD1322_NR_ROW);
		ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

		for (i = 0; i < SSD1322_NR_ROW; i++) {
			for (j = 0; j < SSD1322_NR_COL; j++) {
				ssd1322_send_data((0<<4) | (0<<0));	// Pixel 0, 1
				delay_ms(100);
				ssd1322_send_data((0<<4) | (0<<0));	// Pixel 2, 3
				delay_ms(100);
			}
		}

		delay_ms(1000);

		ssd1322_set_column_address(SSD1322_START_COL, SSD1322_START_COL + SSD1322_NR_COL);
		ssd1322_set_row_address(SSD1322_START_ROW, SSD1322_START_ROW + SSD1322_NR_ROW);
		ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

		for (i = 0; i < SSD1322_NR_ROW; i++) {
			for (j = 0; j < SSD1322_NR_COL; j++) {
				ssd1322_send_data((0<<4) | (5<<0));		// Pixel 0, 1
				delay_ms(100);
				ssd1322_send_data((10<<4) | (15<<0));	// Pixel 2, 3
				delay_ms(100);
			}
		}

		delay_ms(1000);
//	}
}


void ssd1322_write_text(const char *string, int row, int col) {
	int i, j;
	uint8_t data;
	char tkn;

	ssd1322_set_column_address(0x1C + col, 0x1C + col + 5);
	ssd1322_set_row_address(row, row + 7);
	ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

	for (i = 0; string[i] != 0; i++) {
		tkn = string[i];
		if ((tkn >= 0x20) && (tkn <= 0x7F)) {
			for (j = 0; j <= 5; j++) {
//				data = Font_12.chars->image.data[j + 5*(tkn - 0x20)];
				ssd1322_send_data((((data & 0x80)>>4)*0xF));
			}
			ssd1322_send_data(0x00);
		}
		string++;
	}
}


void ssd1322_show_image(const uint8_t *image) {
	int i;

	ssd1322_set_column_address(SSD1322_START_COL, SSD1322_START_COL + SSD1322_NR_COL - 1);
	ssd1322_set_row_address(SSD1322_START_ROW, SSD1322_START_ROW + SSD1322_NR_ROW - 1);
	ssd1322_send_cmd(SSD1322_WRITE_RAM_CMD);

	for (i = 0; i < 2*SSD1322_NR_ROW*SSD1322_NR_COL; i++) {
		ssd1322_send_data(image[i]);	// Pixel 0, 1
	}
}