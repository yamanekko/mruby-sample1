#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
// #include "kernel_cfg.h"
#include "target_test.h"

#include <mruby.h>
#include "mruby_sample1.h"

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))
static volatile long_t	volatile_var;
SYSTIM	stime1, stime2;
ulong_t	task_loop;		/* タスク内でのループ回数 */


static void
consume_time(ulong_t ctime)
{
	ulong_t		i;

	for (i = 0; i < ctime; i++) {
		(void) volatile_var;
	}
}


static mrb_value
mrb_sample1_initialize(mrb_state *mrb, mrb_value self)
{
  SVC_PERROR(get_tim(&stime1));
  consume_time(LOOP_REF);
  SVC_PERROR(get_tim(&stime2));
  task_loop = LOOP_REF * 400LU / (ulong_t)(stime2 - stime1) * 1000LU;

	return self;
}

MRB_API mrb_value
mrb_sample1_consume_time_task_loop(mrb_state *mrb, mrb_value self)
{
  consume_time(task_loop);
	return self;
}

MRB_API mrb_value
mrb_sample1_consume_time_loop_ref(mrb_state *mrb, mrb_value self)
{
  consume_time(LOOP_REF);
	return self;
}

MRB_API mrb_value
mrb_sample1_consume_time_1000lu(mrb_state *mrb, mrb_value self)
{
  consume_time(1000LU);
	return self;
}

void
mrb_mruby_sample1_gem_init(mrb_state* mrb) {
#ifdef USE_MODULE
	struct RClass * nucleo_class = mrb_module_get(mrb, "Nucleo");
#else
	struct RClass * nucleo_class = mrb_class_get(mrb, "Nucleo");
#endif
	struct RClass * class = mrb_define_class_under(mrb, nucleo_class, "Sample1", mrb->object_class);

	mrb_define_method(mrb, class, "initialize", mrb_sample1_initialize, MRB_ARGS_NONE());
  mrb_define_method(mrb, class, "consume_time_task_loop", mrb_sample1_consume_time_task_loop, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, class, "consume_time_LOOP_REF", mrb_sample1_consume_time_loop_ref, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, class, "consume_time_1000LU", mrb_sample1_consume_time_1000lu, MRB_ARGS_REQ(1));
}

void
mrb_mruby_sample1_gem_final(mrb_state* mrb) {
  /* finalizer */
}
