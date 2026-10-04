#ifndef PCD8544_H
#define PCD8544_H

#include <stdint.h>

#define PCD8544_WIDTH   84
#define PCD8544_HEIGHT  48
#define PCD8544_COLS    14   /* caractères par ligne (6 px chacun) */
#define PCD8544_ROWS    6    /* lignes de texte (8 px chacune) */

/* contrast : 0..127 (valeur typique 0x3F, à ajuster selon le module) */
int  pcd8544_init(uint8_t contrast);
void pcd8544_clear(void);
void pcd8544_print(uint8_t col, uint8_t row, const char *s);
int  pcd8544_update(void);

#endif