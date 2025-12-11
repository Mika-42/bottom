#ifndef UI_EVENT_DISPATCHER_H
#define UI_EVENT_DISPATCHER_H

#include "processus_array.h"
#include "ui.h"

typedef int (*processus_callback_t)(processus_t *);
processus_callback_t	ui_event_dispatcher_normal(const processus_array_t *array[], const int ch,ui_t *ui, user_selection_t *s);
void			ui_event_dispatcher_help(const int ch, ui_t *ui, user_selection_t *s);
void			ui_event_dispatcher_search(const int ch, ui_t *ui, user_selection_t *s);

#endif /* UI_EVENT_DISPATCHER_H */
