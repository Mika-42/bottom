#include "processus_signal.h"
#include "processus_sort.h"
#include "ssh_processus.h"
#include "thread.h"
#include "ui_event_dispatcher.h"
#include "ui_page.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

/* Notes:
 *	array[0] is ALWAYS the local machine !
 */

static const proc_compare_t sort_func[2][header_element_count] = {
    {pid_dsc, user_dsc, name_dsc, state_dsc, ram_dsc, cpu_dsc, time_dsc},
    {pid_asc, user_asc, name_asc, state_asc, ram_asc, cpu_asc, time_asc}};

static atomic_int thread_counter = 0;
atomic_bool running = true;
pthread_mutex_t ssh_lock = PTHREAD_MUTEX_INITIALIZER;

void *manager_task(void *arg) {

  pthread_setname_np(pthread_self(), "bttm-manager");

  if (!arg) {
    return nullptr;
  }

  thread_args_t *args = arg;
  user_selection_t *s = args->selection;
  error_code_t err = SUCCESS;

  while (atomic_load_explicit(&running, memory_order_acquire)) {

    pthread_mutex_lock(&s->lock);
    const int machine = s->machine_selected;
    const size_t curr = s->selected;
	const proc_event_t evt = s->event;
    const int index = atomic_load_explicit(&args[machine].array->active, memory_order_acquire);
    processus_array_t *proc_list = &args[machine].array->buffer[index];
	
    ssh_session session = args[machine].session;

    if (curr >= proc_list->size) {
      pthread_mutex_unlock(&s->lock);
  //    nanosleep(&proc_thread_time_interval, nullptr);
      continue;
    }
    processus_t *curr_el = &proc_list->data[curr];
    pthread_mutex_unlock(&s->lock);

    switch (evt) {
    case PAUSE_CONTINUE: {

      if (curr_el->state == 'T') {

        if (session) {
			pthread_mutex_lock(&ssh_lock);
			ssh_cont_processus(session, curr_el->pid);
			pthread_mutex_unlock(&ssh_lock);
		} else proc_cont(curr_el);

      } else {
        if (session) {
				pthread_mutex_lock(&ssh_lock);
          ssh_stop_processus(session, curr_el->pid);
		  	pthread_mutex_unlock(&ssh_lock);
		} else proc_stop(curr_el);

      }
    } break;

    case TERMINATE: {

      if (session) {
		pthread_mutex_lock(&ssh_lock);
	 	  ssh_term_processus(session, curr_el->pid);
		pthread_mutex_unlock(&ssh_lock); 
	  } else proc_term(curr_el);

    } break;

    case KILL: {
      if (session) {
		pthread_mutex_lock(&ssh_lock);
        ssh_kill_processus(session, curr_el->pid);
		pthread_mutex_unlock(&ssh_lock);
		} else proc_kill(curr_el);
    } break;

    case RELOAD: {
      if (session) {
		  pthread_mutex_lock(&ssh_lock);
		  ssh_restart_processus(session, curr_el);
		pthread_mutex_unlock(&ssh_lock);
	  } else proc_restart(curr_el);
    } break;

    default:
      break;
    }

    if (evt != NOTHING) {
	    pthread_mutex_lock(&s->lock);
		s->event = NOTHING;
	    pthread_mutex_unlock(&s->lock);

    }

   nanosleep(&proc_thread_time_interval, nullptr);
  }

printf("bttm-manager exit : %s\n", err_to_str(err));

  return nullptr;
}

void *proc_task(void *arg) {
  int id = atomic_fetch_add(&thread_counter, 1);
  char name[16];
  snprintf(name, sizeof(name), "bttm-worker-%d", id);

  pthread_setname_np(pthread_self(), name);

  if (!arg) {
    return nullptr;
  }

  thread_args_t *args = arg;
  user_selection_t *s = args->selection;
  double_buffer_t *db = args->array;
  ssh_session session = args->session;

  error_code_t err = SUCCESS;

  proc_array_init(&db->buffer[0]);
  proc_array_init(&db->buffer[1]);

  while (atomic_load_explicit(&running, memory_order_acquire)) {

    pthread_mutex_lock(&s->lock);
    const int header = s->header_selected;
    const sort_type_t sort = s->sort;
    pthread_mutex_unlock(&s->lock);

    const int index =
        1 - atomic_load_explicit(&db->active, memory_order_acquire);
    processus_array_t *proc_list = &db->buffer[index];

    if (session) {
  pthread_mutex_lock(&ssh_lock);
		err = ssh_array_update(proc_list, session);
		pthread_mutex_unlock(&ssh_lock);
    } else {
      err = proc_array_update(proc_list);

      if (err != SUCCESS) {
        break;
      }
      if (proc_array_get_cpu(&db->buffer[1 - index], &db->buffer[index]) !=
          SUCCESS) {
        break;
      }
    }

    if ((size_t)header < header_element_count) {
      proc_array_sort(proc_list, sort_func[sort][header]);
    }
    nanosleep(&proc_thread_time_interval, nullptr);

    atomic_store_explicit(&db->active, index, memory_order_release);
  }

  proc_array_free(&db->buffer[0]);
  proc_array_free(&db->buffer[1]);

  printf("%s exit : %s\n", name, err_to_str(err));

  return nullptr;
}

void *ui_task(void *arg) {

  pthread_setname_np(pthread_self(), "bttm-ui");

  if (!arg) {
    return nullptr;
  }

  ui_t ui = {0};
  ui_init(&ui);

  size_t select = 0;

  thread_args_t *args = arg;
  user_selection_t *s = args->selection;
  double_buffer_t *machines = args->array;
  struct timespec last_update = {0, 0};

  while (atomic_load_explicit(&running, memory_order_acquire)) {

    const int ch = getch();

    pthread_mutex_lock(&s->lock);

    if (ch == KEY_F(9)) {
      pthread_mutex_unlock(&s->lock);
      atomic_store_explicit(&running, false, memory_order_release);
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

      // update header
      mvwprintw(ui.header, 1, 0, proc_array_tab_header[1],
                args[s->machine_selected].machine_name, proc_list->size);

      // non-bloking delay 200ms
      struct timespec now;
      clock_gettime(CLOCK_MONOTONIC, &now);
      if ((now.tv_sec - last_update.tv_sec) * 1000 +
              (now.tv_nsec - last_update.tv_nsec) / 1000000 >=
          250) {
        if (ui_show_proc(proc_list, &ui, s) != SUCCESS) {

          pthread_mutex_unlock(&s->lock);
          atomic_store_explicit(&running, false, memory_order_release);
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
  atomic_store_explicit(&running, false, memory_order_release);

  return nullptr;
}
