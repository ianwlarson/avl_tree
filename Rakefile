
require 'rake/clean'

require 'open3'

c_compiler = "cc"
if ENV.key?("CC")
  c_compiler = ENV["CC"]
end

cflags = ["-I.", "-Os", "-ggdb3", "-coverage"]
ldflags = []

file 'libavl.so'
file 'libavl.a'

task :staticlib => ['libavl.a']
task :sharedlib => ['libavl.so']

task :default => [:staticlib, :sharedlib]

[:staticlib, :sharedlib].each do |t|
  tsk = task(t)
  #puts tsk.methods
  def tsk.timestamp
      Time.at 0
  end
end
#def (task(:sharedlib)).timestamp
#  Time.at 0
#end

directory "shared"
directory "static"
CLEAN << "shared"
CLEAN << "static"

CLOBBER << "libavl.so"
CLOBBER << "libavl.a"

task :test => ['test/avltest']
file 'test/avltest' => ['test/avltest.o', :sharedlib]

CLOBBER << 'test/avltest'

gcov_files = Dir['**/*.gcda'] + Dir['**/*.gcno']
gcov_files.each { |f| CLEAN << f }
CLEAN << "out"
CLEAN << "coverage.info"

c_files = Dir['**/*.c']
c_files.each do |fname|
  # Run the compiler dependency command
  stdout, stderr, status = Open3.capture3("#{c_compiler} #{cflags.join(" ")} -MM #{fname}") 
  if status != 0
    puts "Failed to run #{c_compiler} -MM on #{fname}"
    fail
  end

  # Grab the name of the object
  obj_depend_fmt = /(\S+\.o):/
  obj = stdout.match(obj_depend_fmt)[1]

  # Grab the names of the header dependencies
  headers = stdout.scan(/\S+\.h(?!:)/)

  if File.dirname(fname) == "test"
    obj = File.join("test", obj)
    file obj => [fname, __FILE__] + headers
    CLEAN << obj
    next
  end

  obj_sh = "shared/" + obj
  obj_st = "static/" + obj

  # Associate the object files with the dependencies
  file obj_sh => [fname, __FILE__, "shared"] + headers
  file obj_st => [fname, __FILE__, "static"] + headers

  file 'libavl.so' => obj_sh
  file 'libavl.a' => obj_st
end

#
# Create *.c -> *.o rule
#
# Because I'm using subdirectories (shared/static) I need to use a lambda
# to find the actual C file. I am just using basename and swapping the extension
# but it could easily be made more sophisticated.
#
def find_cfile(objfile)
  cname = File.basename(objfile).ext('.c')
  dirname = File.dirname(objfile)
  if dirname == "test"
    return File.join(dirname, cname)
  end
  return cname
end

rule ".o" => lambda {|objfile| find_cfile(objfile)} do |t|
  cmd = "#{c_compiler} -c -o #{t.name} #{cflags.join(" ")} #{t.source}"
  if t.name.start_with?("shared")
    cmd += " -fPIC"
  end
  puts cmd

  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts "Failed to compile #{t.source}"
    puts stderr
    fail
  end
end

file "libavl.so" do |t|
  cmd = "#{c_compiler} -shared -o #{t.name} #{cflags.join(" ")} #{ldflags.join(" ")} #{t.prerequisites.join(" ")}"
  puts cmd

  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts stderr
    puts "Failed to link #{t.name} with #{t.prerequisites.join(" ")}"
    fail
  end
end


file "libavl.a" do |t|
  cmd = "ar rcs #{t.name} #{t.prerequisites.join(" ")}"
  puts cmd

  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts stderr
    puts "Failed to link #{t.name} with #{t.prerequisites.join(" ")}"
    fail
  end
end

file "test/avltest" do |t|
  prereqs = t.prerequisites.select{ |x| File.exist? x }
  cmd = "#{c_compiler} -o #{t.name} #{cflags.join(" ")} #{ldflags.join(" ")} -L. -lavl -lcmocka #{prereqs.join(" ")} -Wl,-rpath=#{File.expand_path(".")}"
  puts cmd
  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts stderr
    puts "Failed to link #{t.name} with #{t.prerequisites.join(" ")}"
    fail
  end
end
