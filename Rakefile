
require 'rake/clean'

require 'open3'

c_compiler = "cc"
if ENV.key?("CC")
  c_compiler = ENV["CC"]
end

cflags = ["-I.", "-O3"]
ldflags = []

task :default => ['libavl.so', 'libavl.a']

directory "shared"
directory "static"
CLEAN << "shared"
CLEAN << "static"

file 'libavl.so'
file 'libavl.a'

CLOBBER << "libavl.so"
CLOBBER << "libavl.a"

c_files = Dir['./**/*.c']
c_files.each do |fname|
  if fname.start_with?("./test")
    # Skip files in the test directory
    next
  end

  # Run the compiler dependency command
  stdout, stderr, status = Open3.capture3("#{c_compiler} #{cflags.join(" ")} -MM #{fname}") 
  if status != 0
    puts "Failed to run #{c_compiler} -MM on #{fname}"
  end

  # Grab the name of the object
  obj_depend_fmt = /(\S+\.o):/
  obj = stdout.match(obj_depend_fmt)[1]

  # Grab the names of the header dependencies
  headers = stdout.scan(/\S+\.h(?!:)/)

  obj_sh = "shared/" + obj
  obj_st = "static/" + obj

  # Associate the object files with the dependencies
  file obj_sh => [__FILE__, "shared"] + headers
  file obj_st => [__FILE__, "static"] + headers

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
rule ".o" => lambda {|objfile| File.basename(objfile).ext(".c")} do |t|
  cmd = "#{c_compiler} -c -o #{t.name} #{cflags.join(" ")} #{t.source}"
  if t.name.start_with?("shared")
    cmd += " -fPIC"
  end
  #puts cmd

  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts "Failed to compile #{t.source}"
    puts stderr
  end
end

file "libavl.so" do |t|
  cmd = "#{c_compiler} -shared -o #{t.name} #{cflags.join(" ")} #{ldflags.join(" ")} #{t.prerequisites.join(" ")}"
  #puts cmd
  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts "Failed to link #{t.name} with #{t.prerequisites.join(" ")}"
    puts stderr
  end
end


file "libavl.a" do |t|
  cmd = "ar rcs #{t.name} #{t.prerequisites.join(" ")}"
  #puts cmd
  stdout, stderr, status = Open3.capture3(cmd) 
  if status != 0
    puts "Failed to link #{t.name} with #{t.prerequisites.join(" ")}"
    puts stderr
  end
end

