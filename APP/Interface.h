#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#define X_SEAT 400
#define Y_SEAT 10

#define lcd_rectangle(x,y,w,h,color)	LCD_Fill(x,y,(x+w),(y+h),color)

void Vitta_show(void);
void Vitta_cursor_show(float value);
#endif
