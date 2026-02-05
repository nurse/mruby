#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/string.h>

#include <sys/stat.h>
#include <unistd.h>

struct mrb_filestat {
  struct stat st;
};

static void
filestat_free(mrb_state *mrb, void *p)
{
  if (p) {
    mrb_free(mrb, p);
  }
}

static const struct mrb_data_type filestat_type = { "File::Stat", filestat_free };

static mrb_value
filestat_mtime(mrb_state *mrb, mrb_value self)
{
  struct mrb_filestat *fs = (struct mrb_filestat *)mrb_data_get_ptr(mrb, self, &filestat_type);
  if (!fs) return mrb_nil_value();
  struct RClass *time_cls = NULL;
  if (mrb->object_class) {
    time_cls = mrb_class_get(mrb, "Time");
  }
  if (time_cls) {
    return mrb_funcall(mrb, mrb_obj_value(time_cls), "at", 1, mrb_fixnum_value((mrb_int)fs->st.st_mtime));
  }
  return mrb_fixnum_value((mrb_int)fs->st.st_mtime);
}

static mrb_value
filestat_mtime_i(mrb_state *mrb, mrb_value self)
{
  struct mrb_filestat *fs = (struct mrb_filestat *)mrb_data_get_ptr(mrb, self, &filestat_type);
  if (!fs) return mrb_nil_value();
  return mrb_fixnum_value((mrb_int)fs->st.st_mtime);
}

static mrb_value
file_stat_common(mrb_state *mrb, int use_lstat)
{
  const char *path;
  mrb_get_args(mrb, "z", &path);

  struct stat st;
  int r = use_lstat ? lstat(path, &st) : stat(path, &st);
  if (r != 0) {
    return mrb_nil_value();
  }

  struct mrb_filestat *fs = (struct mrb_filestat *)mrb_malloc(mrb, sizeof(struct mrb_filestat));
  fs->st = st;
  return mrb_obj_value(mrb_data_object_alloc(mrb, mrb_class_get_under(mrb, mrb_class_get(mrb, "File"), "Stat"), fs, &filestat_type));
}

static mrb_value
file_stat(mrb_state *mrb, mrb_value self)
{
  return file_stat_common(mrb, 0);
}

static mrb_value
file_lstat(mrb_state *mrb, mrb_value self)
{
  return file_stat_common(mrb, 1);
}

static mrb_value
file_mtime(mrb_state *mrb, mrb_value self)
{
  const char *path;
  mrb_get_args(mrb, "z", &path);
  struct stat st;
  if (stat(path, &st) != 0) {
    return mrb_nil_value();
  }
  struct RClass *time_cls = NULL;
  if (mrb->object_class) {
    time_cls = mrb_class_get(mrb, "Time");
  }
  if (time_cls) {
    return mrb_funcall(mrb, mrb_obj_value(time_cls), "at", 1, mrb_fixnum_value((mrb_int)st.st_mtime));
  }
  return mrb_fixnum_value((mrb_int)st.st_mtime);
}

void
mrb_mruby_file_stat_gem_init(mrb_state *mrb)
{
  struct RClass *file = mrb_class_get(mrb, "File");
  struct RClass *stat_cls = mrb_define_class_under(mrb, file, "Stat", mrb->object_class);
  MRB_SET_INSTANCE_TT(stat_cls, MRB_TT_DATA);

  mrb_define_method(mrb, stat_cls, "mtime", filestat_mtime, MRB_ARGS_NONE());
  mrb_define_method(mrb, stat_cls, "mtime_i", filestat_mtime_i, MRB_ARGS_NONE());

  mrb_define_class_method(mrb, file, "stat", file_stat, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, file, "lstat", file_lstat, MRB_ARGS_REQ(1));
  mrb_define_class_method(mrb, file, "mtime", file_mtime, MRB_ARGS_REQ(1));
}

void
mrb_mruby_file_stat_gem_final(mrb_state *mrb)
{
  (void)mrb;
}
