set(targets QuotePrinter)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
