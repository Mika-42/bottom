#ifndef UI_FILTER_H
#define UI_FILTER_H

#include "processus_array.h"
#include "error.h"
#include "ui.h"

error_code_t DO_NOT_IGNORE ui_filter_proc(const processus_array_t *array, user_selection_t *s);

#endif /* UI_FILTER_H */
