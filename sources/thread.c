#include "processus_signal.h"
#include "processus_sort.h"
#include "thread.h"
#include "ui_event_dispatcher.h"
#include "ui_page.h"

/* Notes:
 *	array[0] is ALWAYS the local machine !
 */

pthread_t proc_thread;
pthread_t ui_thread;

void *proc_task(void *arg) {

  if (!arg)
    return nullptr;

  thread_args_t *args = arg;

  double_buffer_t *db = &args->array[0];
  user_selection_t *s = &args->selection;

  static const proc_compare_t sort_func[2][header_element_count] = {
      {pid_dsc, user_dsc, name_dsc, state_dsc, ram_dsc, cpu_dsc, time_dsc},
      {pid_asc, user_asc, name_asc, state_asc, ram_asc, cpu_asc, time_asc}};

  while (atomic_load_explicit(&args->running, memory_order_acquire)) {

    pthread_mutex_lock(&s->lock);
    int header = s->header_selected;
    sort_type_t sort = s->sort;
    proc_event_t evt = s->event;
    size_t curr = s->selected;
    pthread_mutex_unlock(&s->lock);

    const int index =
        1 - atomic_load_explicit(&db->active, memory_order_acquire);
    processus_array_t *proc_list = &db->buffer[index];
    auto curr_el = &proc_list->data[curr];

    switch (evt) {
    case PAUSE_CONTINUE:
      (curr_el->state == 'T') ? proc_cont(curr_el) : proc_stop(curr_el);
      break;
    case TERMINATE:
      proc_term(curr_el);
      break;
    case KILL:
      proc_kill(curr_el);
      break;
    case RELOAD:
      proc_restart(curr_el);
      break;
    default:
      break;
    }

    if (evt != NOTHING) {
      pthread_mutex_lock(&s->lock);
      s->event = NOTHING;
      pthread_mutex_unlock(&s->lock);
    }

    if (proc_array_update(proc_list) != SUCCESS) {
      atomic_store_explicit(&args->running, false, memory_order_release);
      break;
    }

    if (proc_array_get_cpu(&db->buffer[1 - index], &db->buffer[index]) !=
        SUCCESS) {
      atomic_store_explicit(&args->running, false, memory_order_release);
      break;
    }

    if ((size_t)header < header_element_count)
      proc_array_sort(proc_list, sort_func[sort][header]);

    nanosleep(&proc_thread_time_interval, nullptr);

    atomic_store_explicit(&db->active, index, memory_order_release);
  }

  return nullptr;
}

void *ui_task(void *arg) {

  if (!arg)
    return nullptr;

  ui_t ui = {0};
  ui_init(&ui);

  size_t select = 0;

  thread_args_t *args = arg;
  user_selection_t *s = &args->selection;
  double_buffer_t *machines = args->array;
  struct timespec last_update = {0, 0};

  while (atomic_load_explicit(&args->running, memory_order_acquire)) {

    const int ch = getch();

    pthread_mutex_lock(&s->lock);

    if (ch == KEY_F(9)) {
      pthread_mutex_unlock(&s->lock);
      atomic_store_explicit(&args->running, false, memory_order_release);
      break;
    }

    double_buffer_t *db = &machines[s->machine_selected];
    int index = atomic_load_explicit(&db->active, memory_order_acquire);
    processus_array_t *proc_list = &db->buffer[index];

    if (s->mode == HELP) {
      ui.ui_scroll_y = 0;
      ui_event_dispatcher_help(ch, &ui, s);
      select = 0;
    } else {
      if (s->mode == SEARCH) {
        ui_event_dispatcher_search(ch, &ui, s);
      } else {
        ui_event_dispatcher_normal(proc_list, ch, &ui, s);
      }

      // non-bloking delay 200ms
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
      if ((now.tv_sec - last_update.tv_sec) * 1000 +
              (now.tv_nsec - last_update.tv_nsec) / 1000000 >=
          250) {
        if (ui_show_proc(proc_list, &ui, s) != SUCCESS) {

          pthread_mutex_unlock(&s->lock);
          atomic_store_explicit(&args->running, false, memory_order_release);
          break;
        }
        last_update = now;
      }

      ui_select(&ui, s);

      select = s->selected;
    }
    const int scroll_factor = ui_event_dispatcher_global(ch);

    ui_scroll(&ui, scroll_factor, select);

    ui_update(&ui, proc_list->size);

    pthread_mutex_unlock(&s->lock);

    nanosleep(&ui_thread_time_interval, nullptr);
  }

  ui_deinit(&ui);

  return nullptr;
}

error_code_t init_data(thread_args_t *args) {

  if (!args)
    return NULLPTR_PARAMETER_ERROR;

  user_selection_t *s = &args->selection;

  s->selected = 0;
  s->machine_selected = 0;
  s->header_selected = 0;
  s->sort = ASC;
  s->mode = NORMAL;
  s->event = NOTHING;
  s->max_machine = 2;
  s->input[0] = '\0';
  s->input_length = 0;
  s->indices.data = nullptr;
  s->indices.size = 0;
  s->indices.capacity = 0;
  atomic_store_explicit(&args->running, true, memory_order_release);

  if (pthread_mutex_init(&s->lock, nullptr) != 0) {
    return MEMORY_ALLOCATION_FAILED;
  }

  args->array = calloc(s->max_machine, sizeof(*args->array));
  if (!args->array)
    return MEMORY_ALLOCATION_FAILED;

  double_buffer_t *array = args->array;

  for (size_t i = 0; i < s->max_machine; ++i) {
    proc_array_init(&array[i].buffer[0]);
    proc_array_init(&array[i].buffer[1]);

    atomic_store_explicit(&array[i].active, 0, memory_order_release);
  }

  return SUCCESS;
}

error_code_t start_threads(thread_args_t *args) {

  if (!args)
    return NULLPTR_PARAMETER_ERROR;

  if (pthread_create(&proc_thread, nullptr, proc_task, args) != 0) {
    return THREAD_FAILED;
  }

  if (pthread_create(&ui_thread, nullptr, ui_task, args) != 0) {
    atomic_store_explicit(&args->running, false, memory_order_release);
    pthread_join(proc_thread, nullptr);
    return THREAD_FAILED;
  }

  return SUCCESS;
}

void release_data(thread_args_t *args) {

  if (!args)
    return;

  if (args->array) {
    for (size_t i = 0; i < args->selection.max_machine; ++i) {
      proc_array_free(&args->array[i].buffer[0]);
      proc_array_free(&args->array[i].buffer[1]);
    }

    free(args->array);
  }

  pthread_mutex_destroy(&args->selection.lock);
  ui_index_array_free(&args->selection.indices);
}

error_code_t bottom_ui(thread_args_t *args) {

  if (!args)
    return NULLPTR_PARAMETER_ERROR;

  error_code_t err = init_data(args);

  if (err != SUCCESS) {
    release_data(args);
    return err;
  }
  err = start_threads(args);

  if (err != SUCCESS) {
    release_data(args);
    return err;
  }

  pthread_join(proc_thread, nullptr);
  pthread_join(ui_thread, nullptr);

  release_data(args);

  return SUCCESS;
}
