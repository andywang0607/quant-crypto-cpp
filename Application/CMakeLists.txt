set(targets QuoteReceiver)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
