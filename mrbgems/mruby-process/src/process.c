#include <mruby.h>
#include <mruby/array.h>
#include <mruby/string.h>
#include <mruby/variable.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static struct RClass *process_status_class = NULL;

static mrb_value
status_exitstatus(mrb_state *mrb, mrb_value self)
{
  mrb_value v = mrb_iv_get(mrb, self, mrb_intern_lit(mrb, "@exitstatus"));
  return v;
}

static mrb_value
process_spawn(mrb_state *mrb, mrb_value self)
{
  mrb_value *argv;
  mrb_int argc;
  mrb_get_args(mrb, "*", &argv, &argc);
  if (argc < 1) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "spawn requires at least 1 argument");
  }

  char **cargv = (char **)mrb_malloc(mrb, sizeof(char *) * (argc + 1));
  for (mrb_int i = 0; i < argc; i++) {
    mrb_value s = mrb_obj_as_string(mrb, argv[i]);
    cargv[i] = mrb_str_to_cstr(mrb, s);
  }
  cargv[argc] = NULL;

  pid_t pid = fork();
  if (pid == 0) {
    execvp(cargv[0], cargv);
    _exit(127);
  }
  mrb_free(mrb, cargv);
  return mrb_fixnum_value((mrb_int)pid);
}

static mrb_value
process_waitpid2(mrb_state *mrb, mrb_value self)
{
  mrb_int pid;
  mrb_get_args(mrb, "i", &pid);

  int status = 0;
  pid_t r = waitpid((pid_t)pid, &status, 0);
  if (r < 0) {
    return mrb_nil_value();
  }

  mrb_value st = mrb_obj_new(mrb, process_status_class, 0, NULL);
  int exitstatus = 0;
  if (WIFEXITED(status)) {
    exitstatus = WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    exitstatus = 128 + WTERMSIG(status);
  } else {
    exitstatus = status;
  }
  mrb_iv_set(mrb, st, mrb_intern_lit(mrb, "@exitstatus"), mrb_fixnum_value(exitstatus));

  mrb_value ary = mrb_ary_new_capa(mrb, 2);
  mrb_ary_push(mrb, ary, mrb_fixnum_value((mrb_int)r));
  mrb_ary_push(mrb, ary, st);
  return ary;
}

void
mrb_mruby_process_gem_init(mrb_state *mrb)
{
  struct RClass *process = mrb_define_module(mrb, "Process");
  process_status_class = mrb_define_class_under(mrb, process, "Status", mrb->object_class);
  mrb_define_method(mrb, process_status_class, "exitstatus", status_exitstatus, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, process, "spawn", process_spawn, MRB_ARGS_ANY());
  mrb_define_class_method(mrb, process, "waitpid2", process_waitpid2, MRB_ARGS_REQ(1));
}

void
mrb_mruby_process_gem_final(mrb_state *mrb)
{
  (void)mrb;
}
