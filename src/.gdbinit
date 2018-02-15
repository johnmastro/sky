# .gdbinit - Conveniences for inspecting Sky values in GDB

define pp
  set $tmp = $arg0
  call println(stdout, $tmp)
end

define pr
  pp $
end
