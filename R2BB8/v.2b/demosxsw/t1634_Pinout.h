#ifndef T1634_PINOUT_H
#define T1634_PINOUT_H

#define PA0 8
#define PA1 7
#define PA2 6
#define PA3 5 /* A0 */
#define PA4 4 /* A1 */
#define PA5 3 /* A2/OC0B */
#define PA6 2 /* A3/OC1B */
#define PA7 1 /* A4 */

#define PB0 0 /* A5 */
#define PB1 16 /* A6/DI (MOSI-S|MISO-M)/SDA */
#define PB2 15 /* A7/DO (MISO-S|MOSI-M) */
#define PB3 14 /* A8/OC1A */

#define PC0 13 /* A9/OC0A */
#define PC1 12 /* A10/SCK/SCL */
#define PC2 11 /* A11/INT0 */
#define PC3 17 /* RST */
#define PC4 10 /* XTAL2 */
#define PC5 9 /* XTAL1 */

#define USI_DO 15 /* PB2 */
#define USI_DI 16 /* PB1 */
#define SCK 12 /* PC1 */
#endif //T1634_PINOUT_H
