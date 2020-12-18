#ifndef _CHAT_INPUT_H_
#define _CHAT_INPUT_H_

extern void input_clean_stdin (void);

// returns a newly allocated c string
extern char *input_get_line (void);

#endif