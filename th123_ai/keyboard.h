
#pragma once

//	キーコード名	仮想キーコード
#define	VK_0	48
#define	VK_1	49
#define	VK_2	50
#define	VK_3	51
#define	VK_4	52
#define	VK_5	53
#define	VK_6	54
#define	VK_7	55
#define	VK_8	56
#define	VK_9	57
#define	VK_A	65
#define	VK_B	66
#define	VK_C	67
#define	VK_D	68
#define	VK_E	69
#define	VK_F	70
#define	VK_G	71
#define	VK_H	72
#define	VK_I	73
#define	VK_J	74
#define	VK_K	75
#define	VK_L	76
#define	VK_M	77
#define	VK_N	78
#define	VK_O	79
#define	VK_P	80
#define	VK_Q	81
#define	VK_R	82
#define	VK_S	83
#define	VK_T	84
#define	VK_U	85
#define	VK_V	86
#define	VK_W	87
#define	VK_X	88
#define	VK_Y	89
#define	VK_Z	90

void keybd_event3(int code, DWORD dwFlags);
void keybd_event2(int code, DWORD dwFlags);
void kev_call(void);
void kev_clear(void);

extern int key_delay;
extern int key_frame;
