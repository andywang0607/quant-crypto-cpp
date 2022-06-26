set(targets Receiver QuoteNode BybitQuote)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
