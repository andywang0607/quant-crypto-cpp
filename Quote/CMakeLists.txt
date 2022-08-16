set(targets Receiver QuoteNode QuoteApi BybitQuote)
foreach(target ${targets})
  add_subdirectory(${target})
endforeach(target)
