#ifndef UI_EVENT_DISPATCHER_H
#define UI_EVENT_DISPATCHER_H

#include "processus_array.h"
#include "ui.h"
#include "error.h"

error_code_t	DO_NOT_IGNORE ui_event_dispatcher_normal(const processus_array_t *array, const int ch,ui_t *ui, user_selection_t *s);
void		ui_event_dispatcher_help(const int ch, ui_t *ui, user_selection_t *s);
void		ui_event_dispatcher_search(const int ch, ui_t *ui, user_selection_t *s);
void		ui_event_dispatcher_sort(const int ch, user_selection_t *s);
int		ui_event_dispatcher_global(const int ch);

#endif /* UI_EVENT_DISPATCHER_H */
