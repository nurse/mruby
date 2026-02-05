MRuby::Build.new do |conf|
  toolchain :clang

  conf.gem core: "mruby-bin-mruby"
  conf.gem core: "mruby-eval"
  conf.gem core: "mruby-struct"
  conf.gem core: "mruby-errno"
  conf.gem core: "mruby-string-ext"
  conf.gem core: "mruby-fiber"
  conf.gem core: "mruby-enumerator"
  conf.gem core: "mruby-hash-ext"
  conf.gem core: "mruby-kernel-ext"
  conf.gem core: "mruby-object-ext"
  conf.gem core: "mruby-metaprog"
  conf.gem core: "mruby-io"
  conf.gem core: "mruby-time"
  conf.gem core: "mruby-dir"
  conf.gem :gemdir => "#{__dir__}/mrbgems/mruby-process"
  conf.gem :gemdir => "#{__dir__}/mrbgems/mruby-file-stat"
end
