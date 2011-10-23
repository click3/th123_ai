
void key_reset(void);
void key_off(int n);
void key_on(int n);
void set_key_delay(int delay);
void check_lastkey();
void Clear();
void search_key(CHAR_ID operation_char);

extern int muki,gyaku,dis,dfront,dback,ufront,uback,is_dir_front;
extern KeybdEvent keyboard;
